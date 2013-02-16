#ifndef QRK_CONNECTION_UTILS_H
#define QRK_CONNECTION_UTILS_H

/*!
  \file
  \brief Connection を用いた補助関数

  \author Satofumi KAMIMURA

  $Id: ConnectionUtils.h 1838 2010-06-01 03:17:17Z satofumi $
*/

#include <cstddef>
#include <algorithm>


//! Quick Robot Develoment Kit.
namespace qrk
{
    class Connection;


    /*!
      \brief 改行コードかを返す

      \retval true LF, CR のとき
      \retval false 上記以外のとき
    */
    extern bool isLF(const char ch);


    /*!
      \brief 受信データを読み飛ばす

      Connection::clear() とは、タイムアウト時間を指定して読み飛ばせる点が異なる

      \param[in,out] con 通信リソース
      \param[in] total_timeout タイムアウト時間の上限 [msec]
      \param[in] each_timeout 受信データ間におけるタイムアウト時間の上限 [msec]
    */
    extern void skip(Connection* con, int total_timeout, int each_timeout = 0);


    /*!
      \brief 改行までのデータ読み出し

      文字列終端に \\0 を付加して返す

      \param[in,out] con 通信リソース
      \param[out] buf 受信バッファ
      \param[in] count 受信バッファの最大サイズ
      \param[in] timeout タイムアウト [msec]

      \return 受信文字数 (受信がなくてタイムアウトした場合は -1)
    */
    extern int readline(Connection* con, char* buf, const size_t count,
                        int timeout);


    /*!
      \brief 接続オブジェクトの交換

      a と b の接続オブジェクトを交換する。
    */
    template <class T>
    void swapConnection(T& a, T& b)
    {
        Connection* t = a.connection();
        a.setConnection(b.connection());
        b.setConnection(t);
    }
}

#endif /* !QRK_CONNECTION_UTILS_H */
