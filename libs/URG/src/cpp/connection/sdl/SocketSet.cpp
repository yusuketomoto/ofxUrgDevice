/*!
  \file
  \brief 通信ソケット管理

  \author Satofumi KAMIMURA

  $Id: SocketSet.cpp 1836 2010-05-26 11:23:08Z satofumi $
*/

#include "SocketSet.h"
#include "SdlNetInit.h"
#include "TcpipSocket.h"

using namespace qrk;


/*!
  \brief SocketSet の内部クラス
*/
struct SocketSet::pImpl : private SdlNetInit
{
    size_t max_num;
    size_t hold_num;
    SDLNet_SocketSet socket_set;

    pImpl(size_t size)
        : max_num(size), hold_num(0),
          socket_set(SDLNet_AllocSocketSet(static_cast<int>(max_num)))
    {
    }


    ~pImpl(void)
    {
        SDLNet_FreeSocketSet(socket_set);
    }
};


SocketSet::SocketSet(size_t size) : pimpl(new pImpl(size))
{
}


SocketSet::~SocketSet(void)
{
}


bool SocketSet::add(TCPsocket socket)
{
    if (pimpl->hold_num < pimpl->max_num) {
        if (SDLNet_TCP_AddSocket(pimpl->socket_set, socket) >= 0) {
            ++(pimpl->hold_num);
            return true;
        }
    }
    return false;
}


void SocketSet::del(TCPsocket socket)
{
    if ((pimpl->hold_num > 0) &&
        (SDLNet_TCP_DelSocket(pimpl->socket_set, socket) >= 0)) {
        --(pimpl->hold_num);
    }
}


size_t SocketSet::check(int timeout)
{
    return SDLNet_CheckSockets(pimpl->socket_set, timeout);
}
