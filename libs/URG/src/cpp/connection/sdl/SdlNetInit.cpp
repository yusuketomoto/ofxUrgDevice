/*!
  \file
  \brief SDL_net ‚Ì‰Šú‰»

  \author Satofumi KAMIMURA

  $Id$
*/

#include "SdlNetInit.h"
#include "SdlInit.h"
#include <SDL_net.h>

using namespace qrk;


struct SdlNetInit::pImpl : private SdlInit
{
    static bool initialized;
};
bool SdlNetInit::pImpl::initialized = false;


SdlNetInit::SdlNetInit(void) : pimpl(new pImpl)
{
    if (pimpl->initialized) {
        return;
    }

    if (SDLNet_Init() < 0) {
        return;
    }
    atexit(SDLNet_Quit);
    pimpl->initialized = true;
}


SdlNetInit::~SdlNetInit(void)
{
}
