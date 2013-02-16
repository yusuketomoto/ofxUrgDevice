/*!
  \file
  \brief ロックガード

  \author Satofumi KAMIMURA

  $Id: LockGuard.cpp 772 2009-05-05 06:57:57Z satofumi $
*/

#include "LockGuard.h"
#include "Lock.h"

using namespace qrk;


struct LockGuard::pImpl
{
  Lock& mutex_;


  pImpl(Lock& mutex) : mutex_(mutex)
  {
    mutex_.lock();
  }


  ~pImpl(void)
  {
    mutex_.unlock();
  }
};


LockGuard::LockGuard(Lock& mutex) : pimpl(new pImpl(mutex))
{
}


LockGuard::~LockGuard(void)
{
}
