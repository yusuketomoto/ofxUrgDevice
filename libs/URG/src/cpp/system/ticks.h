#ifndef QRK_TICKS_H
#define QRK_TICKS_H

/*!
  \file
  \brief タイムスタンプ取得関数

  \author Satofumi KAMIMURA

  $Id: ticks.h 1811 2010-04-30 16:12:05Z satofumi $
*/


//! Quick Robot Development Kit
namespace qrk
{
    /*!
      \brief タイムスタンプの取得

      \retval タイムスタンプ [msec]
    */
    extern long ticks(void);
}

#endif /* !QRK_TICKS_H */
