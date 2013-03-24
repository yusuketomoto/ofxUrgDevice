#include "ofxUrgDevice.h"

#include "mUrgDevice.h"
#include "RangeSensorParameter.h"
#include "SerialDevice.h"
#include "TcpipSocket.h"
#include "FindComPorts.h"
#include "UrgUsbCom.h"
#include "ticks.h"
#include "MonitorModeManager.h"
#include "MonitorEventScheduler.h"
#include "Lock.h"
#include "LockGuard.h"
#include "log_printf.h"
#include "split.h"

#include "ofMain.h"

using namespace qrk;


template <class _Tp>
struct abs_minus : public binary_function<_Tp, _Tp, _Tp>
{
    _Tp
    operator()(const _Tp& __x, const _Tp& __y) const
    { return (__x > __y) ? (__x - __y)  : (__y - __x); }
};

inline float modf( float X, float Y )
{
    return ( X - int( X / Y ) * Y );
}

struct ofxUrgDevice::Impl
{
private:
    FindComPorts com_finder;
    UrgUsbCom urg_usb;
    UrgDevice urg;
    
    Lock urg_mutex;
    
    vector<string> devices;
    string device;
    vector<long> data;
    long timestamp;
    
    ofxUrg::UrgData urg_data;
    
    bool bNearThresh;
    bool bFarThresh;
    long near_thresh;
    long far_thresh;
    
//    float sensor_angle;
    
    vector<int> filtered_index;
    
public:
    Impl()
    :bNearThresh(false), bFarThresh(false)
    ,near_thresh(0), far_thresh(4000)
//    ,sensor_angle(0.0)
    {
        devices = findCom();
    }
        
    void setup()
    {
        if (devices.size()>0) {
            device = devices[0];
        } else {
            cout << "not found device" << endl;
        }
        connect();
    }
    
    void setup(const string& device_)
    {
        device = device_;
        connect();
    }
    
    void update()
    {
        qrk::LockGuard guard(urg_mutex);
        urg.capture(data, &timestamp);
        
        vector<float> angles;
        for (int i=0; i<data.size(); i++) {
            angles.push_back( urg.index2rad(i));
        }
        
        urg_data.setData(data);
        urg_data.setDataAngles(angles);
        
    }
    
    void draw(float x, float y) const
    {
        ofPushStyle();
        ofNoFill();
        urg_data.draw(x, y);
        ofPopStyle();
    }
    
    void connect()
    {
        qrk::LockGuard guard(urg_mutex);
        
        if (! urg.connect(device.c_str())) {
            printf("UrgDevice::connect: %s\n", urg.what());
            exit(1);
        }
    }
    
    void disconnect()
    {
        urg.disconnect();
    }
    
    inline vector<string> getDevices() const { return devices; }
    inline ofxUrg::UrgData getData() const { return urg_data; }
    inline long minDistance() const { return urg.minDistance(); }
    inline long maxDistance() const { return urg.maxDistance(); }
    inline int maxScanIndex() const { return urg.maxScanLines(); }
    inline double maxScanAngle() const { return urg.index2rad(urg.maxScanLines()); }

    inline double index2rad(const int index) const { return urg.index2rad(index); }

    vector<string> findCom()
    {
        com_finder.addBaseName("/dev/ttyACM");
        com_finder.addBaseName("/dev/tty.usbmodem");
        
        com_finder.addDriverName("URG Series USB Device Driver");
        com_finder.addDriverName("URG-X002 USB Device Driver");
        
        vector<string> devices;
        com_finder.find(devices);
        return devices;
    }
    
    void setSensorAngle(float degree)
    {
        urg_data.setSensorAngle(degree);
    }
    
    void drawCoordinate(float x, float y) const
    {
        ofPushStyle();
        ofNoFill();
        ofSetCircleResolution(64);
        int n = 9;
        while (n--) {
            ofCircle(x, y, 500*n);
        }
        ofPopStyle();
    }
};

// ------------- 以下、委譲の嵐 ---------------
ofxUrgDevice::ofxUrgDevice() : pImpl(new Impl)
{}

ofxUrgDevice::~ofxUrgDevice()
{
}

void ofxUrgDevice::setup()
{
    pImpl->setup();
}

void ofxUrgDevice::setup(const string& device)
{
    pImpl->setup(device);
}

void ofxUrgDevice::update()
{
    pImpl->update();
}

void ofxUrgDevice::draw(float x, float y) const
{
    pImpl->draw(x, y);
}

void ofxUrgDevice::drawCoordinate(float x, float y) const
{
    pImpl->drawCoordinate(x, y);
}

void ofxUrgDevice::connect()
{
    pImpl->connect();
}

void ofxUrgDevice::disconnect()
{
    pImpl->disconnect();
}

void ofxUrgDevice::setSensorAngle(float degree)
{
    pImpl->setSensorAngle(degree);
}

vector<std::string> ofxUrgDevice::getDevices() const
{
    return pImpl->getDevices();
}

ofxUrg::UrgData ofxUrgDevice::getData() const
{
    return pImpl->getData();
}

long ofxUrgDevice::minDistance() const
{
    return pImpl->minDistance();
}

long ofxUrgDevice::maxDistance() const
{
    return pImpl->maxDistance();
}

int ofxUrgDevice::maxScanIndex() const
{
    return pImpl->maxScanIndex();
}

double ofxUrgDevice::maxScanAngle() const
{
    return pImpl->maxScanAngle();
}

double ofxUrgDevice::index2rad(const int index) const
{
    return pImpl->index2rad(index);
}