#ifndef QRK_SCIP_HANDLER_H
#define QRK_SCIP_HANDLER_H

/*!
  \file
  \brief Process SCIP communication

  \author Satofumi KAMIMURA

  $Id: ScipHandler.h 1723 2010-02-24 23:30:34Z satofumi $
*/

#include "CaptureSettings.h"
#include <memory>
#include <vector>
#include <string>


namespace qrk
{
  class Connection;
  class RangeSensorParameter;


  //! Handler to SCIP protocol.
  class ScipHandler
  {
  public:
    static const bool Off = false;
    static const bool On = true;
    static const bool Force = true;

    ScipHandler(void);
    ~ScipHandler(void);

    const char* what(void) const;

    static long decode(const char* data, size_t size);
    static bool checkSum(const char* buffer, int size, char actual_sum);

    void setConnection(Connection* con);
    Connection* connection(void);

    // Match the baudrate and then connect the device.
    bool connect(const char* device, long baudrate);

    int send(const char data[], int size);
    int recv(char data[], int size, int timeout);

    // Read the parameter from URG device.
    bool loadParameter(RangeSensorParameter& parameters);

    bool versionLines(std::vector<std::string>& lines);

    //! Transit to timestamp mode
    bool setRawTimestampMode(bool on);

    bool rawTimestamp(int* timestamp);

    bool setLaserOutput(bool on, bool force = false);

    CaptureType receiveCaptureData(std::vector<long>& data,
                                   CaptureSettings& settings, long* timestamp,
                                   int* remain_times = NULL,
                                   int* total_times = NULL);

  private:
    ScipHandler(const ScipHandler& rhs);
    ScipHandler& operator = (const ScipHandler& rhs);

    struct pImpl;
    std::auto_ptr<pImpl> pimpl;
  };
}

#endif /* !QRK_SCIP_HANDLER_H */
