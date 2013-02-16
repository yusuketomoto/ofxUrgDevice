/*!
  \file
  \brief モニタ対応の通信インターフェース

  \author Satofumi KAMIMURA

  $Id: mConnection.cpp 1313 2009-09-17 01:11:03Z satofumi $
*/

#include "mConnection.h"
#include "MonitorDataHandler.h"
#include "LogNameHolder.h"
#include <string>

using namespace qrk;
using namespace std;


struct mConnection::pImpl
{
  Connection* con_;
  MonitorDataHandler* handler_;
  MonitorModeManager::MonitorMode mode_;
  string error_message_;

  long baudrate_;


  pImpl(Connection* connection)
    : con_(connection), handler_(NULL), mode_(MonitorModeManager::Undefined),
      error_message_(con_->what()), baudrate_(0)
  {
  }


  ~pImpl(void)
  {
    delete handler_;
  }


  bool connect(const char* device, long baudrate)
  {
    if (! handler_) {
      string log_name = LogNameHolder::object()->name("mConnection");
      mode_ = MonitorModeManager::object()->mode();
      handler_ = new MonitorDataHandler(log_name, mode_);
    }

    // connect() については、return 情報のみ管理する
    if (mode_ == MonitorModeManager::Record) {
      bool ret = con_->connect(device, baudrate);
      handler_->fetch(ret);
      return ret;

    } else if (mode_ == MonitorModeManager::Play) {
      bool ret = false;
      if (handler_->fetch(ret) < 0) {
        disconnect();
        return false;
      }
      return ret;

    } else if (mode_ == MonitorModeManager::Simulation) {
      // 対応するポートに対して接続を行う
      // !!!
      return false;

    } else {
      return con_->connect(device, baudrate);
    }
  }


  void disconnect(void)
  {
    con_->disconnect();
  }


  void updateErrorMessage(void)
  {
    error_message_ = con_->what();
    // エラーメッセージも記録と再生の対象とする
    if ((mode_ == MonitorModeManager::Record) ||
        (mode_ == MonitorModeManager::Play)) {
      handler_->fetch(error_message_);

    } else if (mode_ == MonitorModeManager::Simulation) {
      // !!!
      error_message_ = "Not implemented.";
    }
  }


  bool setBaudrate(long baudrate)
  {
    // ボーレート値と戻り値を記録する
    if ((mode_ == MonitorModeManager::Record) ||
        (mode_ == MonitorModeManager::Play)) {
      bool ret = false;
      if (mode_ == MonitorModeManager::Record) {
        ret = con_->setBaudrate(baudrate);
      }
      handler_->fetch(ret);
      handler_->fetch(baudrate);
      baudrate_ = baudrate;
      return ret;

    } else if (mode_ == MonitorModeManager::Simulation) {
      // !!!
      return false;

    } else {
      return con_->setBaudrate(baudrate);
    }
  }


  long baudrate(void)
  {
    if (mode_ == MonitorModeManager::Play) {
      return baudrate_;

    } else {
      return con_->baudrate();
    }
  }


  bool isConnected(void)
  {
    // 戻り値のみを記録対象とする
    if ((mode_ == MonitorModeManager::Record) ||
        (mode_ == MonitorModeManager::Play)) {
      bool ret = con_->isConnected();
      handler_->fetch(ret);
      return ret;

    } else if (mode_ == MonitorModeManager::Simulation) {
      // !!!
      return false;

    } else {
      return con_->isConnected();
    }
  }


  int send(const char* data, size_t count)
  {
    if (mode_ == MonitorModeManager::Record) {
      // 記録する必要はないが、デバッグ情報として使うために残しておく
      int send_size = con_->send(data, count);
      return handler_->fetch(const_cast<char*>(data), send_size, "send()");

    } else if (mode_ == MonitorModeManager::Play) {
      string line;
      int send_size = handler_->fetch(static_cast<char*>(NULL), count);
      if (send_size < 0) {
        disconnect();
      }
      return send_size;

    } else if (mode_ == MonitorModeManager::Simulation) {
      // 対応するポートに対して接続を行う
      // !!!
      return false;

    } else {
      return con_->send(data, count);
    }
  }


  int recv(char* data, size_t count, int timeout)
  {
    if (mode_ == MonitorModeManager::Record) {
      int recv_size = con_->receive(data, count, timeout);
      return handler_->fetch(data, recv_size, "recv()");

    } else if (mode_ == MonitorModeManager::Play) {
      int recv_size = handler_->fetch(data, count);
      if (recv_size < 0) {
        disconnect();
      }
      return recv_size;

    } else if (mode_ == MonitorModeManager::Simulation) {
      // 対応するポートに対して接続を行う
      // !!!
      return false;

    } else {
      return con_->receive(data, count, timeout);
    }
  }


  size_t size(void)
  {
    // !!! 記録対象
    return con_->size();

    if ((mode_ == MonitorModeManager::Record) ||
        (mode_ == MonitorModeManager::Play)) {
      int data_size = con_->size();
      return handler_->fetch(data_size);

    } else if (mode_ == MonitorModeManager::Simulation) {
      // 対応するポートに対して接続を行う
      // !!!
      return 0;

    } else {
      return con_->size();
    }
  }
};


mConnection::mConnection(Connection* connection) : pimpl(new pImpl(connection))
{
}


mConnection::~mConnection(void)
{
}


const char* mConnection::what(void) const
{
  pimpl->updateErrorMessage();
  return pimpl->error_message_.c_str();
}


bool mConnection::connect(const char* device, long baudrate)
{
  return pimpl->connect(device, baudrate);
}


void mConnection::disconnect(void)
{
  pimpl->disconnect();
}


bool mConnection::setBaudrate(long baudrate)
{
  return pimpl->setBaudrate(baudrate);
}


long mConnection::baudrate(void) const
{
  return pimpl->baudrate();
}


bool mConnection::isConnected(void) const
{
  return pimpl->isConnected();
}


int mConnection::send(const char* data, size_t count)
{
  return pimpl->send(data, count);
}


int mConnection::receive(char* data, size_t count, int timeout)
{
  return pimpl->recv(data, count, timeout);
}


size_t mConnection::size(void) const
{
  return pimpl->size();
}


void mConnection::flush(void)
{
  pimpl->con_->flush();
}


void mConnection::clear(void)
{
  pimpl->con_->clear();
}


void mConnection::ungetc(const char ch)
{
  pimpl->con_->ungetc(ch);
}
