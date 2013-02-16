#ifndef QRK_IS_USING_COM_DRIVER_H
#define QRK_IS_USING_COM_DRIVER_H

/*!
  \file
  \brief 指定ドライバが利用中かを判断する

  \author Satofumi KAMIMURA

  $Id: isUsingComDriver.h 1981 2012-04-05 17:22:40Z satofumi $
*/

namespace qrk
{
    /*!
      \brief 指定ドライバを利用している COM ポートが存在するかを返す

      \param[in] com_port 判定を行う COM ポート
      \param[in] driver_name 判定を行うドライバ名

      \retval true 指定ドライバの COM ポートが存在する
      \retval false 存在しない

      \attention Windows 環境でなければ常に false を返す
      \attention 動作確認は、Windows XP のみ。他では未検証
    */
    bool isUsingComDriver(const char* com_port, const char* driver_name);
}

#endif /* !QRK_IS_USING_COM_DRIVER_H */
