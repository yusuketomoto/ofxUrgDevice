#ifndef QRK_MONITOR_EVENT_SCHEDULER_H
#define QRK_MONITOR_EVENT_SCHEDULER_H

/*!
  \file
  \brief モニタイベント管理

  \author Satofumi KAMIMURA

  $Id: MonitorEventScheduler.h 783 2009-05-05 08:56:26Z satofumi $
*/

#include <memory>


namespace qrk
{
  class ConditionVariable;
  class DeviceServer;
  class Lock;


  /*!
    \brief モニタイベント管理
  */
  class MonitorEventScheduler
  {
    MonitorEventScheduler(void);
    MonitorEventScheduler(const MonitorEventScheduler& rhs);
    MonitorEventScheduler& operator = (const MonitorEventScheduler& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;

  public:
    ~MonitorEventScheduler(void);
    static MonitorEventScheduler* object(void);

    void terminate(void);

    void registerWakeupTicks(ConditionVariable* condition, int ticks);
    void registerDeviceServer(DeviceServer* device);
  };
}

#endif /* !QRK_MONITOR_EVENT_SCHEDULER_H */
