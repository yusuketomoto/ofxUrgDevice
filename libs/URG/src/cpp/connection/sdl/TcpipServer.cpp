/*!
  \file
  \brief TCP/IP のサーバ

  \author Satofumi KAMIMURA

  $Id: TcpipServer.cpp 1836 2010-05-26 11:23:08Z satofumi $

  \todo TcpipAccept を用いた実装に修正する
  \todo SocketSet を共通にできる仕組みを作るかを検討する
  \todo TcpipAccept を使うことにして、削除してしまう
*/

#include "TcpipServer.h"
#include "SdlNetInit.h"
#include "TcpipSocket.h"
#include "DetectOS.h"
#include <SDL_net.h>
#include <string>

using namespace qrk;
using namespace std;

#ifdef MSC
#define snprintf _snprintf
#endif


struct TcpipServer::pImpl : private SdlNetInit
{
    string error_message_;
    IPaddress ip_address_;
    TCPsocket accept_socket_;
    SDLNet_SocketSet accept_set_;
    bool is_activated_;

    TcpipSocket* con_;


    pImpl(void)
        : error_message_("no error."),
          accept_socket_(NULL), accept_set_(NULL),
          is_activated_(false), con_(NULL)
    {
    }


    ~pImpl(void)
    {
        if (con_) {
            delete con_;
        }
    }


    bool activate(long port)
    {
        if (is_activated_) {
            disconnect();
            deactivate();
        }

        if (SDLNet_ResolveHost(&ip_address_, NULL,
                               static_cast<Uint16>(port)) < 0) {
            enum { BufferSize = 256 };
            char buffer[BufferSize];
            snprintf(buffer, BufferSize - 1, "%ld", port);

            error_message_ =
                string("activate(): can not assign port ")
                + buffer + " :" + SDLNet_GetError();
            return false;
        }

        accept_socket_ = SDLNet_TCP_Open(&ip_address_);
        if (accept_socket_ == NULL) {
            error_message_ = string("activate(): could not open socket.")
                + " :" + SDLNet_GetError();
            return false;
        }

        accept_set_ = SDLNet_AllocSocketSet(1);
        if (accept_set_ == NULL) {
            error_message_ = string("accept(): could not allock socket set.")
                + ": " + SDLNet_GetError();
            return false;
        }

        if (SDLNet_TCP_AddSocket(accept_set_, accept_socket_) < 0) {
            error_message_ = string("accept(): could not add socket.")
                + " :" + SDLNet_GetError();
            return false;
        }

        is_activated_ = true;
        return true;
    }


    void deactivate(void)
    {
        if (! is_activated_) {
            return;
        }

        if (SDLNet_TCP_DelSocket(accept_set_, accept_socket_) < 0) {
            error_message_ = "deactivate(): SDLNet_GetError()";
            return;
        }

        SDLNet_FreeSocketSet(accept_set_);
        accept_set_ = NULL;

        SDLNet_TCP_Close(accept_socket_);
        accept_socket_ = NULL;

        is_activated_ = false;
    }


    bool accept(int timeout)
    {
        if (is_activated_ == false) {
            error_message_ = "call activate() before accept() called.";
            return false;
        }

        int n = SDLNet_CheckSockets(accept_set_, timeout);
        if ((n <= 0) || (! SDLNet_SocketReady(accept_socket_))) {
            // 接続なし
            return false;
        }

        TCPsocket socket = SDLNet_TCP_Accept(accept_socket_);
        if (socket == NULL) {
            return false;
        }

        if (con_) {
            delete con_;
        }
        con_ = new TcpipSocket(socket);
        return true;
    }


    void disconnect(void)
    {
        if (con_) {
            con_->disconnect();
        }
    }
};


TcpipServer::TcpipServer(void) : pimpl(new pImpl)
{
}


TcpipServer::~TcpipServer(void)
{
    deactivate();
}


const char* TcpipServer::what(void) const
{
    return pimpl->error_message_.c_str();
}


bool TcpipServer::activate(long port)
{
    return pimpl->activate(port);
}


void TcpipServer::deactivate(void)
{
    pimpl->deactivate();
}


bool TcpipServer::accept(int timeout)
{
    return pimpl->accept(timeout);
}


bool TcpipServer::connect(const char* host, long port)
{
    if (! isConnected()) {
        return false;
    }

    return pimpl->con_->connect(host, port);
}


void TcpipServer::disconnect(void)
{
    pimpl->disconnect();
}


bool TcpipServer::setBaudrate(long baudrate)
{
    if (! isConnected()) {
        return false;
    }
    return pimpl->con_->setBaudrate(baudrate);
}


long TcpipServer::baudrate(void) const
{
    if (! isConnected()) {
        return -1;
    }
    return pimpl->con_->baudrate();
}


bool TcpipServer::isConnected(void) const
{
    if (! pimpl->con_) {
        return false;
    }
    return pimpl->con_->isConnected();
}


int TcpipServer::send(const char* data, size_t count)
{
    if (! pimpl->con_) {
        return -1;
    }
    return pimpl->con_->send(data, count);
}


int TcpipServer::receive(char* data, size_t count, int timeout)
{
    if (! pimpl->con_) {
        return -1;
    }
    return pimpl->con_->receive(data, count, timeout);
}


size_t TcpipServer::size(void) const
{
    if (! pimpl->con_) {
        return 0;
    }
    return pimpl->con_->size();
}


void TcpipServer::flush(void)
{
    if (! pimpl->con_) {
        return;
    }
    return pimpl->con_->flush();
}


void TcpipServer::clear(void)
{
    if (! pimpl->con_) {
        return;
    }
    return pimpl->con_->clear();
}


void TcpipServer::ungetc(const char ch)
{
    if (! pimpl->con_) {
        return;
    }
    return pimpl->con_->ungetc(ch);
}
