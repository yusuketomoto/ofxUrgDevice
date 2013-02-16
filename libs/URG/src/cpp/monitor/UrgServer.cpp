/*!
  \file
  \brief シミュレータ用の URG サーバ

  \author Satofumi KAMIMURA

  $Id: UrgServer.cpp 772 2009-05-05 06:57:57Z satofumi $
*/

#include "UrgServer.h"

using namespace qrk;


struct UrgServer::pImpl
{
  // !!!
};


UrgServer::UrgServer(void) : pimpl(new pImpl)
{
}


UrgServer::~UrgServer(void)
{
}


bool UrgServer::activate(long port)
{
  static_cast<void>(port);

  // !!!
  return false;
}


int UrgServer::updateInterval(void)
{
  return 1;
}


void UrgServer::update(void)
{
}
