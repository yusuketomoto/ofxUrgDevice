## Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES = $(CPP_INCLUDES)
lib_LTLIBRARIES = liburg.la
liburg_includedir = $(includedir)/urg
liburg_include_HEADERS = RangeSensor.h CaptureSettings.h RangeSensorParameter.h RangeCaptureMode.h UrgUsbCom.h UrgUtils.h findUrgPorts.h UrgDevice.h UrgCtrl.h
liburg_la_SOURCES = UrgDevice.cpp ScipHandler.cpp findUrgPorts.cpp UrgUsbCom.cpp UrgCtrl.cpp \
ScipHandler.h
AM_CXXFLAGS = $(SDL_CFLAGS)
