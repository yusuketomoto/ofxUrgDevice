/*!
  \file
  \brief TCP/IP 通信

  \author Satofumi KAMIMURA

  $Id: TcpipSocket.cpp 1836 2010-05-26 11:23:08Z satofumi $
*/

#include "DetectOS.h"
#include "TcpipSocket.h"
#include "SdlNetInit.h"
#include "SocketSet.h"
#include "RingBuffer.h"
#include <string>

#ifdef WINDOWS_OS
#define snprintf _snprintf
#endif

using namespace qrk;
using namespace std;


struct TcpipSocket::pImpl : private SdlNetInit
{
    string error_message_;
    TCPsocket socket_;
    SocketSet* socket_set_;
    SDLNet_SocketSet own_set_;
    RingBuffer<char> ring_buffer_;
    bool have_data_;
    long dummy_baudrate_;


    pImpl(TCPsocket socket, SocketSet* socket_set)
        : error_message_("no error."), socket_(socket),
          socket_set_(socket_set), own_set_(NULL), have_data_(false),
          dummy_baudrate_(0)
    {
        if (socket_set_ == NULL) {
            own_set_ = SDLNet_AllocSocketSet(1);
            if (socket_ != NULL) {
                addSocket(socket_);
            }
        } else {
            socket_set_->add(socket_);
        }
    }


    ~pImpl(void)
    {
        disconnect();
        if (own_set_) {
            SDLNet_FreeSocketSet(own_set_);
        }
    }


    void disconnect(void)
    {
        if (socket_) {
            if (socket_set_) {
                socket_set_->del(socket_);
            } else {
                deleteSocket(socket_);
            }

            SDLNet_TCP_Close(socket_);
            socket_ = NULL;
        }
        error_message_ = "disconnected.";
    }


    void addSocket(TCPsocket socket)
    {
        SDLNet_TCP_AddSocket(own_set_, socket);
    }


    void deleteSocket(TCPsocket socket)
    {
        SDLNet_TCP_DelSocket(own_set_, socket);
    }


    int update(int timeout, int size = 0)
    {
        enum { BufferSize = 1024 };
        int require_size = (size > 0) ? size : BufferSize;
        int filled = 0;
        while (filled < require_size) {
            if (socket_set_ == NULL) {
                if (SDLNet_CheckSockets(own_set_,
                                        (size == 0) ? 0 : timeout) <= 0) {
                    break;
                }
            }
            if (! SDLNet_SocketReady(socket_)) {
                break;
            }
            char data[BufferSize];
            int read_n = require_size - filled;
            int n =
                SDLNet_TCP_Recv(socket_, data,
                                (read_n > BufferSize) ? BufferSize : read_n);
            if (n <= 0) {
                disconnect();
                return -1;
            }
            ring_buffer_.put(data, n);
            have_data_ = true;
            filled += n;

            if (socket_set_ != NULL) {
                // SocketSet を使ったチェックの関係で、読み込みは１回のみ
                break;
            }
        }
        return 0;
    }
};


TcpipSocket::TcpipSocket(void) : pimpl(new pImpl(NULL, NULL))
{
}


TcpipSocket::TcpipSocket(void* socket)
    : pimpl(new pImpl(static_cast<TCPsocket>(socket), NULL))
{
}


TcpipSocket::TcpipSocket(SocketSet* socket_set, void* socket)
    : pimpl(new pImpl(static_cast<TCPsocket>(socket), socket_set))
{
}


TcpipSocket::~TcpipSocket(void)
{
}


const char* TcpipSocket::what(void) const
{
    return pimpl->error_message_.c_str();
}


bool TcpipSocket::connect(const char* host, long port)
{
    IPaddress ip;
    int ret = SDLNet_ResolveHost(&ip, host, static_cast<short>(port));
    if (ret < 0) {
        pimpl->error_message_ =
            string("SDLNet_ResolveHost(): ") + SDLNet_GetError();
        return false;
    }

    TCPsocket socket = SDLNet_TCP_Open(&ip);
    if (socket == NULL) {
        char strnum[13];
        sprintf(strnum, "%ld", port);
        pimpl->error_message_ =
            string(SDLNet_GetError()) + " (" + host + ", " + strnum + ")";
        return false;
    }
    disconnect();
    swap(socket, pimpl->socket_);

    // ソケットを登録
    if (pimpl->socket_set_) {
        pimpl->socket_set_->add(pimpl->socket_);
    } else {
        pimpl->addSocket(pimpl->socket_);
    }
    pimpl->error_message_ = "connected.";
    return true;
}


void TcpipSocket::disconnect(void)
{
    pimpl->disconnect();
}


bool TcpipSocket::setBaudrate(long baudrate)
{
    pimpl->dummy_baudrate_ = baudrate;
    return true;
}


long TcpipSocket::baudrate(void) const
{
    return pimpl->dummy_baudrate_;
}


bool TcpipSocket::isConnected(void) const
{
    return (pimpl->socket_ == NULL) ? false : true;
}


int TcpipSocket::send(const char* data, size_t count)
{
    if (! isConnected()) {
        return -1;
    }
    return SDLNet_TCP_Send(pimpl->socket_,
                           const_cast<char*>(data), static_cast<int>(count));
}


int TcpipSocket::receive(char* data, size_t count, int timeout)
{
    if (! isConnected()) {
        return -1;
    } else if (count == 0) {
        return 0;
    }

    // 受信データの更新
    if ((pimpl->update(timeout) < 0) && (! pimpl->have_data_)) {
        // 切断を検出
        return -1;
    }

    // 要求分だけ受信データがあれば、返す
    size_t filled = pimpl->ring_buffer_.size();
    if (filled >= count) {
        pimpl->ring_buffer_.get(data, static_cast<int>(count));
        pimpl->have_data_ = ! pimpl->ring_buffer_.empty();
        return static_cast<int>(count);
    }

    // 受信できたデータを返す
    int left = static_cast<int>(count - filled);
    if ((pimpl->own_set_ != NULL) && (pimpl->update(timeout, left) < 0)
        && (! pimpl->have_data_)) {
        // 切断を検出
        return -1;
    }

    // size() を呼ぶと update(0) が呼ばれるため
    filled = pimpl->ring_buffer_.size();
    pimpl->ring_buffer_.get(data, filled);
    pimpl->have_data_ = ! pimpl->ring_buffer_.empty();

    return static_cast<int>(filled);
}


size_t TcpipSocket::size(void) const
{
    if ((pimpl->update(0) < 0) && (! pimpl->have_data_)) {
        return 0;
    }
    return pimpl->ring_buffer_.size();
}


void TcpipSocket::flush(void)
{
    // 実装方法、不明
}


void TcpipSocket::clear(void)
{
    pimpl->ring_buffer_.clear();
}


void TcpipSocket::ungetc(const char ch)
{
    if (! isConnected()) {
        pimpl->error_message_ = "no connection.";
        return;
    }

    pimpl->ring_buffer_.ungetc(ch);
}
