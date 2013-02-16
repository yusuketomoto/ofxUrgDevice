#ifndef QRK_SERIAL_DEVICE_H
#define QRK_SERIAL_DEVICE_H

/*!
  \file
  \brief シリアル通信

  \author Satofumi KAMIMURA

  $Id: SerialDevice.h 1811 2010-04-30 16:12:05Z satofumi $
*/

#include "Connection.h"
#include <memory>


namespace qrk
{
    //! シリアル通信クラス
    class SerialDevice : public Connection
    {
    public:
        enum {
            DefaultBaudrate = 115200,
        };
        SerialDevice(void);
        ~SerialDevice(void);

        const char* what(void) const;

        bool connect(const char* device, long baudrate = DefaultBaudrate);
        void disconnect(void);
        bool setBaudrate(long baudrate);
        long baudrate(void) const;
        bool isConnected(void) const;
        int send(const char* data, size_t count);
        int receive(char* data, size_t count, int timeout);
        size_t size(void) const;
        void flush(void);
        void clear(void);
        void ungetc(const char ch);

    private:
        SerialDevice(const SerialDevice& rhs);
        SerialDevice& operator = (const SerialDevice& rhs);

        struct pImpl;
        const std::auto_ptr<pImpl> pimpl;
    };
}

#endif /* !QRK_SERIAL_DEVICE_H */
