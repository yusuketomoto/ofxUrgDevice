/*!
  \file
  \brief URG sensor control

  \author Satofumi KAMIMURA

  $Id: UrgCtrl.cpp 1851 2010-07-12 00:42:14Z satofumi $
*/

#include "UrgCtrl.h"
#include "UrgDevice.h"
#include "RangeSensorParameter.h"

using namespace qrk;


struct UrgCtrl::pImpl
{
    UrgDevice urg_;
};


UrgCtrl::UrgCtrl(void) : pimpl(new pImpl)
{
}


UrgCtrl::~UrgCtrl(void)
{
}


void UrgCtrl::captureReceived(void)
{
  // 受信完了を継承先のクラスが利用するためのメソッド
}


const char* UrgCtrl::what(void) const
{
    return pimpl->urg_.what();
}


bool UrgCtrl::connect(const char* device, long baudrate)
{
    return pimpl->urg_.connect(device, baudrate);
}


void UrgCtrl::setConnection(Connection* con)
{
    pimpl->urg_.setConnection(con);
}


Connection* UrgCtrl::connection(void)
{
    return pimpl->urg_.connection();
}


void UrgCtrl::disconnect(void)
{
    pimpl->urg_.disconnect();
}


bool UrgCtrl::isConnected(void) const
{
    return pimpl->urg_.isConnected();
}


long UrgCtrl::minDistance(void) const
{
    return pimpl->urg_.minDistance();
}


long UrgCtrl::maxDistance(void) const
{
    return pimpl->urg_.maxDistance();
}


int UrgCtrl::maxScanLines(void) const
{
    return pimpl->urg_.maxScanLines();
}


void UrgCtrl::setRetryTimes(size_t times)
{
    return pimpl->urg_.setRetryTimes(times);
}


void UrgCtrl::setCapturesSize(size_t size)
{
    pimpl->urg_.setCapturesSize(size);
}


int UrgCtrl::scanMsec(void) const
{
    return pimpl->urg_.scanMsec();
}


void UrgCtrl::setCaptureMode(RangeCaptureMode mode)
{
    pimpl->urg_.setCaptureMode(mode);
}


RangeCaptureMode UrgCtrl::captureMode(void)
{
    return pimpl->urg_.captureMode();
}


void UrgCtrl::setCaptureRange(int begin_index, int end_index)
{
    pimpl->urg_.setCaptureRange(begin_index, end_index);
}


void UrgCtrl::setCaptureFrameInterval(size_t interval)
{
    pimpl->urg_.setCaptureFrameInterval(interval);
}


void UrgCtrl::setCaptureTimes(size_t times)
{
    pimpl->urg_.setCaptureTimes(times);
}


size_t UrgCtrl::remainCaptureTimes(void)
{
    return pimpl->urg_.remainCaptureTimes();
}


void UrgCtrl::setCaptureSkipLines(size_t skip_lines)
{
    pimpl->urg_.setCaptureSkipLines(skip_lines);
}


int UrgCtrl::capture(std::vector<long>& data, long* timestamp)
{
    return pimpl->urg_.capture(data, timestamp);
}


int UrgCtrl::captureWithIntensity(std::vector<long>& data,
                                  std::vector<long>& intensity_data,
                                  long* timestamp)
{
    return pimpl->urg_.captureWithIntensity(data, intensity_data, timestamp);
}


void UrgCtrl::stop(void)
{
    pimpl->urg_.stop();
}


bool UrgCtrl::setTimestamp(int timestamp, int* response_msec,
                           int* force_delay_msec)
{
    return pimpl->urg_.setTimestamp(timestamp, response_msec, force_delay_msec);
}


long UrgCtrl::recentTimestamp(void) const
{
    return pimpl->urg_.recentTimestamp();
}


bool UrgCtrl::setLaserOutput(bool on)
{
    return pimpl->urg_.setLaserOutput(on);
}


double UrgCtrl::index2rad(const int index) const
{
    return pimpl->urg_.index2rad(index);
}


int UrgCtrl::rad2index(const double radian) const
{
    return pimpl->urg_.rad2index(radian);
}


void UrgCtrl::setParameter(const RangeSensorParameter& parameter)
{
    pimpl->urg_.setParameter(parameter);
}


RangeSensorParameter UrgCtrl::parameter(void) const
{
    return pimpl->urg_.parameter();
}


bool UrgCtrl::loadParameter(void)
{
    return pimpl->urg_.loadParameter();
}


bool UrgCtrl::versionLines(std::vector<std::string>& lines)
{
    return pimpl->urg_.versionLines(lines);
}


bool UrgCtrl::reboot(void)
{
    return pimpl->urg_.reboot();
}


bool UrgCtrl::reset(void)
{
    return pimpl->urg_.reboot();
}
