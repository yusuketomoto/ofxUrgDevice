/*!
  \file
  \brief URG É|Å[ÉgÇÃéÊìæ

  \author Satofumi KAMIMURA

  $Id: findUrgPorts.cpp 1433 2009-10-20 15:53:11Z satofumi $
*/

#include "findUrgPorts.h"
#include "FindComPorts.h"
#include "DetectOS.h"
#if defined(WINDOWS_OS)
#include "isUsingComDriver.h"
#endif

using namespace std;


size_t qrk::findUrgPorts(std::vector<std::string>& ports)
{
  FindComPorts com_finder;

#if defined(LINUX_OS)
  com_finder.addBaseName("/dev/ttyACM");
#elif defined(MAC_OS)
  com_finder.addBaseName("/dev/tty.usbmodem");
#elif defined(WINDOWS_OS)
  com_finder.addDriverName("URG Series USB Device Driver");
  com_finder.addDriverName("URG-X002 USB Device Driver");
#endif

  vector<string> com_ports;
  com_finder.find(ports, false);

  return ports.size();
}
