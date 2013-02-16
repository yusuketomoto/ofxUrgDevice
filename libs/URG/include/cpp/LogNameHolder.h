#ifndef QRK_LOG_NAME_HOLDER_H
#define QRK_LOG_NAME_HOLDER_H

/*!
  \file
  \brief ÉçÉOñºÇÃä«óù

  \author Satofumi KAMIMURA

  $Id: LogNameHolder.h 783 2009-05-05 08:56:26Z satofumi $
*/

#include <memory>
#include <string>


namespace qrk
{
  /*!
    \brief ÉçÉOñºÇÃä«óù
  */
  class LogNameHolder
  {
    LogNameHolder(void);
    LogNameHolder(const LogNameHolder& rhs);
    LogNameHolder& operator = (const LogNameHolder& rhs);

    struct pImpl;
    const std::auto_ptr<pImpl> pimpl;

  public:
    ~LogNameHolder(void);
    static LogNameHolder* object(void);

    std::string name(const char* baseName);
  };
}

#endif /* !QRK_LOG_NAME_HOLDER_H */
