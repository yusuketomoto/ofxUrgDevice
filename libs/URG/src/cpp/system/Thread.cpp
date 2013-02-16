/*!
  \file
  \brief スレッド処理のラッパー

  \author Satofumi KAMIMURA

  $Id: Thread.cpp 914 2009-05-20 22:16:35Z satofumi $

  \todo 条件変数を用いた停止、再開の仕組みにすることを検討する
  \todo スレッド関数の実行は、１回に限定すべきかを検討する
*/

#include "Thread.h"
#include "SdlInit.h"
#include "Lock.h"
#include <SDL.h>
#include <SDL_thread.h>

using namespace qrk;


/*!
  \brief Thread の内部クラス
*/
struct Thread::pImpl : private SdlInit
{
  static int function_handler(void* args);

  /*!
    \brief Thread の内部クラス
  */
  class ThreadInfo
  {
  public:
    Lock mutex;
    int (*function)(void *);
    void* args;
    bool pause;
    bool quit;
    size_t times;
    size_t next_times;

    ThreadInfo(int (*fn)(void *), void* thread_args)
      : function(fn), args(thread_args),
        pause(true), quit(false), times(1), next_times(0) {
    }
  };

  SDL_Thread* thread_;
  ThreadInfo thread_info_;


  pImpl(int (*fn)(void *), void* args)
    : thread_(NULL), thread_info_(fn, args) {
  }

  ~pImpl(void) {
    thread_info_.quit = true;
    if (thread_) {
      SDL_KillThread(thread_);
    }
  }


  void run(void)
  {
    if (thread_ == NULL) {
      // スレッドの起動
      thread_info_.pause = false;
      thread_info_.quit = false;
      thread_ = SDL_CreateThread(pImpl::function_handler, &thread_info_);
      return;
    }

    // スレッドの再開
    thread_info_.mutex.lock();
    thread_info_.pause = false;
    thread_info_.mutex.unlock();
  }
};


/*!
  \todo pause 変数の使い方を見直すべき
*/
int Thread::pImpl::function_handler(void* args)
{
  pImpl::ThreadInfo& info = *static_cast<pImpl::ThreadInfo*>(args);

  int ret = 0;
  bool pause = false;
  bool quit = false;
  while (! quit) {
    if (pause) {
      SDL_Delay(1);
    } else {
      ret = info.function(info.args);
    }

    info.mutex.lock();
    if (pause == false) {
      ++info.times;
    }

    pause = info.pause;
    quit = info.quit;
    if (info.times == info.next_times) {
      // !!! pause 変数の使い方を見直すべき
      info.pause = true;
      pause = true;
    }
    info.mutex.unlock();
  }
  return ret;
}


Thread::Thread(int (*fn)(void *), void* args)
  : pimpl(new pImpl(fn, args))
{
}


Thread::~Thread(void)
{
}


void Thread::run(int times)
{
  if (times != Infinity) {
    pimpl->thread_info_.mutex.lock();
    size_t now_times = pimpl->thread_info_.times;
    pimpl->thread_info_.next_times = now_times + times;
    pimpl->thread_info_.mutex.unlock();
  }
  pimpl->run();
}


void Thread::stop(void)
{
  // スレッドの停止
  pimpl->thread_info_.mutex.lock();
  pimpl->thread_info_.pause = true;
  pimpl->thread_info_.mutex.unlock();
}


int Thread::wait(void)
{
  pimpl->thread_info_.mutex.lock();
  pimpl->thread_info_.quit = true;
  pimpl->thread_info_.pause = true;
  pimpl->thread_info_.mutex.unlock();

  int retval = 0;
  SDL_WaitThread(pimpl->thread_, &retval);
  pimpl->thread_ = NULL;

  return retval;
}


bool Thread::isRunning(void) const
{
  return ((pimpl->thread_ == NULL) ||
          pimpl->thread_info_.pause || pimpl->thread_info_.quit) ? false : true;
}
