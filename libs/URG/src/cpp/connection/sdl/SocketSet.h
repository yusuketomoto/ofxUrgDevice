#ifndef QRK_SOCKET_SET_H
#define QRK_SOCKET_SET_H

/*!
  \file
  \brief 通信ソケット管理

  \author Satofumi KAMIMURA

  $Id: SocketSet.h 1836 2010-05-26 11:23:08Z satofumi $
*/

#ifdef check
#undef check
#endif

#include <SDL_net.h>
#include <memory>

namespace qrk
{
    class TcpipCtrl;


    //! 通信ソケット管理
    class SocketSet
    {
    public:
        /*!
          \brief コンストラクタ

          \param size [i] 初期状態でのソケット許容数
        */
        explicit SocketSet(size_t size = 1);
        ~SocketSet(void);


        /*!
          \brief ソケットの登録

          \param socket [i] 登録するソケット

          \retval true 登録に成功
          \retval false 登録に失敗
        */
        bool add(TCPsocket socket);


        /*!
          \brief ソケットの削除

          \param socket [i] 削除するソケット
        */
        void del(TCPsocket socket);


        /*!
          \brief 受信データがあるかの確認

          \param timeout [i] タイムアウト時間 [msec]

          \return 受信データのあるソケット数
        */
        size_t check(int timeout);

    private:
        SocketSet(const SocketSet& rhs);
        SocketSet& operator = (const SocketSet& rhs);

        struct pImpl;
        const std::auto_ptr<pImpl> pimpl;
    };
}

#endif /* !QRK_SOCKET_SET_H */
