#ifndef QRK_URG_USB_COM_H
#define QRK_URG_USB_COM_H

/*!
  \file
  \brief Search for USB connected port

  \author Satofumi KAMIMURA

  $Id: UrgUsbCom.h 1723 2010-02-24 23:30:34Z satofumi $
*/

#include "IsUsbCom.h"


namespace qrk
{
  //! Search for USB connected port
  class UrgUsbCom : public IsUsbCom
  {
  public:
    UrgUsbCom(void);

    std::vector<std::string> baseNames(void);
    bool isUsbCom(const char* com_port);

  private:
    UrgUsbCom(const UrgUsbCom& rhs);
    UrgUsbCom& operator = (const UrgUsbCom& rhs);
  };
}

#endif /* !QRK_URG_USB_COM_H */
