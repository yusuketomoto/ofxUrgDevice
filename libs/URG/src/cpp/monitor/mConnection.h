#ifndef QRK_M_CONNECTION_H
#define QRK_M_CONNECTION_H

/*!
  \file
  \brief モニタ対応の通信インターフェース

  \author Satofumi KAMIMURA

  $Id: mConnection.h 1302 2009-09-15 21:51:30Z satofumi $
*/

#include "Connection.h"
#include <memory>


namespace qrk
{
  /*!
    \brief モニタ対応の通信インターフェース
  */
  class mConnection : public Connection
  {
    mConnection(void);
    mConnection(const mConnection& rhs);
    mConnection& operator = (const mConnection& rhs);

    struct pImpl;
    const std::auto_ptr<pImpl> pimpl;

  public:
    explicit mConnection(Connection* connection);
    ~mConnection(void);

    const char* what(void) const;

    bool connect(const char* device, long baudrate);
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
  };
}

#endif /* !QRK_CONNECTION_H */
