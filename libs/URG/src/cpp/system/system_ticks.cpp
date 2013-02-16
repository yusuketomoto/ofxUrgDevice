/*!
  \file
  \brief タイムスタンプ取得関数

  \author Satofumi KAMIMURA

  $Id: system_ticks.cpp 1728 2010-02-26 21:14:05Z satofumi $

  \todo 再生速度の変更に対処すること
*/

#include "SdlInit.h"
#include "system_ticks.h"
#include <SDL.h>

namespace
{
  class TicksInit : private qrk::SdlInit
  {
  };
}


long qrk::system_ticks(void)
{
  // 初期化用
  static TicksInit sdl_init;

  return SDL_GetTicks();
}
