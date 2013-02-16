#ifndef QRK_MONITOR_MODE_MANAGER_H
#define QRK_MONITOR_MODE_MANAGER_H

/*!
  \file
  \brief モニタのモード管理

  \author Satofumi KAMIMURA

  $Id: MonitorModeManager.h 783 2009-05-05 08:56:26Z satofumi $
*/

#include <memory>


/*!
  \brief モニタのモード管理

  プログラム実行時に渡された引数により、動作モードを決定する。１度決定された動作モードは、変更を許さない。
*/
namespace qrk
{
  class MonitorModeManager
  {
    MonitorModeManager(void);
    MonitorModeManager(const MonitorModeManager& rhs);
    MonitorModeManager& operator = (const MonitorModeManager& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;

  public:

    // !!! 独立のヘッダファイルとして管理するべき
    typedef enum {
      Undefined,
      Record,
      Play,
      Simulation,
    } MonitorMode;

    ~MonitorModeManager(void);
    static MonitorModeManager* object(void);

    void setMode(int argc, char* argv[]);
    MonitorMode mode(void);
  };
}

#endif /* !QRK_MONITOR_MODE_MANAGER_H */
