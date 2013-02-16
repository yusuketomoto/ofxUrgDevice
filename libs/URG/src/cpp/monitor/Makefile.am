## Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES = ${CPP_INCLUDES}
lib_LTLIBRARIES = liburg_monitor.la

liburg_monitor_includedir = ${includedir}/urg
liburg_monitor_include_HEADERS = MonitorDataHandler.h MonitorModeManager.h MonitorEventScheduler.h DeviceIpManager.h LogNameHolder.h mConnection.h mUrgDevice.h UrgServer.h DeviceServer.h
liburg_monitor_la_SOURCES = MonitorDataHandler.cpp MonitorModeManager.cpp MonitorEventScheduler.cpp DeviceIpManager.cpp LogNameHolder.cpp mConnection.cpp mUrgDevice.cpp UrgServer.cpp

AM_CXXFLAGS = ${SDL_CFLAGS}
#LIBS += ${BOOST_FILESYSTEM_LIB}
