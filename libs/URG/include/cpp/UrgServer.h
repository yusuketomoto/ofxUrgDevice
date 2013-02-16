#ifndef QRK_URG_SERVER_H
#define QRK_URG_SERVER_H

/*!
  \file
  \brief シミュレータ用の URG サーバ

  \author Satofumi KAMIMURA

  $Id: UrgServer.h 783 2009-05-05 08:56:26Z satofumi $
*/

#include "DeviceServer.h"
#include <memory>


namespace qrk
{
  /*!
    \brief シミュレータ用の URG サーバ
  */
  class UrgServer : public DeviceServer
  {
    UrgServer(const UrgServer& rhs);
    UrgServer& operator = (const UrgServer& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;

  public:
    UrgServer(void);
    ~UrgServer(void);

    bool activate(long port);
    int updateInterval(void);
    void update(void);
  };
}

#endif /* !QRK_URG_SERVER_H */
