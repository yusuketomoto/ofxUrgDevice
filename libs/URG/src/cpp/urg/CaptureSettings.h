#ifndef QRK_CAPTURE_SETTINGS_H
#define QRK_CAPTURE_SETTINGS_H

/*!
  \file
  \brief Data acquisition setting

  \author Satofumi KAMIMURA

  $Id: CaptureSettings.h 1715 2010-02-21 21:03:13Z satofumi $
*/

/*!
  \brief Quick Robot Development Kit.
*/
namespace qrk
{
  /*!
    \brief Receive data type
  */
  typedef enum {
    TypeUnknown,                //!< Unknown
    QT,                         //!< Quit to receive
    GD,                         //!< Get Data (each data size: 3 byte)
    GS,                         //!< Get Short data (each data size: 2 byte)
    MD,                         //!< Measure Data (each data size: 3 byte)
    MS,                         //!< Measure Data (each data size: 2 byte)
    ME,                         //!< Intensity data
    Mx_Reply,                   //!< The first response message of Mx command
    InvalidData,                //!< Error response
  } CaptureType;


  /*!
    \brief Data acquisition setting
  */
  class CaptureSettings
  {
  public:
    CaptureType type;           //!< Type of receive data
    int error_code;             //!< Error code
    long timestamp;             //!< [msec]
    int capture_first;          //!< Acquisition beginning index
    int capture_last;           //!< Acquisition end index
    int skip_lines;             //!< Number of lines to skip
    int skip_frames;            //!< Data acquisition interval
    int remain_times;           //!< Remaining number of  scans
    int data_byte;              //!< Number of data bytes


    CaptureSettings(void)
      : type(TypeUnknown), error_code(-1), timestamp(-1),
        capture_first(-1), capture_last(-1),
        skip_lines(-1), skip_frames(-1), remain_times(-1), data_byte(-1)
    {
    }
  };
}

#endif /* !QRK_CAPTURE_SETTINGS_H */
