/*!
  \file
  \brief 待機関数

  \author Satofumi KAMIMURA

  $Id: delay.cpp 772 2009-05-05 06:57:57Z satofumi $
*/

#include "delay.h"
#include "system_delay.h"
#include "MonitorModeManager.h"


void qrk::delay(int msec)
{
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
    system_delay(msec);

  } else {
    // 再生時、シミュレーション時は、
    // 唯一の StopWatch を用いた時間を返す関数として動作する
    // !!!
    // !!! 仮実装
    system_delay(msec);
  }
}
