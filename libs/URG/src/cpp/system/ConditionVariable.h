#ifndef QRK_CONDITION_VARIABLE_H
#define QRK_CONDITION_VARIABLE_H

/*!
  \file
  \brief ğŒ•Ï”

  \author Satofumi KAMIMURA

  $Id: ConditionVariable.h 783 2009-05-05 08:56:26Z satofumi $
*/

#include <memory>


namespace qrk
{
  class Lock;

  /*!
    \brief ğŒ•Ï”
  */
  class ConditionVariable
  {
    ConditionVariable(const ConditionVariable& rhs);
    ConditionVariable& operator = (const ConditionVariable& rhs);

    struct pImpl;
    const std::auto_ptr<pImpl> pimpl;

  public:
    enum {
      NoTimeout = -1,
    };
    ConditionVariable(void);
    ~ConditionVariable(void);

    bool isWaiting(void);
    void wakeup(void);
    void wakeupAll(void);

    bool wait(Lock* lock, int timeout = NoTimeout);
  };
}

#endif /* !QRK_CONDITION_VARIABLE_H */
