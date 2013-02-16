/*!
  \file
  \brief モニタイベント管理

  \author Satofumi KAMIMURA

  $Id: MonitorEventScheduler.cpp 1483 2009-11-01 23:58:16Z satofumi $
*/

#include "MonitorEventScheduler.h"
#include "Thread.h"
#include "Lock.h"
#include "ticks.h"
#include "delay.h"
#include <list>

using namespace qrk;
using namespace std;


struct MonitorEventScheduler::pImpl
{
  enum {
    ThreadMaxDelay = 200,       // [msec]
  };

  class event_t
  {
  public:
    int ticks;
    ConditionVariable* condition;

    event_t(int ticks_, ConditionVariable* condition_)
      : ticks(ticks_), condition(condition_)
    {
    }

    bool operator < (const event_t& rhs) const
    {
      return ticks < rhs.ticks;
    }
  };

  Lock mutex_;
  Thread thread_;
  list<event_t> event_list_;

  ConditionVariable wait_condition_;
  bool terminated_;


  pImpl(void) : thread_(schedule_thread, this), terminated_(false)
  {
    thread_.run(Thread::Infinity);
  }


  ~pImpl(void)
  {
    terminate();
  }


  static void eventDelay(int delay_msec)
  {
    if (delay_msec > ThreadMaxDelay) {
      // Thread の実装によっては、スレッドは定期的に終了することが望ましい
      // そのための待機状態の最大値
      delay_msec = ThreadMaxDelay;
    }
    delay((delay_msec > 0) ? delay_msec : 1);
  }


  static int schedule_thread(void* args)
  {
    pImpl* obj = static_cast<pImpl*>(args);
    obj->mutex_.lock();
    if (obj->terminated_) {
      obj->mutex_.unlock();
      return 0;
    }

    // 現在の ticks を取得し、より小さい ticks のイベントを順番に起こす
    // !!! 一時停止を考慮すべき
    int current_ticks = ticks();
    list<event_t>::iterator end_it = obj->event_list_.end();
    for (list<event_t>::iterator it = obj->event_list_.begin();
         it != end_it;) {
      if (current_ticks < it->ticks) {
        break;
      }

      if (! it->condition->isWaiting()) {
        // 待ち状態でなければ、次の周期で処理する
        // 以下の delay() 以降に、呼び出し元で待ち状態に遷移するはず
        int delay_msec = it->ticks - current_ticks;
        obj->mutex_.unlock();
        eventDelay(delay_msec);
        return 0;
      }

      ConditionVariable* condition = it->condition;
      it = obj->event_list_.erase(it);
      condition->wakeup();
    }

    if (obj->event_list_.empty()) {
      // 待ち状態に入る
      eventDelay(1);
      obj->wait_condition_.wait(&obj->mutex_);
      obj->mutex_.unlock();

    } else {
      // 次のイベントまで待機させる
      int delay_msec = obj->event_list_.front().ticks - current_ticks;
      obj->mutex_.unlock();
      eventDelay(delay_msec);
    }
    return 0;
  }


  void terminate(void)
  {
    if (! thread_.isRunning()) {
      return;
    }

    mutex_.lock();
    terminated_ = true;

    // 管理しているタスクを全て起床させる
    list<event_t>::iterator end_it = event_list_.end();
    for (list<event_t>::iterator it = event_list_.begin();
         it != end_it;) {
      ConditionVariable* condition = it->condition;
      it = event_list_.erase(it);
      condition->wakeup();
    }
    mutex_.unlock();

    if (wait_condition_.isWaiting()) {
      wait_condition_.wakeup();
    }
    thread_.stop();
    thread_.wait();
  }
};


MonitorEventScheduler::MonitorEventScheduler(void) : pimpl(new pImpl)
{
}


MonitorEventScheduler::~MonitorEventScheduler(void)
{
}


MonitorEventScheduler* MonitorEventScheduler::object(void)
{
  static MonitorEventScheduler singleton_object;
  return &singleton_object;
}


void MonitorEventScheduler::terminate(void)
{
  pimpl->terminate();
}


void MonitorEventScheduler::registerWakeupTicks(ConditionVariable* condition,
                                                int ticks)
{
  pimpl->mutex_.lock();
  if (pimpl->terminated_) {
    pimpl->mutex_.unlock();
    return;
  }

  // イベントリストに追加
  pImpl::event_t event(ticks, condition);
  list<pImpl::event_t> add_event;
  add_event.push_back(event);
  pimpl->event_list_.merge(add_event);
  pimpl->mutex_.unlock();

  // イベントが空の時の追加で、スケジューラのスレッドを起こす
  if (pimpl->wait_condition_.isWaiting()) {
    pimpl->wait_condition_.wakeup();
  }
}


void MonitorEventScheduler::registerDeviceServer(DeviceServer* device)
{
  static_cast<void>(device);
  // !!!

  // !!! 登録後、接続を行わせるために、スレッドを動作させる
  // !!!
}
