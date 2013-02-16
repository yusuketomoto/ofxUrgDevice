/*!
  \file
  \brief シミュレータ接続用の IP ポート管理

  \author Satofumi KAMIMURA

  $Id: DeviceIpManager.cpp 1574 2009-12-13 03:53:55Z satofumi $
*/

#include "DeviceIpManager.h"
#include <string>
#include <map>

using namespace qrk;
using namespace std;


namespace
{
  typedef map<string, long> PortMap;
}


struct DeviceIpManager::pImpl
{
  PortMap port_map_;


  // !!!

  long createIpPort(const char* device, int sub_id)
  {
    static_cast<void>(device);
    static_cast<void>(sub_id);

    // string key = device + lexical_cast<string>(sub_id);

    // !!! 実装案を書き出してから、実装する
    return -1;
  }
};


DeviceIpManager::DeviceIpManager(void) : pimpl(new pImpl)
{
}


DeviceIpManager::~DeviceIpManager(void)
{
}


DeviceIpManager* DeviceIpManager::object(void)
{
  static DeviceIpManager singleton_object;
  return &singleton_object;
}


long DeviceIpManager::createIpPort(const char* device, int sub_id)
{
  return pimpl->createIpPort(device, sub_id);
}
