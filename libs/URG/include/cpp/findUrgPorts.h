#ifndef QRK_FIND_URG_PORTS_H
#define QRK_FIND_URG_PORTS_H

/*!
  \file
  \brief URG ポートの取得

  \author Satofumi KAMIMURA

  $Id: findUrgPorts.h 1386 2009-10-09 08:00:11Z satofumi $
*/

#include <vector>
#include <string>


namespace qrk
{
  /*!
    \brief URG ポートの取得

    Example
    \code
    !!!
    !!! findUrgPorts()
    FindComPorts com_finder;
    !!! unique() 相当のなにか
    !!! 記述すること
    \endcode
  */
  size_t findUrgPorts(std::vector<std::string>& ports);
};

#endif /* !QRK_FIND_URG_PORTS_H */
