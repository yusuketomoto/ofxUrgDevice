#ifndef QRK_M_URG_DEVICE_H
#define QRK_M_URG_DEVICE_H

/*!
  \file
  \brief モニタ対応の URG 制御

  \author Satofumi KAMIMURA

  $Id: mUrgDevice.h 1841 2010-06-06 12:01:11Z satofumi $
*/

#include "UrgDevice.h"


namespace qrk
{
  /*!
    \brief モニタ対応の URG 制御
  */
  class mUrgDevice : public RangeSensor
  {
    mUrgDevice(void);
    mUrgDevice(const mUrgDevice& rhs);
    mUrgDevice& operator = (const mUrgDevice& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;

  public:
    /*!
      \brief オプション用パラメータ
    */
    enum {
      DefaultBaudrate = 115200, //!< 接続時のデフォルトボーレート
      Infinity = 0,             //!< 無限回のデータ取得
    };


    /*!
      \brief コンストラクタ

      動作モードを指定する

      - Record モード (-r 指定)
        - 取得データをログファイルに書き出す

      - Play モード (-p 指定)
        - ログファイルのデータを取得データとして扱う

      使用例
      \code
// プログラム実行時の引数に -r, -p を指定することで動作が変わる。
//
// % ./a.exe -r    データを取得しつつ、記録する
// % ./a.exe -p    記録済みのデータを再生する
//
// ただし、記録時と再生時では同じプログラムを使うこと。

#include <mUrgDevice.h>

using namespace qrk;
using namespace std;


int main(int argc, char *argv[])
{
  mUrgDevice urg(argc, argv);

  if (! urg.connect("COM4")) {
    ...
  }

  long timestamp = 0;
  vector<long> data;
  urg.capture(data, &timestamp);

  ...
} \endcode

      \see UrgDevice
    */
    explicit mUrgDevice(int argc, char *argv[]);
    ~mUrgDevice(void);

    const char* what(void) const;

    bool connect(const char* device, long baudrate = DefaultBaudrate);
    void setConnection(Connection* con);
    Connection* connection(void);
    void disconnect(void);
    bool isConnected(void) const;

    long minDistance(void) const;
    long maxDistance(void) const;
    int maxScanLines(void) const;

    void setRetryTimes(size_t times);

    int scanMsec(void) const;

    void setCaptureMode(RangeCaptureMode mode);
    RangeCaptureMode captureMode(void);

    void setCaptureRange(int begin_index, int end_index);
    void setCaptureFrameInterval(size_t interval);
    void setCaptureTimes(size_t times);
    size_t remainCaptureTimes(void);
    void setCaptureSkipLines(size_t skip_lines);

    int capture(std::vector<long>& data, long* timestamp = NULL);
    int captureWithIntensity(std::vector<long>& data,
                             std::vector<long>& intensity_data,
                             long* timestamp = NULL);
    void stop(void);

    bool setTimestamp(int ticks = 0, int* response_msec = NULL,
                      int* force_delay_msec = NULL);

    bool setLaserOutput(bool on);

    double index2rad(const int index) const;
    int rad2index(const double radian) const;

    void setParameter(const RangeSensorParameter& parameter);
    RangeSensorParameter parameter(void) const;
    bool loadParameter(void);

    bool versionLines(std::vector<std::string>& lines);

    void reboot(void);
  };
}

#endif /* !QRK_M_URG_DEVICE_H */
