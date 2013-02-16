/*!
  \file
  \brief SDL システムの初期化

  \author Satofumi KAMIMURA

  $Id: SdlInit.cpp 772 2009-05-05 06:57:57Z satofumi $
*/

#include <cstdlib>
#include "SdlInit.h"
#include <SDL.h>

using namespace qrk;


struct SdlInit::pImpl
{
  static bool initialized_;
};
bool SdlInit::pImpl::initialized_ = false;


SdlInit::SdlInit(void) : pimpl(new pImpl)
{
  if (pimpl->initialized_) {
    // 初期化済みならば、戻る
    return;
  }

  if (SDL_Init(0) < 0) {
    // !!! 例外を投げるべき
    return;
  }
  atexit(SDL_Quit);
  pimpl->initialized_ = true;
}


SdlInit::~SdlInit(void)
{
}
