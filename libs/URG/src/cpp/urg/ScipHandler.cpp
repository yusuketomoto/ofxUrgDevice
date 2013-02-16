/*!
  \file
  \brief SCIP 通信処理

  \author Satofumi KAMIMURA

  $Id: ScipHandler.cpp 1977 2012-04-05 08:04:51Z satofumi $
*/

#include "ScipHandler.h"
#include "RangeSensorParameter.h"
#include "Connection.h"
#include "ConnectionUtils.h"
#include "ticks.h"
#include "delay.h"
#include "DetectOS.h"
#include "log_printf.h"
#include <cstring>
#include <cstdio>

#ifdef MSC
#define snprintf _snprintf
#endif

using namespace qrk;
using namespace std;


namespace
{
  typedef enum {
    ProcessContinue,
    ProcessBreak,
    ProcessNormal,
  } LoopProcess;

  void clearReceived(vector<long>& data, CaptureType& type,
                     int& line_count, int& timeout,
                     string& remain_string,
                     string& left_packet_data)
  {
    data.clear();
    type = TypeUnknown;
    line_count = 0;
    timeout = 0;
    remain_string.clear();
    left_packet_data.clear();
  }
}


struct ScipHandler::pImpl
{
  enum {
    TotalTimeout = 1000,        // [msec]
    ContinuousTimeout = 100,    // [msec]
    FirstTimeout = 1000,        // [msec]

    BufferSize = 64 + 1 + 1,    // データ長 + チェックサム + 改行

    ResponseTimeout = -1,
    MismatchResponse = -2,
    SendFail = -3,
    ChecksumFail = -4,
    Scip11Response = -14,

    InvalidRange = -1,
  };

  typedef enum {
    LaserUnknown = 0,
    LaserOn,
    LaserOff,
  } LaserState;

  string error_message_;
  Connection* con_;
  LaserState laser_state_;
  bool mx_capturing_;

  bool isPreCommand_QT_;


  pImpl(void)
    : error_message_("no error."), con_(NULL), laser_state_(LaserUnknown),
      mx_capturing_(false), isPreCommand_QT_(false)
  {
  }


  bool connect(const char* device, long baudrate)
  {
    if (! con_->connect(device, baudrate)) {
      error_message_ = con_->what();
      return false;
    }

    long try_baudrates[] = { 115200, 19200, 38400, };
    size_t try_size = sizeof(try_baudrates) / sizeof(try_baudrates[0]);

    // 接続したいボーレートを配列の先頭と入れ換える
    for (size_t i = 1; i < try_size; ++i) {
      if (baudrate == try_baudrates[i]) {
        swap(try_baudrates[0], try_baudrates[i]);
        break;
      }
    }

    // 指定のボーレートで接続し、応答が返されるかどうか試す
    for (size_t i = 0; i < try_size; ++i) {

      // ホスト側のボーレートを変更
      if (! con_->setBaudrate(try_baudrates[i])) {
        error_message_ = con_->what();
        return false;
      }

      // 前回分の受信パケットを読み捨て
      con_->clear();

      // QT の発行
      int return_code = -1;
      char qt_expected_response[] = { 0, -1 };
      // return_code を使いたいため、setLaserOutput() を用いずに QT を送信する
      if (response(return_code, "QT\n", qt_expected_response)) {
        laser_state_ = LaserOff;
        return changeBothBaudrate(baudrate);

      } else if (return_code == ResponseTimeout) {
        // ボーレートが違っていて、通信できなかったとみなす
        error_message_ = "baudrate is not detected.";
        continue;

      } else if (return_code == MismatchResponse) {
        // MD/MS の応答とみなし、受信データを読み飛ばす
	con_->clear();
        skip(con_, ContinuousTimeout);
        return changeBothBaudrate(baudrate);

      } else if (return_code == Scip11Response) {
        // SCIP1.1 プロトコルの場合のみ、SCIP2.0 を送信する
        char scip20_expected_response[] = { 0, -1 };
        if (! response(return_code, "SCIP2.0\n", scip20_expected_response)) {
          error_message_ =
            "SCIP1.1 protocol is not supported. Please update URG firmware, or reconnect after a few seconds because sensor is booting.";
          return false;
        }
        laser_state_ = LaserOff;
        return changeBothBaudrate(baudrate);

      } else if (return_code == 0xE) {
        // TM モードとみなし、TM2 を発行する
        char tm2_expected_response[] = { 0, -1 };
        if (response(return_code, "TM2\n", tm2_expected_response)) {
          laser_state_ = LaserOff;
          return changeBothBaudrate(baudrate);
        }
      }
    }

    con_->disconnect();
    return false;
  }


  bool changeBothBaudrate(long baudrate)
  {
    // 既に目標対象のボーレート値ならば、成功とみなす
    // この関数は、ScipHandler::connect() か、それ以後でないと呼ばれないため
    if (con_->baudrate() == baudrate) {
      return true;
    }

    // URG 側のボーレートを変更
    int pre_ticks = ticks();
    if (! changeBaudrate(baudrate)) {
      return false;
    }

    // シリアル通信の場合、ボーレート変更後、１周分だけ待つ必要がある
    int reply_msec = ticks() - pre_ticks;
    delay((reply_msec * 4 / 3) + 10);

    // ホスト側のボーレートを変更
    return con_->setBaudrate(baudrate);
  }


  bool changeBaudrate(long baudrate)
  {
#if 0
    // Tcpip 接続に対応するため、コメントアウト
    if (! ((baudrate == 19200) || (baudrate == 38400) ||
           (baudrate == 57600) || (baudrate == 115200))) {
      error_message_ = "Invalid baudrate value.";
      return false;
    }
#endif

    // SS を送信し、URG 側のボーレートを変更する
    char send_buffer[] = "SSxxxxxx\n";
    snprintf(send_buffer, 10, "SS%06ld\n", baudrate);
    int return_code = -1;
    // !!! 既に設定対象のボーレート、の場合の戻り値を ss_expected... に追加する
    char ss_expected_response[] = { 0, 0x3, 0x4, 0xf, -1 };
    if (! response(return_code, send_buffer, ss_expected_response)) {
      error_message_ = "Baudrate change fail.";
      return false;
    }

    return true;
  }


  bool loadParameter(RangeSensorParameter& parameters)
  {
    // PP の送信とデータの受信
    int return_code = -1;
    char pp_expected_response[] = { 0, -1 };
    vector<string> lines;
    if (! response(return_code, "PP\n", pp_expected_response, &lines)) {
      error_message_ = "PP fail.";
      return false;
    }

    // PP 応答内容の格納
    if (lines.size() != 8) {
      error_message_ = "Invalid PP response.";
      return false;
    }

    // !!! チェックサムの評価を行うべき

    int modl_length =
      static_cast<int>(lines[RangeSensorParameter::MODL].size());
    // 最初のタグと、チェックサムを除いた文字列を返す
    if (modl_length > (5 + 2)) {
      modl_length -= (5 + 2);
    }
    parameters.model = lines[RangeSensorParameter::MODL].substr(5, modl_length);

    parameters.distance_min = substr2int(lines[RangeSensorParameter::DMIN], 5);
    parameters.distance_max = substr2int(lines[RangeSensorParameter::DMAX], 5);
    parameters.area_total = substr2int(lines[RangeSensorParameter::ARES], 5);
    parameters.area_min = substr2int(lines[RangeSensorParameter::AMIN], 5);
    parameters.area_max = substr2int(lines[RangeSensorParameter::AMAX], 5);
    parameters.area_front = substr2int(lines[RangeSensorParameter::AFRT], 5);
    parameters.scan_rpm = substr2int(lines[RangeSensorParameter::SCAN], 5);

    return true;
  }


  int substr2int(const string& line, int from_n, int length = string::npos)
  {
    return atoi(line.substr(from_n, length).c_str());
  }


  bool response(int& return_code, const char send_command[],
                char expected_response[],
                vector<string>* lines = NULL)
  {
    return_code = -1;
    if (! con_) {
      error_message_ = "no connection.";
      return false;
    }

    size_t send_size = strlen(send_command);
    int actual_send_size = con_->send(send_command, send_size);
    if (strncmp(send_command, "QT\n", send_size)) {
      isPreCommand_QT_ = false;
    }
    if (actual_send_size != static_cast<int>(send_size)) {
      return_code = SendFail;
      return false;
    }

    // エコーバックの受信
    char buffer[BufferSize];
    int recv_size = readline(con_, buffer, BufferSize, FirstTimeout);
    if (recv_size < 0) {
      error_message_ = "response timeout.";
      return_code = ResponseTimeout;
      return false;
    }

    // シリアル接続でボーレート変更直後の 0x00 は、判定外とする
    if (! ((recv_size == 1) && (buffer[0] == 0x00))) {
      if ((recv_size != static_cast<int>(send_size - 1)) ||
          (strncmp(buffer, send_command, recv_size))) {
        error_message_ = "mismatch response: " + string(buffer);
        return_code = MismatchResponse;
        return false;
      }
    }

    // 応答の受信
    // !!! 上記の処理となるべく共通にする
    // !!! SCIP1.1 プロトコルの応答は、負号を付加した上で return_code に格納する
    recv_size = readline(con_, buffer, BufferSize, ContinuousTimeout);
    if (recv_size < 0) {
      // !!! この処理をくくる
      error_message_ = "response timeout.";
      return_code = ResponseTimeout;
      return false;
    }
    if (recv_size == 3) {
      // ３文字ならば、SCIP2.0 とみなしてチェックサムを確認する
      // !!! チェックサムの確認
      if (! checkSum(buffer, recv_size - 1, buffer[recv_size - 1])) {
	return_code = ChecksumFail;
	return false;
      }
      buffer[2] = '\0';
      return_code = strtol(buffer, NULL, 16);

    } else if (recv_size == 1) {
      // １文字ならば、SCIP1.1 とみなして 16進変換した値に負号をつけて返す
      buffer[1] = '\0';
      return_code = -strtol(buffer, NULL, 16);
    }

    // データ領域の受信
    // １行読み出し、改行のみならば終了とみなす
    do {
      recv_size = readline(con_, buffer, BufferSize, ContinuousTimeout);
      if (lines && (recv_size > 0)) {
        lines->push_back(buffer);
      }
    } while (recv_size > 0);

    for (int i = 0; expected_response[i] != -1; ++i) {
      if (return_code == expected_response[i]) {
        return true;
      }
    }
    return false;
  }


  bool setLaserOutput(bool on, bool force)
  {
    if (((on == true) && (laser_state_ == LaserOn)) ||
        ((on == false) && (laser_state_ == LaserOff))) {
      if (! force) {
        // レーザ出力が現在の状態と同じならば戻る
        // 強制設定フラグが true のときは戻らずに設定を行う
        return true;
      }
    }
    if ((!on) && isPreCommand_QT_) {
      return false;
    }

    if (on) {
      int return_code = -1;
      char expected_response[] = { 0, -1 };
      if (! response(return_code, "BM\n", expected_response)) {
        error_message_ = "BM fail.";
        return false;
      }
      laser_state_ = LaserOn;
      return true;

    } else {
      // "QT"
      if (! mx_capturing_) {
        // 消灯するための QT では、応答を待つべき
        int return_code = -1;
        char qt_expected_response[] = { 0, -1 };
        if (! response(return_code, "QT\n", qt_expected_response)) {
          return false;
        }
        laser_state_ = LaserOff;
        isPreCommand_QT_ = true;
        return true;

      } else {
        // MD を中断するための QT では、応答を待ってはならない
        // 応答は、受信スレッド内で処理される
        con_->send("QT\n", 3);
        isPreCommand_QT_ = true;
      }

      return true;
    }
  }


  bool testChecksum(const char* buffer, int line_size,
                    vector<long>& data, CaptureType& type,
                    int line_count, int timeout,
                    string& remain_string, string& left_packet_data)
  {
    if (! checkSum(buffer, line_size - 1, buffer[line_size - 1])) {
      log_printf("checksum error: %s\n", buffer);
      // return InvalidData;
      // !!! URG のパケットエラーがなくなったら、この実装に戻す

      // !!! 存在するだけのパケットを読み飛ばす
      error_message_ = "invalid packet.";
      clearReceived(data, type, line_count, timeout,
                    remain_string, left_packet_data);
      return false;
    }

    return true;
  }


  LoopProcess handleEchoback(const char* buffer, CaptureSettings& settings,
                             CaptureType& type, vector<long>& data,
                             int line_count, int timeout,
                             string& remain_string,
                             string& left_packet_data)
  {
    string line = buffer;
    if ((! line.compare(0, 2, "GD")) || (! line.compare(0, 2, "GS"))) {
      if (! parseGdEchoback(settings, line)) {
        return ProcessBreak;
      }
      type = (line[1] = 'D') ? GD : GS;

    } else if ((! line.compare(0, 2, "MD")) ||
               (! line.compare(0, 2, "MS"))) {
      if (! parseMdEchoback(settings, line)) {
        return ProcessBreak;
      }
      type = (line[1] = 'D') ? MD : MS;
      laser_state_ = LaserOn;

    } else if (! line.compare(0, 2, "ME")) {
      if (! parseMeEchoback(settings, line)) {
        return ProcessBreak;
      }
      type = ME;
      laser_state_ = LaserOn;

    } else if (! line.compare(0, 2, "QT")) {
      settings.remain_times = 0;
      laser_state_ = LaserOff;
      mx_capturing_ = false;
      return ProcessNormal;

    } else {
      //return InvalidData;
      // !!! URG が正常なパケットを返すようになったら、この実装に戻す

      clearReceived(data, type, line_count, timeout,
                    remain_string, left_packet_data);
      //fprintf(stderr, "invalid data: %s\n", buffer);
      return ProcessContinue;
    }

    data.reserve(settings.capture_last + 1);
    return ProcessNormal;
  }


  void handleReturnCode(char* buffer, CaptureSettings& settings, int timeout,
                        CaptureType& type, int* total_times)
  {
    // !!! 長さが 2 + 1 かのチェックをすべき
    buffer[2] = '\0';
    settings.error_code = atoi(buffer);

    if (settings.error_code == 10) {
      // レーザ消灯を検出
      laser_state_ = pImpl::LaserOff;
    }

    // "0B" が返された場合、センサとホストの応答がずれている可能性があるので
    // 続く応答を読み捨てる
    if (! strncmp(buffer, "0B", 2)) {
      skip(con_, TotalTimeout, timeout);
    }

    // !!! "00P" との比較をすべき
    if ((settings.error_code == 0) &&
        ((type == MD) || (type == MS) || (type == ME))) {
      if (total_times) {
        *total_times = settings.remain_times;
      }
      type = Mx_Reply;
    }
  }


  CaptureType receiveCaptureData(vector<long>& data,
                                 CaptureSettings& settings, long* timestamp,
                                 int* remain_times, int* total_times)
  {
    int line_count = 0;
    data.clear();

    string remain_string;

    string left_packet_data;
    char buffer[BufferSize];

    error_message_ = "no response.";

    CaptureType type = TypeUnknown;
    int timeout = FirstTimeout;
    int line_size = 0;
    while ((line_size = readline(con_, buffer, BufferSize, timeout)) > 0) {
      //fprintf(stderr, "%d: % 3d: %s\n", ticks(), line_count, buffer);

      // チェックサムの確認
      if (line_count != 0) {
        // エコーバックにはチェックサム文字列がないので、無視
        if (! testChecksum(buffer, line_size, data, type, line_count, timeout,
                           remain_string, left_packet_data)) {
          continue;
        }
      }

      if (line_count == 0) {
        // エコーバック
        LoopProcess loop_process =
          handleEchoback(buffer, settings, type, data, line_count, timeout,
                         remain_string, left_packet_data);
        if (loop_process == ProcessContinue) {
          continue;

        } else if (loop_process == ProcessBreak) {
          break;
        }

      } else if (line_count == 1) {
        // 応答コード
        handleReturnCode(buffer, settings, timeout, type, total_times);

      } else if (line_count == 2) {
        // タイムスタンプ
        if (timestamp) {
          *timestamp = decode(buffer, 4);
        }
      } else {
        if (line_count == 3) {
          // 受信データがない先頭からの領域を、ダミーデータで埋める
          for (int i = 0; i < settings.capture_first; ++i) {
            data.push_back(InvalidRange);
            if (type == ME) {
              // ME 受信のときは、強度データ分も埋める
              data.push_back(InvalidRange);
            }
          }
        }
        // 距離データの格納
        left_packet_data =
          addLengthData(data, string(buffer), left_packet_data,
                        settings.data_byte, settings.skip_lines);
      }
      ++line_count;
      timeout = ContinuousTimeout;
    }

    // !!! type が距離データ取得のときは、正常に受信が完了したか、を確認すべき

    // ME で "まとめる数" 設定以上のデータが返されるバグに対処
    size_t expected_n = settings.capture_last * ((type == ME) ? 2 : 1);
    if (expected_n < data.size()) {
      data.erase(data.begin() + expected_n, data.end());
    }

    if (remain_times) {
      *remain_times = settings.remain_times;
    }
    return type;
  }


  bool parseGdEchoback(CaptureSettings& settings, const string& line)
  {
    if (line.size() != 12) {
      error_message_ = "Invalid Gx packet has arrived.";
      return false;
    }

    settings.capture_first = substr2int(line, 2, 4);
    settings.capture_last = substr2int(line, 6, 4) + 1;
    int skip_lines = substr2int(line, 10, 2);
    settings.skip_lines = (skip_lines == 0) ? 1 : skip_lines;
    settings.data_byte = (line[1] == 'D') ? 3 : 2;

    return true;
  }


  bool parseMdEchoback(CaptureSettings& settings, const string& line)
  {
    if (line.size() != 15) {
      error_message_ = "Invalid Mx packet has arrived.";
      return false;
    }

    settings.capture_first = substr2int(line, 2, 4);
    settings.capture_last = substr2int(line, 6, 4) + 1;
    int skip_lines = substr2int(line, 10, 2);
    settings.skip_lines = (skip_lines == 0) ? 1 : skip_lines;
    settings.skip_frames = substr2int(line, 12, 1);
    settings.remain_times = substr2int(line, 13, 2);
    settings.data_byte = (line[1] == 'D') ? 3 : 2;

    if (settings.remain_times == 1) {
      // 最後のデータ取得で、レーザを消灯扱いにする
      // 本当は、次のデータ取得後にレーザは消灯されている
      // 1 で判定すると、取得回数が 1 のときにも正常に動作するため
      mx_capturing_ = false;

    } else {
      if (settings.remain_times > 0) {
        mx_capturing_ = true;
      } else if (settings.remain_times == 0) {
        settings.remain_times = 100;
      }
    }

    return true;
  }


  bool parseMeEchoback(CaptureSettings& settings, const string& line)
  {
    if (line.size() != 15) {
      error_message_ = "Invalid ME packet has arrived.";
      return false;
    }

    settings.capture_first = substr2int(line, 2, 4);
    settings.capture_last = substr2int(line, 6, 4) + 1;
    int skip_lines = substr2int(line, 10, 2);
    settings.skip_lines = (skip_lines == 0) ? 1 : skip_lines;
    settings.skip_frames = substr2int(line, 12, 1);
    settings.remain_times = substr2int(line, 13, 2);
    settings.data_byte = 3;

    if (settings.remain_times == 1) {
      mx_capturing_ = false;

    } else {
      mx_capturing_ = true;
    }

    return true;
  }


  string addLengthData(vector<long>& data,
                       const string& line,
                       const string& left_packet_data,
                       const size_t data_byte, const int skip_lines = 1)
  {
    if (line.empty()) {
      // 空行の場合、戻る
      return left_packet_data;
    }

    // 端数。次回に処理する分
    string left_byte = left_packet_data;

    size_t data_size = (left_byte.size() + (line.size() - 1)) / data_byte;
    size_t n = data_size * data_byte - left_byte.size();
    for (size_t i = 0; i < n; ++i) {
      left_byte.push_back(line[i]);
      if (left_byte.size() >= data_byte) {
        // データを距離に変換して、格納
        long length = decode(&left_byte[0], data_byte);
        for (int j = 0; j < skip_lines; ++j) {
          data.push_back(length);
        }
        left_byte.clear();
      }
    }
    left_byte += line.substr(n, (line.size() - n) - 1);

    return left_byte;
  }
};


ScipHandler::ScipHandler(void) : pimpl(new pImpl)
{
}


ScipHandler::~ScipHandler(void)
{
}


const char* ScipHandler::what(void) const
{
  return pimpl->error_message_.c_str();
}


long ScipHandler::decode(const char* data, size_t size)
{
  const char* p = data;
  const char* last_p = p + size;

  int value = 0;
  while (p < last_p) {
    value <<= 6;
    value &= ~0x3f;
    value |= *p++ - 0x30;
  }
  return value;
}


bool ScipHandler::checkSum(const char* buffer, int size, char actual_sum)
{
  const char* p = buffer;
  const char* last_p = p + size;

  char expected_sum = 0x00;
  while (p < last_p) {
    expected_sum += *p++;
  }
  expected_sum = (expected_sum & 0x3f) + 0x30;

  return (expected_sum == actual_sum) ? true : false;
}


void ScipHandler::setConnection(Connection* con)
{
  pimpl->con_ = con;
}


Connection* ScipHandler::connection(void)
{
  return pimpl->con_;
}


bool ScipHandler::connect(const char* device, long baudrate)
{
  return pimpl->connect(device, baudrate);
}


int ScipHandler::send(const char data[], int size)
{
  if (size >= 2) {
    // コマンド送信後の受信前だと、レーザ点灯中の判定ができないため、
    // ここでレーザ点灯中の状態を判定する
    if ((! strncmp("MD", data, 2)) || (! strncmp("MS", data, 2)) ||
        (! strncmp("ME", data, 2))) {
      pimpl->laser_state_ = pImpl::LaserOn;
      pimpl->mx_capturing_ = true;
      pimpl->isPreCommand_QT_ = false;
    }
  }
  return pimpl->con_->send(data, size);
}


int ScipHandler::recv(char data[], int size, int timeout)
{
  return pimpl->con_->receive(data, size, timeout);
}


bool ScipHandler::loadParameter(RangeSensorParameter& parameters)
{
  return pimpl->loadParameter(parameters);
}


bool ScipHandler::versionLines(vector<string>& lines)
{
  int return_code = -1;
  char expected_response[] = { 0, -1 };
  if (! pimpl->response(return_code, "VV\n", expected_response, &lines)) {
    return false;
  }
  return true;
}


bool ScipHandler::setRawTimestampMode(bool on)
{
  char send_command[] = "TMx\n";
  send_command[2] = (on) ? '0' : '2';

  // TM0 or TM2 の送信
  int return_code = -1;
  char expected_response[] = { 0, -1 };
  if (! pimpl->response(return_code, send_command, expected_response)) {
    pimpl->error_message_ = (on) ? "TM0 fail." : "TM2 fail.";
    return false;
  }

  // TM1, TM2 の応答が正常ならば、レーザは消灯しているはず
  pimpl->laser_state_ = pImpl::LaserOff;

  return true;
}


bool ScipHandler::rawTimestamp(int* timestamp)
{
  // TM1 の値を返す
  int return_code = -1;
  char expected_response[] = { 0, -1 };
  vector<string> lines;
  if (! pimpl->response(return_code, "TM1\n", expected_response, &lines)) {
    pimpl->error_message_ = "TM1 fail.";
    return false;
  }

  if ((lines.size() != 1) || (lines[0].size() != 5)) {
    pimpl->error_message_ = "response mismatch.";
    return false;
  }

  *timestamp = decode(lines[0].c_str(), 4);
  return true;
}


bool ScipHandler::setLaserOutput(bool on, bool force)
{
  return pimpl->setLaserOutput(on, force);
}


CaptureType ScipHandler::receiveCaptureData(vector<long>& data,
                                            CaptureSettings& settings,
                                            long* timestamp, int* remain_times,
                                            int* total_times)
{
  return pimpl->receiveCaptureData(data, settings,
                                   timestamp, remain_times, total_times);
}
