#ifndef QRK_URG_DEVICE_H
#define QRK_URG_DEVICE_H

/*!
  \file
  \brief URG sensor control

  \author Satofumi KAMIMURA

  $Id: UrgDevice.h 1846 2010-06-13 23:26:56Z satofumi $
*/

#include "RangeSensor.h"
#include "Coordinate.h"


namespace qrk
{
    class Connection;


    //! URG sensor control
    class UrgDevice : public Coordinate, public RangeSensor
    {
    public:

        //! Parameter of options
        enum {
            DefaultBaudrate = 115200, //!< [bps]
            DefaultRetryTimes = 8,
            Infinity = 0,

            Off = 0,                  //!< Laser is off
            On = 1,                   //!< Laser is on
        };

        UrgDevice(void);
        virtual ~UrgDevice(void);

        const char* what(void) const;

        bool connect(const char* device, long baudrate = DefaultBaudrate);
        void setConnection(Connection* con);
        Connection* connection(void);
        void disconnect(void);
        bool isConnected(void) const;

        long minDistance(void) const;
        long maxDistance(void) const;
        int maxScanLines(void) const;


        /*!
          \brief Set number of retry times when connection failed

          The retry counter is cleared when connect normaly.

          \param[in] times retry times

          \attention Data acquisition mode is valid only at #AutoCapture
        */
        void setRetryTimes(size_t times);


        /*!
          \brief Set number of scan data stored internally

          \param[in] size Number of scanning data to be stored
        */
        void setCapturesSize(size_t size);


        /*!
          \brief Time taken for 1scan.

          \return Time taken for 1scan.[msec]
        */
        int scanMsec(void) const;


        /*!
          \brief Specification of data acquisition mode

          \param[in] mode The mode defined by #RangeCaptureMode can be specified

          \see captureMode()
        */
        void setCaptureMode(RangeCaptureMode mode);


        /*!
          \brief Data acquisition mode

          \return Present data acquisition mode

          \see setCaptureMode()
        */
        RangeCaptureMode captureMode(void);


        /*!
          \brief Specifies the range of data acquisition

          \ref Refer to scip_capture_parameter_section.

          \param[in] begin_index Measurement beginning position
          \param[in] end_index Measurement end position
        */
        void setCaptureRange(int begin_index, int end_index);


        /*!
          \brief Acquisition interval of scanning data

          The data traffic between URG and the library can be decreased by lowering the frequency of data acquisition.
          \ref refer to scip_capture_parameter_section

          \param[in] interval capture interval
        */
        void setCaptureFrameInterval(size_t interval);


        /*!
          \brief Specify the frequency of data acquisition

          \ref Refer to scip_capture_parameter_section

          \param[in] times Frequency of data acquisition

          \attention Specify #UrgInfinityTimes to acquire data infinity times.
          \attention Data acquisition mode is valid only at #AutoCapture, #IntensityCapture.
        */
        void setCaptureTimes(size_t times);


        /*!
          \brief Get number of remaining capture times

          \return number of remaining capture times

          \attention Data acquisition mode is valid only at #AutoCapture, #IntensityCapture
        */
        size_t remainCaptureTimes(void);


        /*!
          \brief Specify the number of scanline to be skipped.

          The number of thinning out of acquisition data in one scanning is specified.

          \param[in] skip_lines number of scanline to be skipped.

          For example, if number of scan lines to be skipped = 1 , then will be
          \verbatim
          100, 101, 103, 104, 105, 100, ... \endverbatim

          In case, number of scanlines to be skipped = 2, then acquired data will be
          \verbatim
          100, 103, 100, ... \endverbatim

          (The shortest distance data is returned. )\n
          As a result, the data traffic between URG and the library can be decreased.
          However, the volume of data that capture() returns doesnot change.
          Returns \verbatim
          100, 100, 103, 103, 100, 100, ... \endverbatim

          To guarantee the operation of rad2index() and index2rad(), volume of data does not changing :.
        */
        void setCaptureSkipLines(size_t skip_lines);


        int capture(std::vector<long>& data, long* timestamp = NULL);


        /*
          \brief Get intensity data and distance data.

          \attention Only Top-URG and Classic-URG supports this fuction (2008-12-24)
        */
        int captureWithIntensity(std::vector<long>& data,
                                 std::vector<long>& intensity_data,
                                 long* timestamp = NULL);


        /*!
          \brief Stop data acquisition

          \attention Data acquisition mode is valid only at #AutoCapture, #IntensityCapture
        */
        void stop(void);


        bool setTimestamp(int timestamp = 0, int* response_msec = NULL,
                          int* force_delay_msec = NULL);

        long recentTimestamp(void) const;


        /*!
          \brief Laser turn Off/ turn On

          \param[in] on true to turn On , false to turn off
        */
        bool setLaserOutput(bool on);

        double index2rad(const int index) const;
        int rad2index(const double radian) const;

        void setParameter(const RangeSensorParameter& parameter);
        RangeSensorParameter parameter(void) const;


        /*!
          \brief Again read the URG parameter

          \retval true success
          \retval false error
        */
        bool loadParameter(void);


        /*!
          \brief Get version information

          \param[out] lines Storage location of version information

          \retval true success
          \retval false error

          <b>Example</b>
          - \ref viewVvSample.cpp
        */
        bool versionLines(std::vector<std::string>& lines);


        /*!
          \brief Reboot URG

          \attention Only Top-URG (2010-02-04)
        */
        bool reboot(void);


    protected:
        virtual void captureReceived(void);


    private:
        UrgDevice(const UrgDevice& rhs);
        UrgDevice& operator = (const UrgDevice& rhs);

        struct pImpl;
        std::auto_ptr<pImpl> pimpl;
    };
}

#endif /* !QRK_URG_DEVICE_H */
