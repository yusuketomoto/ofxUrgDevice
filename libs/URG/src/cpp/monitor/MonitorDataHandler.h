#ifndef QRK_MONITOR_DATA_HANDLER_H
#define QRK_MONITOR_DATA_HANDLER_H

/*!
  \file
  \brief モニタデータ管理

  \author Satofumi KAMIMURA

  $Id: MonitorDataHandler.h 783 2009-05-05 08:56:26Z satofumi $
*/

#include "MonitorModeManager.h"
#include "DetectOS.h"
#include <string>

#ifdef MSC
#define snprintf _snprintf
#endif


namespace qrk
{

  /*!
    \brief モニタデータ管理クラス

    データフォーマットは、２行から構成される
    - １行目: タイムスタンプ, データバイト数
    - ２行目: データ

    １行目において、# 以降はコメントとみなされる
  */
  class MonitorDataHandler
  {
    MonitorDataHandler(void);
    MonitorDataHandler(const MonitorDataHandler& rhs);
    MonitorDataHandler& operator = (const MonitorDataHandler& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;

  public:
    explicit MonitorDataHandler(std::string file,
                                MonitorModeManager::MonitorMode mode);
    ~MonitorDataHandler(void);

    int fetch(char* data, int size, const char* comment = NULL);

    /*!
      \attention 改行が複数ある文字列データは、扱えない
    */
    int fetch(std::string& data, const char* comment = NULL);
    int fetch(bool& data, const char* comment = NULL);
    int fetch(int& data, const char* comment = NULL);
    int fetch(unsigned int& data, const char* comment = NULL);
    int fetch(short& data, const char* comment = NULL);
    int fetch(long& data, const char* comment = NULL);
  };
}

#endif /* !QRK_MONITOR_DATA_HANDLER_H */
