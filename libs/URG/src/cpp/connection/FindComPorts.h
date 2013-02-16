#ifndef QRK_FIND_COM_PORTS_H
#define QRK_FIND_COM_PORTS_H

/*!
  \file
  \brief COM ポート一覧の取得

  \author Satofumi KAMIMURA

  $Id: FindComPorts.h 1811 2010-04-30 16:12:05Z satofumi $

  \attention 動作確認は Windows XP のみ。他では未検証
*/

#include <string>
#include <vector>
#include <memory>


namespace qrk
{
    /*!
      \brief シリアルポートの探索を行う

      Windows では、レジストリ情報を元に COM 一覧を取得して返す。\n
      Linux, MacOS では、与えられたパターンに一致するファイル名を取得して返す。
    */
    class FindComPorts
    {
    public:
        FindComPorts(void);
        ~FindComPorts(void);


        /*!
          \brief 登録済みのベース名のクリア
        */
        void clearBaseNames(void);


        /*!
          \brief 探索で用いるポートのベース名を追加する

          \param[in] base_name ポートのベース名
        */
        void addBaseName(const char* base_name);


        /*!
          \brief 登録済みのポートのベース名を返す

          Linux 実装で用いる

          \return ポートのベース名
        */
        std::vector<std::string> baseNames(void);


        void addDriverName(const char* driver_name);


        /*!
          \brief COM ポート一覧の取得

          \param[out] ports 検出したポート
          \param[in] all_ports addDriverName() で指定したポートのみ取得するとき false

          \retval COM 一覧が格納された string 配列
        */
        size_t find(std::vector<std::string>& ports, bool all_ports = true);

        size_t find(std::vector<std::string>& ports,
                    std::vector<std::string>& driver_names, bool all_ports = true);

    private:
        FindComPorts(const FindComPorts& rhs);
        FindComPorts& operator = (const FindComPorts& rhs);

        struct pImpl;
        std::auto_ptr<pImpl> pimpl;
    };
}

#endif /* !QRK_FIND_COM_PORTS_H */
