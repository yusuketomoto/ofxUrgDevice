/*!
  \file
  \brief シリアル通信

  \author Satofumi KAMIMURA

  $Id: SerialDevice.cpp 1977 2012-04-05 08:04:51Z satofumi $
*/

#include "SerialDevice.h"
#include "DetectOS.h"
#include "RingBuffer.h"
#include <string>

using namespace qrk;
using namespace std;


#if defined(WINDOWS_OS)
#include "SerialDevice_win.cpp"   // Windows (win32) 環境
#else
#include "SerialDevice_lin.cpp"   // Linux, Mac 環境 (共通)
#endif


struct SerialDevice::pImpl
{
    string error_message_;
    long baudrate_;
    RawSerialDevice raw_;
    RingBuffer<char> ring_buffer_; //!< 受信バッファ


    pImpl(void) : error_message_("no error"), baudrate_(0)
    {
    }


    void updateRingBuffer(void)
    {
        enum { BufferSize = 2048 };
        char buffer[BufferSize];

        int n = raw_.receive(buffer, BufferSize, 0);
        if (n > 0) {
            ring_buffer_.put(buffer, n);
        }
    }


    int receive(char* data, size_t count, int timeout)
    {
        if (! isConnected()) {
            error_message_ = "no connection.";
            return -1;
        }
        if (count == 0) {
            return 0;
        }

        size_t filled = 0;

        size_t ring_filled = ring_buffer_.size();
        if (ring_filled < count) {
            updateRingBuffer();
        }

        // バッファにデータがある場合、バッファからデータを格納する
        size_t read_size = std::min(count, ring_buffer_.size());
        ring_buffer_.get(data, read_size);
        filled += read_size;

        // バッファが空の場合、残りのデータはシステムから直接読み込む
        read_size = max(0, static_cast<int>(count - filled));
        if (read_size > 0) {
            int n = raw_.receive(&data[filled],
                                 static_cast<int>(read_size), timeout);
            if (n < 0) {
                error_message_ = raw_.what();
                return n;
            }
            filled += n;
        }
        return static_cast<int>(filled);
    }


    bool isConnected(void)
    {
        return raw_.isConnected();
    }
};


SerialDevice::SerialDevice(void) : pimpl(new pImpl)
{
}


SerialDevice::~SerialDevice(void)
{
    disconnect();
}


const char* SerialDevice::what(void) const
{
    return pimpl->error_message_.c_str();
}


bool SerialDevice::connect(const char* device, long baudrate)
{
    disconnect();
    clear();
    if (! pimpl->raw_.connect(device, baudrate)) {
        pimpl->error_message_ = pimpl->raw_.what();
        return false;
    } else {
        return true;
    }
}


void SerialDevice::disconnect(void)
{
    return pimpl->raw_.disconnect();
}


bool SerialDevice::setBaudrate(long baudrate)
{
    if (! pimpl->raw_.setBaudrate(baudrate)) {
        pimpl->error_message_ = pimpl->raw_.what();
        pimpl->baudrate_ = 0;
        return false;
    }
    pimpl->baudrate_ = baudrate;
    return true;
}


long SerialDevice::baudrate(void) const
{
    return pimpl->baudrate_;
}


bool SerialDevice::isConnected(void) const
{
    return pimpl->isConnected();
}


int SerialDevice::send(const char* data, size_t count)
{
    if (! isConnected()) {
        pimpl->error_message_ = "no connection.";
        return 0;
    }

    int n = pimpl->raw_.send(data, static_cast<int>(count));
    if (n < 0) {
        pimpl->error_message_ = pimpl->raw_.what();
    }
    return n;
}


int SerialDevice::receive(char* data, size_t count, int timeout)
{
    if (! isConnected()) {
        pimpl->error_message_ = "no connection.";
        return 0;
    }

    return pimpl->receive(data, count, timeout);
}


size_t SerialDevice::size(void) const
{
    pimpl->updateRingBuffer();
    return pimpl->ring_buffer_.size();
}


void SerialDevice::flush(void)
{
    if (! isConnected()) {
        pimpl->error_message_ = "no connection.";
        return;
    }

    return pimpl->raw_.flush();
}


void SerialDevice::clear(void)
{
    pimpl->raw_.flush();
    pimpl->ring_buffer_.clear();
}


void SerialDevice::ungetc(const char ch)
{
    if (! isConnected()) {
        pimpl->error_message_ = "no connection.";
        return;
    }

    pimpl->ring_buffer_.ungetc(ch);
}
