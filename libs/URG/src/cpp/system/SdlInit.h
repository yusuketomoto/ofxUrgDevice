#ifndef QRK_SDL_INIT_H
#define QRK_SDL_INIT_H

/*!
  \file
  \brief SDL システムの初期化

  \author Satofumi KAMIMURA

  $Id: SdlInit.h 783 2009-05-05 08:56:26Z satofumi $
*/

#include <memory>


namespace qrk
{
  /*!
    \brief SDL システムの初期化クラス
  */
  class SdlInit
  {
    SdlInit(const SdlInit& rhs);
    SdlInit& operator = (const SdlInit& rhs);

    struct pImpl;
    const std::auto_ptr<pImpl> pimpl;

  protected:
    SdlInit(void);
    ~SdlInit(void);
  };
}

#endif /* !QRK_SDL_INIT_H */
