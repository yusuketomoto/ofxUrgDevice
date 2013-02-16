#ifndef QRK_URG_UTILS_H
#define QRK_URG_UTILS_H

/*!
  \file
  \brief Utility functions of URG

  \author Satofumi KAMIMURA

  $Id: UrgUtils.h 1715 2010-02-21 21:03:13Z satofumi $
*/

#include <cstdlib>


namespace qrk
{
  /*!
    \brief Get serial ID

    \param[in] urg urg object

    \retval >= serial id
    \retval <0 error
  */
  template <class T>
  long urgSerialId(T* urg)
  {
    std::vector<std::string> version_lines;
    if (! urg->versionLines(version_lines)) {
      return -1;
    }
    if (version_lines.size() < 5) {
      return -1;
    }

    std::string& serial_line = version_lines[4];
    if (serial_line.size() < 7) {
      return -1;
    }
    return atol(serial_line.substr(6).c_str());
  }
}

#endif /* !QRK_URG_UTILS_H */
