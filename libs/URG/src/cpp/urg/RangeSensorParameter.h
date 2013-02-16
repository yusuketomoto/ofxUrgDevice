#ifndef QRK_RANGE_SENSOR_PARAMETER_H
#define QRK_RANGE_SENSOR_PARAMETER_H

/*!
  \file
  \brief Manage range sensor's parameter

  \author Satofumi KAMIMURA

  $Id: RangeSensorParameter.h 1684 2010-02-10 23:56:38Z satofumi $
*/

#include <string>


namespace qrk
{
  //! class to manage range sensor's parameter
  class RangeSensorParameter
  {
  public:
    enum {
      MODL = 0,           //!< Sensor model information
      DMIN,               //!< Least possible measurement range [mm]
      DMAX,               //!< Maximum possible measurement range [mm]
      ARES, //!< Angular resolution(Number of partitions in 360 degree)
      AMIN, //!< Least possible measurement direction in terms of angle
      AMAX, //!< Maximum possible measurement direction in terms of angle
      AFRT, //!< Front direction index
      SCAN, //!< Standard angular velocity [rpm]
    };

    std::string model;          //!< MODL
    long distance_min;          //!< DMIN
    long distance_max;          //!< DMAX
    int area_total;             //!< ARES
    int area_min;               //!< AMIN
    int area_max;               //!< AMAX
    int area_front;             //!< AFRT
    int scan_rpm;               //!< SCAN

    RangeSensorParameter(void)
      : model(""), distance_min(0), distance_max(0), area_total(0),
        area_min(0), area_max(0), area_front(0), scan_rpm(600)
    {
    }
  };
}

#endif /* !QRK_RANGE_SENSOR_PARAMETER_H */
