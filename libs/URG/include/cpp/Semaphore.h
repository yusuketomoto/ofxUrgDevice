#ifndef QRK_SEMAPHORE_H
#define QRK_SEMAPHORE_H

/*!
  \file
  \brief セマフォ管理

  \author Satofumi KAMIMURA

  $Id: Semaphore.h 1949 2011-05-06 07:18:01Z satofumi $

  ドキュメントは、SDL マニュアルを参考にしている
*/

#include <memory>
#include <cstddef>


namespace qrk
{
  /*!
    \brief セマフォ管理
  */
  class Semaphore
  {
    Semaphore(void);
    Semaphore(const Semaphore& rhs);
    Semaphore& operator = (const Semaphore& rhs);

    struct pImpl;
    const std::auto_ptr<pImpl> pimpl;

  public:
    /*!
      \brief コンストラクタ

      \param[in] initial_value 初期値
    */
    explicit Semaphore(size_t initial_value);
    ~Semaphore(void);


    /*!
      \brief デクリメント

      セマフォをロックし、セマフォの値がゼロのときは、スレッドを停止させる
    */
    void wait(void);


    /*!
      \brief スレッドが停止しないデクリメント

      スレッドを停止させずに、セマフォのロックを試みる

      \retval true ロックに成功
      \retval false ロックに失敗 or エラー
    */
    bool tryWait(void);


    /*!
      \brief インクリメント

      セマフォの値を加算する
    */
    void post(void);


    /*!
      \brief セマフォ値の取得

      \return セマフォ値
    */
    size_t value(void);
  };
}

#endif /*! QRK_SEMAPHORE_H */
