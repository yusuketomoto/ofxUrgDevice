#ifndef QRK_TCPIP_SERVER_H
#define QRK_TCPIP_SERVER_H

/*!
  \file
  \brief TCP/IP サーバ

  \author Satofumi KAMIMURA

  $Id: TcpipServer.h 1836 2010-05-26 11:23:08Z satofumi $
*/

#include "Connection.h"
#include <memory>


namespace qrk
{
    //! TCP/IP サーバ
    class TcpipServer : public Connection
    {
    public:
        enum {
            NoTimeout = -1,
        };
        TcpipServer(void);
        ~TcpipServer(void);

        const char* what(void) const;

        bool activate(long port);
        void deactivate(void);
        bool accept(int timeout = NoTimeout);

        bool connect(const char* host, long port);
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
        TcpipServer(const TcpipServer& rhs);
        TcpipServer& operator = (const TcpipServer& rhs);

        struct pImpl;
        const std::auto_ptr<pImpl> pimpl;
    };
}

#endif /* !QRK_TCPIP_SERVER_H */
