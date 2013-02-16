/*!
  \file
  \brief モニタのモード管理

  \author Satofumi KAMIMURA

  $Id: MonitorModeManager.cpp 772 2009-05-05 06:57:57Z satofumi $
*/

#include "MonitorModeManager.h"
#include <cstring>

using namespace qrk;


struct MonitorModeManager::pImpl
{
  MonitorMode mode_;


  pImpl(void) : mode_(Undefined)
  {
  }
};


MonitorModeManager::MonitorModeManager(void) : pimpl(new pImpl)
{
}


MonitorModeManager::~MonitorModeManager(void)
{
}


MonitorModeManager* MonitorModeManager::object(void)
{
  static MonitorModeManager obj;
  return &obj;
}


void MonitorModeManager::setMode(int argc, char* argv[])
{
  if (pimpl->mode_ != Undefined) {
    // モードが決まったら、変更を許さない
    return;
  }

  for (int i = 1; i < argc; ++i) {
    if ((! strcmp("-r", argv[i])) || (! strcmp("--record", argv[i]))) {
      pimpl->mode_ = Record;
      return;

    } else if ((! strcmp("-p", argv[i])) || (! strcmp("--play", argv[i]))) {
      pimpl->mode_ = Play;
      return;

    } else if ((! strcmp("-s", argv[i])) ||
               (! strcmp("--simulation", argv[i]))) {
      pimpl->mode_ = Simulation;
      return;
    }
  }
}


MonitorModeManager::MonitorMode MonitorModeManager::mode(void)
{
  return pimpl->mode_;
}
