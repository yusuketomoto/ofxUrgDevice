/*!
  \file
  \brief URG センサ制御

  \author Satofumi KAMIMURA

  $Id: UrgDevice.cpp 1992 2012-09-11 05:57:06Z satofumi $

  \todo remain_times の 100 をマクロで置き換える
*/

#include "UrgDevice.h"
#include "SerialDevice.h"
#include "ScipHandler.h"
#include "RangeSensorParameter.h"
#include "ticks.h"
#include "Thread.h"
#include "LockGuard.h"
#include "Lock.h"
#include <deque>
#include <limits>
#include <cmath>
#include <cstring>
#include <cstdio>

#ifdef MSC
#define snprintf _snprintf
#endif

using namespace qrk;
using namespace std;


namespace
{
  enum {
    MdScansMax = 100,           // [times]
  };
}


struct UrgDevice::pImpl
{
  struct ScanData
  {
    vector<long> length_data;
    long timestamp;
    CaptureSettings settings;

    ScanData(void) : timestamp(-1)
    {
    }
  };


  class Capture
  {
  public:
    virtual ~Capture(void)
    {
    }

    virtual string createCaptureCommand(void) = 0;
    virtual int capture(vector<long>& data, long* timestamp) = 0;
    virtual void setCapturesSize(size_t size) = 0;
    virtual size_t capturesSize(void) = 0;
    virtual size_t remainCaptureTimes(void) = 0;
  };


  class RawManualCapture : public Capture
  {
    pImpl* pimpl_;

  public:
    RawManualCapture(pImpl* pimpl) : pimpl_(pimpl)
    {
    }


    ~RawManualCapture(void)
    {
    }


    string createCaptureCommand(void)
    {
      // !!! parameter 受信直後に、送受信パラメータへの代入を行う
      // !!! ここでの送受信には、送受信パラメータの内容を用いる

      char buffer[] = "GDbbbbeeeegg\n";
      snprintf(buffer, strlen(buffer) + 1, "GD%04d%04d%02u\n",
               pimpl_->capture_begin_, pimpl_->capture_end_,
               pimpl_->capture_skip_lines_);

      return buffer;
    }


    int capture(vector<long>& data, long* timestamp)
    {
      pimpl_->retry_times_ = 0;

      // レーザを点灯させておく
      pimpl_->scip_.setLaserOutput(ScipHandler::On);

      // データ取得コマンドの送信
      string command = createCaptureCommand();
      int n = pimpl_->scip_.send(command.c_str(),
                                 static_cast<int>(command.size()));
      if (n != static_cast<int>(command.size())) {
        pimpl_->error_message_ = "Send command:" + command + " fail.";
        return -1;
      }

      CaptureSettings settings;
      pimpl_->scip_.receiveCaptureData(data, settings, timestamp, NULL);
      return static_cast<int>(data.size());
    }


    void setCapturesSize(size_t size)
    {
      static_cast<void>(size);

      // 何もしない
    }


    size_t capturesSize(void)
    {
      return 1;
    }


    size_t remainCaptureTimes(void)
    {
      return 0;
    }
  };


  class RawAutoCapture : public Capture
  {
    pImpl* pimpl_;
    size_t captures_size_;


  public:
    RawAutoCapture(pImpl* pimpl) : pimpl_(pimpl), captures_size_(1)
    {
    }


    ~RawAutoCapture(void)
    {
    }


    string createCaptureCommand(void)
    {
      char buffer[] = "MDbbbbeeeeggstt\n";
      snprintf(buffer, strlen(buffer) + 1, "MD%04d%04d%02u%01u%02u\n",
               pimpl_->capture_begin_, pimpl_->capture_end_,
               pimpl_->capture_skip_lines_,
               pimpl_->capture_frame_interval_,
               (pimpl_->capture_times_ > 99) ? 0 : pimpl_->capture_times_);

      pimpl_->remain_times_ = pimpl_->capture_times_;

      return buffer;
    }


    int capture(vector<long>& data, long* timestamp)
    {
      if (pimpl_->retry_times_ > pimpl_->max_retry_times_) {
        return -1;
      }

      // スレッドを起動
      LockGuard guard(pimpl_->mutex_);
      if ((! pimpl_->thread_.isRunning()) && pimpl_->data_buffer_.empty()) {
        pimpl_->thread_.run(1);
      }

      // 取得済みデータがなければ、戻る
      if (pimpl_->data_buffer_.empty()) {
        if (pimpl_->invalid_packet_) {
          return -1;
        }
        return 0;
      }

      swap(data, pimpl_->data_buffer_.front().length_data);
      if (timestamp) {
        *timestamp = pimpl_->data_buffer_.front().timestamp;
        //fprintf(stderr, "MD: %ld, %ld\n", ticks(), *timestamp);
      }
      pimpl_->data_buffer_.pop_front();

      return static_cast<int>(data.size());
    }


    void setCapturesSize(size_t size)
    {
      captures_size_ = size;
    }


    size_t capturesSize(void)
    {
      return captures_size_;
    }


    size_t remainCaptureTimes(void)
    {
      if (pimpl_->capture_times_ == 0) {
        return numeric_limits<size_t>::max();
      }

      LockGuard guard(pimpl_->mutex_);
      return pimpl_->remain_times_;
    }
  };


  class RawIntensityCapture : public Capture
  {
    pImpl* pimpl_;
    size_t captures_size_;


  public:
    RawIntensityCapture(pImpl* pimpl) : pimpl_(pimpl), captures_size_(1)
    {
    }


    ~RawIntensityCapture(void)
    {
    }


    string createCaptureCommand(void)
    {
      char buffer[] = "MEbbbbeeeeggstt\n";
      if ((! pimpl_->urg_type_.compare("URG-04LX")) ||
          (! pimpl_->urg_type_.compare("URG-04LX-UG01")) ||
          (! pimpl_->urg_type_.compare("UBG-04LX-F01"))) {
          // URG-04LX ならば、FF 形式でデータ送信を行う
          snprintf(buffer, strlen(buffer) + 1, "MD%04d%04dFF%01u%02u\n",
                   pimpl_->capture_begin_, pimpl_->capture_end_,
                   pimpl_->capture_frame_interval_,
                   (pimpl_->capture_times_ > 99) ? 0 : pimpl_->capture_times_);

      } else {
          // UTM-30LX ならば、ME コマンドを用いる
          snprintf(buffer, strlen(buffer) + 1, "ME%04d%04d%02u%01u%02u\n",
                   pimpl_->capture_begin_, pimpl_->capture_end_,
                   pimpl_->capture_skip_lines_,
                   pimpl_->capture_frame_interval_,
                   (pimpl_->capture_times_ > 99) ? 0 : pimpl_->capture_times_);
      }
      pimpl_->remain_times_ = pimpl_->capture_times_;

      return buffer;
    }


    int capture(vector<long>& data, long* timestamp)
    {
      if (pimpl_->retry_times_ > pimpl_->max_retry_times_) {
        return -1;
      }

      LockGuard guard(pimpl_->mutex_);
      // 取得済みデータがなければ、スレッドを起動
      if ((! pimpl_->thread_.isRunning()) && pimpl_->data_buffer_.empty()) {
        pimpl_->thread_.run(1);
      }

      // 取得済みデータがなければ、戻る
      if (pimpl_->data_buffer_.empty()) {
        if (pimpl_->invalid_packet_) {
          return -1;
        }
        return 0;
      }

      swap(data, pimpl_->data_buffer_.front().length_data);
      if (timestamp) {
        *timestamp = pimpl_->data_buffer_.front().timestamp;
      }
      CaptureSettings settings = pimpl_->data_buffer_.front().settings;
      pimpl_->data_buffer_.pop_front();
      pimpl_->intensity_data_.timestamp = *timestamp;
      pimpl_->intensity_data_.length_data.clear();

      if (pimpl_->urg_type_.compare("URG-04LX") &&
          pimpl_->urg_type_.compare("URG-04LX-UG01") &&
          pimpl_->urg_type_.compare("UBG-04LX-F01")) {
          // UTM-30LX のとき
        int skip_lines = settings.skip_lines;
        int min_length = pimpl_->parameters_.distance_min;
        int n = data.size() / skip_lines / 2;
        pimpl_->intensity_data_.length_data.reserve(n * skip_lines);

        int filled = 0;
        for (int i = 0; i < n; ++i) {
          size_t index = 2 * skip_lines * i;
          long length = data[index];
          long intensity_value =
            (length < min_length) ? 0 : data[index + skip_lines];
          for (int j = 0; j < skip_lines; ++j) {
            pimpl_->intensity_data_.length_data.push_back(intensity_value);
            data[filled++] = length;
          }
        }
        vector<long>::iterator first_it = data.begin() + (data.size() / 2);
        data.erase(first_it, data.end());

      } else if ((! pimpl_->urg_type_.compare("URG-04LX")) ||
                 (! pimpl_->urg_type_.compare("URG-04LX-UG01")) ||
                 (! pimpl_->urg_type_.compare("UBG-04LX-F01"))) {
        // URG-04LX のとき
        size_t n = data.size();
        int min_length = pimpl_->parameters_.distance_min;
        pimpl_->intensity_data_.length_data.reserve(data.size());
        size_t offset = pimpl_->capture_begin_ & 0x1;
        for (size_t i = 0; i < n; i += 2) {
          size_t index = i + offset;
          long length = data[index];
          if ((index + 1) < n) {
            long intensity_value = (length < min_length) ? 0 : data[index + 1];
            pimpl_->intensity_data_.length_data.push_back(intensity_value);
            pimpl_->intensity_data_.length_data.push_back(intensity_value);
            data[index + 1] = length;
          }
        }
      } else {
        // !!! エラー処理
      }

      return static_cast<int>(data.size());
    }


    void setCapturesSize(size_t size)
    {
      captures_size_ = size;
    }


    size_t capturesSize(void)
    {
      return captures_size_;
    }

    size_t remainCaptureTimes(void)
    {
      if (pimpl_->capture_times_ == 0) {
        return numeric_limits<size_t>::max();
      }

      LockGuard guard(pimpl_->mutex_);
      return pimpl_->remain_times_;
    }
  };


  string error_message_;
  UrgDevice* parent_;
  Connection* con_;
  SerialDevice* serial_; //!< 有効な接続オブジェクトがないときに用いる
  ScipHandler scip_;
  RangeSensorParameter parameters_;
  string urg_type_;
  long recent_timestamp_;
  int timestamp_offset_;

  RangeCaptureMode capture_mode_;
  RawManualCapture manual_capture_;
  RawAutoCapture auto_capture_;
  RawIntensityCapture intensity_capture_;
  Capture* capture_;
  Thread thread_;
  Lock mutex_;

  deque<ScanData> data_buffer_;
  ScanData intensity_data_;

  int capture_begin_;
  int capture_end_;
  size_t capture_skip_lines_;
  int capture_skip_frames_;

  size_t capture_frame_interval_;
  size_t capture_times_;
  size_t remain_times_;
  bool invalid_packet_;
  size_t max_retry_times_;
  size_t retry_times_;

  long base_timestamp_;
  long pre_timestamp_;


  pImpl(UrgDevice* parent)
    : error_message_("no error."), parent_(parent),
      con_(NULL), serial_(NULL), urg_type_(""),
      recent_timestamp_(0), timestamp_offset_(0),
      capture_mode_(ManualCapture),
      manual_capture_(this), auto_capture_(this),
      intensity_capture_(this), capture_(&manual_capture_),
      thread_(&capture_thread, this),
      capture_begin_(0), capture_end_(0),
      capture_skip_lines_(1), capture_skip_frames_(0),
      capture_frame_interval_(0), capture_times_(0),
      remain_times_(0), invalid_packet_(false),
      max_retry_times_(DefaultRetryTimes), retry_times_(0),
      base_timestamp_(0), pre_timestamp_(0)
  {
  }


  ~pImpl(void)
  {
    disconnect();

    // con_ は外部からセットされる可能性があるため、このクラスでは解放しない
    // serial_ のみ解放する
    delete serial_;
  }


  void initializeSerial(void)
  {
    if (! serial_) {
      serial_ = new SerialDevice;
    }
    con_ = serial_;
  }


  bool connect(const char* device, long baudrate)
  {
    disconnect();
    if (! con_) {
      initializeSerial();
    }
    scip_.setConnection(con_);

    // ボーレートを検出した上でのデバイスとの接続
    if (! scip_.connect(device, baudrate)) {
      error_message_ = scip_.what();
      return false;
    }

    if (! loadParameter()) {
      return false;
    }
    updateCaptureParameters();

    return true;
  }


  bool loadParameter(void)
  {
    // URG パラメータの取得
    RangeSensorParameter parameters;
    if (! scip_.loadParameter(parameters)) {
      error_message_ = scip_.what();
      return false;
    }
    swap(parameters_, parameters);

    size_t type_length = min(parameters_.model.find('('),
                             parameters_.model.find('['));
    urg_type_ = parameters_.model.substr(0, type_length);

    // !!! capture_begin_, capture_end_ との調整をすべき
    // !!! min(), max() を使うこと

    return true;
  }


  void disconnect(void)
  {
    stop();
    if (con_) {
      con_->disconnect();
    }
  }


  void updateCaptureParameters(void)
  {
    capture_begin_ = parameters_.area_min;
    capture_end_ = parameters_.area_max;
  }


  // AutoCapture, IntensityCapture のデータ取得を行う
  static int capture_thread(void* args)
  {
    pImpl* obj = static_cast<pImpl*>(args);

    // 設定に基づいて、データ受信コマンドを作成して発行
    string capture_command = obj->capture_->createCaptureCommand();
    if (capture_command.empty()) {
      // ここのエラーが IntensityCapture のときのみ発生するのに依存した実装
      obj->error_message_ = "This urg is not support intensity capture.";
      return -1;
    }
    int n = obj->scip_.send(capture_command.c_str(),
                            static_cast<int>(capture_command.size()));
    if (n != static_cast<int>(capture_command.size())) {
      obj->error_message_ = capture_command + " fail.";
      ++obj->retry_times_;
      return -1;
    }

    // 受信待ち
    ScanData data;
    int remain_times = MdScansMax;
    int total_times = 0;
    while (1) {
      // 受信完了、およびエラーで抜ける
      obj->invalid_packet_ = false;
      CaptureType type =
        obj->scip_.receiveCaptureData(data.length_data, data.settings,
                                      &data.timestamp,
                                      &remain_times, &total_times);
      if (type == Mx_Reply) {
        // MS/MD の応答パケットの場合、次のデータを待つ
        continue;
      }

      if (! ((type == MD) || (type == MS) || (type == ME))) {
        // 受信データでなければ、スレッド処理を中断する
        obj->invalid_packet_ = true;
        ++obj->retry_times_;
        break;
      }
      obj->retry_times_ = 0;

      // タイムスタンプが 24 bit しかないため、１巡することへの対処
      if ((data.timestamp >= 0) && (data.timestamp < obj->pre_timestamp_)) {
        obj->base_timestamp_ += 1 << 24;
      }
      obj->pre_timestamp_ = data.timestamp;
      data.timestamp += obj->base_timestamp_;

      LockGuard guard(obj->mutex_);
      deque<ScanData>& data_buffer = obj->data_buffer_;

      // 古くなったデータを取り除く
      int current_size = static_cast<int>(data_buffer.size());
      int erase_size =
        current_size - static_cast<int>(obj->capture_->capturesSize());
      if (erase_size > 0) {
        data_buffer.erase(data_buffer.begin(),
                          data_buffer.begin() + erase_size);
      }

      // 今回のデータを追加
      ScanData dummy_data;
      data_buffer.push_back(dummy_data);
      swap(data_buffer.back(), data);

      if ((obj->capture_times_ > 0) && (remain_times < MdScansMax)) {
        obj->remain_times_ = remain_times;
      } else {
        --obj->remain_times_;
        if (obj->remain_times_ == 0) {
          obj->scip_.setLaserOutput(Off);
        }
      }
      obj->parent_->captureReceived();

      if ((total_times > 0) && (remain_times == 0)) {
        // 取得が終了したら、抜ける
        break;
      }
    }

    return 0;
  }


  int capture(vector<long>& data, long* timestamp)
  {
    long raw_timestamp = 0;
    int n = capture_->capture(data, &raw_timestamp);
    if (n < 0) {
      error_message_ = scip_.what();
      return n;
    }

    recent_timestamp_ = raw_timestamp - timestamp_offset_;
    if (timestamp) {
      *timestamp = recent_timestamp_;
    }
    return n;
  }


  int captureWithIntensity(vector<long>& data,
                           vector<long>& intensity_data,
                           long* timestamp)
  {
    int n = capture(data, timestamp);
    if (n < 0) {
      return n;
    }
    return captureIntensity(intensity_data, NULL);
  }


  int captureIntensity(vector<long>& intensity_data, int* timestamp)
  {
    if (capture_mode_ != IntensityCapture) {
      error_message_ = "please call setCaptureMode(IntensityCapture).";
      return -1;
    }

    if (intensity_data_.length_data.empty()) {
      return 0;
    }

    if (timestamp) {
      *timestamp = intensity_data_.timestamp;
      recent_timestamp_ = *timestamp;
    }
    swap(intensity_data, intensity_data_.length_data);
    intensity_data_.length_data.clear();

    return static_cast<int>(intensity_data.size());
  }


  bool setTimestamp(int timestamp, int* response_msec, int* force_delay_msec)
  {
    long prepare_ticks = ticks();

    // TM0 モードに遷移
    // !!! true, false をキーワードで置換すること
    if (! scip_.setRawTimestampMode(true)) {
      return false;
    }

    // TM1 のタイムスタンプを取得し、通信遅延と URG のタイムスタンプを取得する
    int urg_timestamp = 0;
    int first_ticks = ticks();
    if (scip_.rawTimestamp(&urg_timestamp)) {

      //fprintf(stderr, "raw: %d, %d\n", timestamp, urg_timestamp);

      int delay = ticks() - first_ticks;
      if (force_delay_msec) {
        delay = *force_delay_msec;
      }
      if (response_msec) {
        *response_msec = delay;
      }
      timestamp_offset_ =
          (urg_timestamp + (delay / 2))
          - (timestamp + (first_ticks - prepare_ticks));
    }

    // URG タイムスタンプ用のオフセット時間を計算し、TM2 で抜ける
    if (! scip_.setRawTimestampMode(false)) {
      return false;
    }

    return true;
  }


  int rad2index(const double radian) const
  {
    int area_total = parameters_.area_total;
    int index =
        static_cast<int>(floor(((radian * area_total) / (2.0 * M_PI)) + 0.5)
                         + parameters_.area_front);

    if (index < 0) {
      index = 0;
    } else if (index > parameters_.area_max) {
      index = parameters_.area_max;
    }
    return index;
  }


  bool isConnected(void) const
  {
    return (con_ == NULL) ? false : con_->isConnected();
  }


  void stop(void)
  {
    if (! isConnected()) {
      return;
    }

    if (capture_mode_ == ManualCapture) {
      return;
    }

    // QT コマンドの発行
    scip_.setLaserOutput(ScipHandler::Off);

    // 応答を待つ
    if (thread_.isRunning()) {
      thread_.wait();
    }
  }


  void clear(void)
  {
    LockGuard guard(mutex_);
    data_buffer_.clear();
    intensity_data_.length_data.clear();
  }
};


UrgDevice::UrgDevice(void) : pimpl(new pImpl(this))
{
}


UrgDevice::~UrgDevice(void)
{
}


void UrgDevice::captureReceived(void)
{
  // 受信完了を継承先のクラスが利用するためのメソッド
}


const char* UrgDevice::what(void) const
{
  return pimpl->error_message_.c_str();
}


bool UrgDevice::connect(const char* device, long baudrate)
{
  return pimpl->connect(device, baudrate);
}


void UrgDevice::setConnection(Connection* con)
{
  pimpl->serial_ = NULL;
  pimpl->con_ = con;
  pimpl->scip_.setConnection(con);
}


Connection* UrgDevice::connection(void)
{
  return pimpl->con_;
}


void UrgDevice::disconnect(void)
{
  pimpl->disconnect();
}


bool UrgDevice::isConnected(void) const
{
  return pimpl->isConnected();
}


long UrgDevice::minDistance(void) const
{
  return pimpl->parameters_.distance_min;
}


long UrgDevice::maxDistance(void) const
{
  return pimpl->parameters_.distance_max;
}


int UrgDevice::maxScanLines(void) const
{
  // +1 は、parameters_ が未初期化のときに new long [0] しないための処置
  return pimpl->parameters_.area_max + 1;
}


void UrgDevice::setRetryTimes(size_t times)
{
  pimpl->max_retry_times_ = times;
}


void UrgDevice::setCapturesSize(size_t size)
{
  pimpl->capture_->setCapturesSize(size);
}


size_t UrgDevice::remainCaptureTimes(void)
{
  return pimpl->capture_->remainCaptureTimes();
}


int UrgDevice::scanMsec(void) const
{
  int scan_rpm = pimpl->parameters_.scan_rpm;
  return (scan_rpm <= 0) ? 1 : (1000 * 60 / scan_rpm);
}


void UrgDevice::setCaptureMode(RangeCaptureMode mode)
{
  // capture を停止する。capture の再開は行わない
  stop();
  pimpl->clear();

  if (mode == ManualCapture) {
    pimpl->capture_ = &pimpl->manual_capture_;

  } else if (mode == AutoCapture) {
    pimpl->capture_ = &pimpl->auto_capture_;

  } else if (mode == IntensityCapture) {
    pimpl->capture_ = &pimpl->intensity_capture_;
  }

  pimpl->capture_mode_ = mode;
}


RangeCaptureMode UrgDevice::captureMode(void)
{
  return pimpl->capture_mode_;
}


void UrgDevice::setCaptureRange(int begin_index, int end_index)
{
  // !!! 排他制御

  // capture を停止する。capture の再開は行わない
  stop();
  pimpl->clear();

  pimpl->capture_begin_ = begin_index;
  pimpl->capture_end_ = end_index;
}


void UrgDevice::setCaptureFrameInterval(size_t interval)
{
  // !!! 排他制御

  // capture を停止する。capture の再開は行わない
  stop();
  pimpl->clear();

  pimpl->capture_frame_interval_ = interval;
}


void UrgDevice::setCaptureTimes(size_t times)
{
  // !!! 排他制御

  // capture を停止する。capture の再開は行わない
  stop();
  pimpl->clear();

  // !!! 範囲制限の判定処理
  pimpl->capture_times_ = times;
}



void UrgDevice::setCaptureSkipLines(size_t skip_lines)
{
  // capture を停止する。capture の再開は行わない
  stop();
  pimpl->clear();

  pimpl->capture_skip_lines_ = skip_lines;
}


int UrgDevice::capture(vector<long>& data, long* timestamp)
{
  // !!! 未接続ならば、戻す

  return pimpl->capture(data, timestamp);
}


int UrgDevice::captureWithIntensity(vector<long>& data,
                                  vector<long>& intensity_data,
                                  long* timestamp)
{
  // !!! 未接続ならば、戻す
  return pimpl->captureWithIntensity(data, intensity_data, timestamp);
}


void UrgDevice::stop(void)
{
  pimpl->stop();
}


bool UrgDevice::setTimestamp(int ticks, int* response_msec,
                             int* force_delay_msec)
{
  // !!! 未接続ならば、戻す

  return pimpl->setTimestamp(ticks, response_msec, force_delay_msec);
}


long UrgDevice::recentTimestamp(void) const
{
  return pimpl->recent_timestamp_;
}


bool UrgDevice::setLaserOutput(bool on)
{
  // !!! 未接続ならば、戻す

  return pimpl->scip_.setLaserOutput(on, ScipHandler::Force);
}


double UrgDevice::index2rad(const int index) const
{
  int index_from_front = index - pimpl->parameters_.area_front;
  return index_from_front * (2.0 * M_PI) / pimpl->parameters_.area_total;
}


int UrgDevice::rad2index(const double radian) const
{
  return pimpl->rad2index(radian);
}


void UrgDevice::setParameter(const RangeSensorParameter& parameter)
{
  pimpl->parameters_ = parameter;
  pimpl->updateCaptureParameters();
}


RangeSensorParameter UrgDevice::parameter(void) const
{
  return pimpl->parameters_;
}


bool UrgDevice::loadParameter(void)
{
  return pimpl->loadParameter();
}


bool UrgDevice::versionLines(vector<string>& lines)
{
  if (! isConnected()) {
    return false;
  }

  return pimpl->scip_.versionLines(lines);
}


bool UrgDevice::reboot(void)
{
  UrgDevice::setLaserOutput(Off);

  // send "RB" twice.
  for (int i = 0; i < 2; ++i) {
    pimpl->scip_.send("RB\n", 3);

    enum {
      RB_Timeout = 1000,        // [msec]
    };
    char recv_buffer[3];
    int send_n = pimpl->scip_.recv(recv_buffer, 3, RB_Timeout);
    if (send_n != 3) {
      return false;
    }
    if (strncmp(recv_buffer, "RB\n", 3)) {
      return false;
    }
  }

  UrgDevice::disconnect();

  return true;
}
