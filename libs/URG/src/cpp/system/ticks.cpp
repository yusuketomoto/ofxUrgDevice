/*!
  \file
  \brief タイムスタンプ取得関数

  \author Satofumi KAMIMURA

  $Id: ticks.cpp 1727 2010-02-26 08:56:46Z satofumi $
*/

#include "ticks.h"
#include "system_ticks.h"
#include "MonitorModeManager.h"


long qrk::ticks(void)
{
  // !!! delay() と共通にすべき
  static bool mode_unknown = true;
  static bool is_normal_delay = true;

  if (mode_unknown) {
    MonitorModeManager::MonitorMode mode =
      MonitorModeManager::object()->mode();
    if ((mode == MonitorModeManager::Play) ||
        (mode == MonitorModeManager::Simulation)) {
      is_normal_delay = false;
    }
    mode_unknown = false;
  }

  if (is_normal_delay) {
    return system_ticks();

  } else {
    // 再生時、シミュレーション時は、
    // 唯一の StopWatch を用いた時間を返す関数として動作する
    // !!!
    // !!! 仮実装
    return system_ticks();
  }
}
