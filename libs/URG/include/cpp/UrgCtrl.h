#ifndef URG_CTRL_H
#define URG_CTRL_H

/*!
  \file
  \brief URG sensor control

  \author Satofumi KAMIMURA

  $Id: UrgCtrl.h 1840 2010-06-06 10:43:46Z satofumi $
*/

#include "RangeSensor.h"
#include "Coordinate.h"


namespace qrk
{
    class Connection;


    /*!
      \brief URG sensor control

      \obsolute use UrgCtrl
    */
    class UrgCtrl : public Coordinate, public RangeSensor
    {
    public:

        enum {
            DefaultBaudrate = 115200, //!< [bps]
            DefaultRetryTimes = 8,
            Infinity = 0,

            Off = 0,                  //!< Laser is off
            On = 1,                   //!< Laser is on
        };

        UrgCtrl(void);
        virtual ~UrgCtrl(void);

        const char* what(void) const;

        bool connect(const char* device, long baudrate = DefaultBaudrate);
        void setConnection(Connection* con);
        Connection* connection(void);
        void disconnect(void);
        bool isConnected(void) const;

        long minDistance(void) const;
        long maxDistance(void) const;
        int maxScanLines(void) const;


        void setRetryTimes(size_t times);
        void setCapturesSize(size_t size);
        int scanMsec(void) const;
        void setCaptureMode(RangeCaptureMode mode);
        RangeCaptureMode captureMode(void);
        void setCaptureRange(int begin_index, int end_index);
        void setCaptureFrameInterval(size_t interval);
        void setCaptureTimes(size_t times);
        size_t remainCaptureTimes(void);
        void setCaptureSkipLines(size_t skip_lines);
        int capture(std::vector<long>& data, long* timestamp = NULL);
        int captureWithIntensity(std::vector<long>& data,
                                 std::vector<long>& intensity_data,
                                 long* timestamp = NULL);
        void stop(void);
        bool setTimestamp(int timestamp = 0, int* response_msec = NULL,
                          int* force_delay_msec = NULL);
        long recentTimestamp(void) const;
        bool setLaserOutput(bool on);

        double index2rad(const int index) const;
        int rad2index(const double radian) const;
        void setParameter(const RangeSensorParameter& parameter);
        RangeSensorParameter parameter(void) const;
        bool loadParameter(void);
        bool versionLines(std::vector<std::string>& lines);
        bool reboot(void);
        bool reset(void);


    protected:
        virtual void captureReceived(void);


    private:
        UrgCtrl(const UrgCtrl& rhs);
        UrgCtrl& operator = (const UrgCtrl& rhs);

        struct pImpl;
        std::auto_ptr<pImpl> pimpl;
    };
}


#endif /* !URG_CTRL_H */
