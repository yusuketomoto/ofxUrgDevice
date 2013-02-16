///*!
//  \file
//  \brief シリアル通信の実処理 (Windows)
//
//  \author Satofumi KAMIMURA
//
//  $Id: SerialDevice_win.cpp 1811 2010-04-30 16:12:05Z satofumi $
//*/
//
//#include "DetectOS.h"
//#include <windows.h>
//#include <setupapi.h>
//#include <string>
//#include <cstdio>
//
//#ifdef MSC
//#define snprintf _snprintf
//#endif
//
//using namespace std;
//
//#undef min
//#undef max
//
//
//class RawSerialDevice
//{
//    string error_message_;
//    HANDLE hCom_;
//    int current_timeout_;
//    string com_name_;
//
//
//public:
//    RawSerialDevice(void)
//        : error_message_("no error."), hCom_(INVALID_HANDLE_VALUE),
//          current_timeout_(0)
//    {
//    }
//
//
//    const char* what(void)
//    {
//        return error_message_.c_str();
//    }
//
//
//    bool connect(const char* device, long baudrate)
//    {
//        /* COM ポートを開く */
//        enum { NameLength = 11 };
//        char adjusted_device[NameLength];
//        snprintf(adjusted_device, NameLength, "\\\\.\\%s", device);
//        hCom_ = CreateFileA(adjusted_device, GENERIC_READ | GENERIC_WRITE, 0,
//                            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//
//        if (hCom_ == INVALID_HANDLE_VALUE) {
//            error_message_ = string("open failed: ") + device;
//            return false;
//        }
//        com_name_ = device;
//
//        // 通信サイズの更新
//        SetupComm(hCom_, 4096 * 8, 4096);
//
//        // タイムアウト設定
//        setTimeout(current_timeout_);
//
//        // ボーレートの変更
//        bool ret = setBaudrate(baudrate);
//        if (! ret) {
//            error_message_ = "fail SerialDevice::setBaudrate()";
//        }
//
//        return true;
//    }
//
//
//    void setTimeout(int timeout)
//    {
//        COMMTIMEOUTS timeouts;
//        GetCommTimeouts(hCom_, &timeouts);
//
//        timeouts.ReadIntervalTimeout = (timeout == 0) ? MAXDWORD : 0;
//        timeouts.ReadTotalTimeoutConstant = timeout;
//        timeouts.ReadTotalTimeoutMultiplier = 0;
//
//        SetCommTimeouts(hCom_, &timeouts);
//    }
//
//
//
//    void disconnect(void)
//    {
//        if (hCom_ != INVALID_HANDLE_VALUE) {
//            CloseHandle(hCom_);
//            hCom_ = INVALID_HANDLE_VALUE;
//        }
//    }
//
//
//    bool isConnected(void)
//    {
//        return (hCom_ == INVALID_HANDLE_VALUE) ? false : true;
//    }
//
//
//    bool setBaudrate(long baudrate)
//    {
//        long baudrate_value;
//        switch (baudrate) {
//
//        case 4800:
//            baudrate_value = CBR_4800;
//            break;
//
//        case 9600:
//            baudrate_value = CBR_9600;
//            break;
//
//        case 19200:
//            baudrate_value = CBR_19200;
//            break;
//
//        case 38400:
//            baudrate_value = CBR_38400;
//            break;
//
//        case 57600:
//            baudrate_value = CBR_57600;
//            break;
//
//        case 115200:
//            baudrate_value = CBR_115200;
//            break;
//
//        default:
//            baudrate_value = baudrate;
//        }
//
//        DCB dcb;
//        GetCommState(hCom_, &dcb);
//        dcb.BaudRate = baudrate_value;
//        dcb.ByteSize = 8;
//        dcb.Parity = NOPARITY;
//        dcb.fParity = FALSE;
//        dcb.StopBits = ONESTOPBIT;
//        if (SetCommState(hCom_, &dcb) == 0) {
//            flush();
//            return false;
//        } else {
//            return true;
//        }
//    }
//
//
//    int send(const char* data, size_t count)
//    {
//        if (count <= 0) {
//            return 0;
//        }
//
//        DWORD n;
//        WriteFile(hCom_, data, (DWORD)count, &n, NULL);
//        return n;
//    }
//
//
//    int receive(char data[], int count, int timeout)
//    {
//        if (count <= 0) {
//            return 0;
//        }
//
//        if (timeout != current_timeout_) {
//            setTimeout(timeout);
//            current_timeout_ = timeout;
//        }
//
//        DWORD n;
//        ReadFile(hCom_, data, count, &n, NULL);
//
//        return n;
//    }
//
//
//    void flush(void)
//    {
//        PurgeComm(hCom_,
//                  PURGE_RXABORT | PURGE_TXABORT |
//                  PURGE_RXCLEAR | PURGE_TXCLEAR);
//    }
//};
