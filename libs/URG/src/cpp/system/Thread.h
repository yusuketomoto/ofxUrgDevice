#ifndef QRK_THREAD_H
#define QRK_THREAD_H

/*!
  \file
  \brief スレッド処理のラッパー

  \author Satofumi KAMIMURA

  $Id: Thread.h 914 2009-05-20 22:16:35Z satofumi $
*/

#include <memory>


namespace qrk
{
  /*!
    \brief スレッド処理
  */
  class Thread
  {
    Thread(void);
    Thread(const Thread& rhs);
    Thread& operator = (const Thread& rhs);

    struct pImpl;
    const std::auto_ptr<pImpl> pimpl;

  public:
    enum {
      Infinity = -1,
    };

    /*!
      \brief コンストラクタ

      \param[in,out] fn 関数ポインタ
      \param[in,out] args スレッド関数への引数
    */
    explicit Thread(int (*fn)(void *), void* args);
    ~Thread(void);


    /*!
      \brief 処理回数を指定してスレッドを起動

      \param[in] times 処理回数
    */
    void run(int times = 1);


    /*!
      \brief スレッドを停止
    */
    void stop(void);


    /*!
      \brief スレッドの終了を待つ

      \return スレッドの戻り値
    */
    int wait(void);


    /*!
      \brief スレッドが動作中かを返す

      \retval true 動作中
      \retval false 停止中
    */
    bool isRunning(void) const;
  };
}

#endif /* !QRK_THREAD_H */
