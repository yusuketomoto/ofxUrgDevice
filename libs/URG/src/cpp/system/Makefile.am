## Makefile.am -- Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = liburg_system.la
liburg_system_includedir = $(includedir)/urg
liburg_system_include_HEADERS = DetectOS.h Thread.h Lock.h LockGuard.h Semaphore.h ticks.h delay.h ConditionVariable.h MathUtils.h log_printf.h
liburg_system_la_SOURCES = MonitorModeManager.cpp SdlInit.cpp Thread.cpp Lock.cpp LockGuard.cpp Semaphore.cpp ticks.cpp delay.cpp system_ticks.cpp system_delay.cpp log_printf.cpp MathUtils.cpp $(BOOST_DEPEND_FILES) \
SdlInit.h system_ticks.h system_delay.h MonitorModeManager.h
AM_CXXFLAGS = $(SDL_CFLAGS)

EXTRA_DIST = $(BOOST_DEPEND_FILES)