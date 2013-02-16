#ifndef QRK_DEVICE_IP_MANAGER_H
#define QRK_DEVICE_IP_MANAGER_H

/*!
  \file
  \brief シミュレータ接続用の IP ポート管理

  \author Satofumi KAMIMURA

  $Id: DeviceIpManager.h 783 2009-05-05 08:56:26Z satofumi $
*/

#include <memory>


namespace qrk
{
  /*!
    \brief シミュレータ接続用の IP ポート管理
  */
  class DeviceIpManager
  {
    DeviceIpManager(void);
    DeviceIpManager(const DeviceIpManager& rhs);
    DeviceIpManager& operator = (const DeviceIpManager& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;

  public:
    ~DeviceIpManager(void);
    static DeviceIpManager* object(void);

    long createIpPort(const char* device, int sub_id = 0);
  };
}

#endif /* !QRK_DEVICE_IP_MANAGER_H */
