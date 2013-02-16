#ifndef QRK_RANGE_CAPTURE_MODE_H
#define QRK_RANGE_CAPTURE_MODE_H

/*!
  \file
  \brief Definition of data acquisition mode

  \author Satofumi KAMIMURA

  $Id: RangeCaptureMode.h 1825 2010-05-15 10:50:09Z satofumi $
*/

namespace qrk
{
    /*!
      \brief The type in which data is acquired.

      There are 2 types of data acquistion. One is "Automatic  acquisition" in which data is transmitted continuously once the command is sent to sensor. Other type is "sequential  acquisition" in which only one set of data is transmitted when command is sent to sensor.

      \image html gd_md_sequence.png "sequential  acquisition(GD/GS) and Automatic  acquisition(MD/MS)

      High intensity data is acquired by "automatic acquisition" type
    */
    typedef enum {
        ManualCapture,              //!< sequential  acquisition
        AutoCapture,                //!< Automatic  acquisition
        IntensityCapture,           //!< High intensity acquisition
    } RangeCaptureMode;
}

#endif /* !QRK_RANGE_CAPTURE_MODE_H */
