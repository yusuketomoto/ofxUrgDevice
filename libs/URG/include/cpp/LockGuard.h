#ifndef QRK_LOCK_GUARD_H
#define QRK_LOCK_GUARD_H

/*!
  \file
  \brief ロックガードクラス

  \author Satofumi KAMIMURA

  $Id: LockGuard.h 1954 2011-07-17 22:08:18Z satofumi $
*/

#include <memory>
#include <cstddef>


namespace qrk
{
  class Lock;

  /*!
    \brief ロックガードクラス
  */
  class LockGuard
  {
    LockGuard(void);
    LockGuard(const LockGuard& rhs);
    LockGuard& operator = (const LockGuard& rhs);

    //void* operator new (size_t);
    //void* operator new[] (size_t);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;

  public:
    /*!
      \brief コンストラクタ

      \param[in] mutex ミューテックス資源
    */
    explicit LockGuard(qrk::Lock& mutex);
    ~LockGuard(void);
  };
}

#endif /* !QRK_LOCK_GUARD_H */
