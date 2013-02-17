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
UrgDevice urg;

template <class _Tp>
struct abs_minus : public binary_function<_Tp, _Tp, _Tp>
{
    _Tp
    operator()(const _Tp& __x, const _Tp& __y) const
    { return (__x > __y) ? (__x - __y)  : (__y - __x); }
};

struct ofxUrgDevice::Impl
{
private:
    FindComPorts com_finder;
    UrgUsbCom urg_usb;
//    UrgDevice urg; // クラスの中で宣言するとexit時にアプリが固まる。とりあえずグローバル変数にしておく。デストラクタが悪さしてるみたいだが。。
    
    Lock urg_mutex;
    
    vector<string> devices;
    string device;
    vector<long> data;
    vector<long> previous_data;
    vector<long> diff_data;
    long timestamp;
    
    bool bNearThresh;
    bool bFarThresh;
    bool bDiffThresh;
    long near_thresh;
    long far_thresh;
    long diff_thresh;
    
    vector<int> filtered_index;
    
    enum {
        URG_DRAW_NORMAL = 0,
        URG_DRAW_DIFFERENCE,
        URG_DRAW_DIFFERENCE_ONLY
    } draw_modes;
    float draw_scale;
    
public:
    Impl()
    :bNearThresh(false), bFarThresh(false), bDiffThresh(false)
    ,near_thresh(0), far_thresh(4000), diff_thresh(4000)
    ,draw_scale(0.1)
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
        std::swap(previous_data, data); // 差分を求めるために過去フレームを保存しておく
        
        urg.capture(data, &timestamp);
        
        // 差分を求める（前フレームとの差分が極端に大きいデータをノイズとみなしてカットしてもよい）
        previous_data.resize(data.size());
        diff_data.resize(data.size());
        std::transform(data.begin(), data.end(), previous_data.begin(), diff_data.begin(), abs_minus<long>());

        // 閾値の条件を満たすインデックスのみ抽出する
        filtered_index.resize(data.size());
        for (int i=0; i<data.size(); i++) {
            filtered_index[i] = i;
        }
        vector<int>::iterator it = filtered_index.begin();
        for (int i=0; it!=filtered_index.end(); ++it, ++i) {
            if ( (bNearThresh && data[*it]<near_thresh)
                || (bFarThresh && data[*it]>far_thresh)
                || (bDiffThresh && diff_data[*it] >= diff_thresh)) {
                filtered_index.erase(it);
                --it;
            }
        }
        
    }
    
    void draw(float x, float y) const
    {
        ofNoFill();
        ofPushMatrix();
        ofTranslate(x, y);
        drawRipples(0, 0);
        
        float offset = ofDegToRad(-90);
        for (vector<int>::const_iterator it = filtered_index.begin(); it!=filtered_index.end(); ++it) {
            const int index = *it;
            float f = data[index] * draw_scale;
            ofLine(0, 0, f*cos(urg.index2rad(index)+offset), f*sin(urg.index2rad(index)+offset));
        }
        ofPopMatrix();
    }
    
    void draw(float x, float y, int mode) const
    {
        ofPushMatrix();
        ofTranslate(x, y);
        ofNoFill();
        switch (mode) {
            case URG_DRAW_NORMAL:
                ofSetColor(255, 255, 255, 100);
                draw(0,0);
                break;
            case URG_DRAW_DIFFERENCE:
                ofSetColor(255, 255, 255, 100);
                draw(0,0);
                ofSetColor(255, 0, 0, 100);
                drawDifference(0, 0);
                break;
            case URG_DRAW_DIFFERENCE_ONLY:
                ofSetColor(255, 0, 0, 100);
                drawDifference(0, 0);
                break;
        }
        if (bNearThresh) {
            ofSetColor(0, 255, 0, 100);
            drawNearThresh(0, 0);
        }
        if (bFarThresh) {
            ofSetColor(0, 0, 255, 100);
            drawFarThresh(0, 0);
        }
        ofPopMatrix();
    }
    
    void drawDifference(float x, float y) const
    {
        ofNoFill();
        ofPushMatrix();
        ofTranslate(x, y);
        if (data.size() == previous_data.size()) {
            float offset = ofDegToRad(-90);
            for (vector<int>::const_iterator it = filtered_index.begin(); it!=filtered_index.end(); ++it) {
                const int index = *it;
                float f1 = data[index] * draw_scale;
                float f2 = previous_data[index] * draw_scale;
                ofLine(f1*cos(urg.index2rad(index)+offset), f1*sin(urg.index2rad(index)+offset),
                       f2*cos(urg.index2rad(index)+offset), f2*sin(urg.index2rad(index)+offset));
            }
        }
        ofPopMatrix();
    }
    
    void drawNearThresh(float x, float y) const
    {
        ofFill();
        ofPushMatrix();
        ofTranslate(x, y);
        ofCircle(0, 0, near_thresh*draw_scale);
        ofPopMatrix();
    }
    
    void drawFarThresh(float x, float y) const
    {
        ofNoFill();
        ofPushMatrix();
        ofTranslate(x, y);
        ofCircle(0, 0, far_thresh*draw_scale);
        ofPopMatrix();
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
    
    void setThresholds(bool bNear, bool bFar, bool bDiff, int near_th, int far_th, int diff_th)
    {
        bNearThresh = bNear;
        bFarThresh  = bFar;
        bDiffThresh = bDiff;
        near_thresh = near_th;
        far_thresh = far_th;
        diff_thresh = diff_th;
    }

    inline vector<string> getDevices() const { return devices; }
    inline vector<long> getData() const { return data; }
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
    
private:
    void drawRipples(float x, float y) const
    {
        ofNoFill();
        int n = 9;
        while (n--) {
            ofCircle(x, y, 50*n);
        }
    }
};

// ------------- 以下、委譲の嵐 ---------------
ofxUrgDevice::ofxUrgDevice() : pImpl(new Impl)
{}

ofxUrgDevice::~ofxUrgDevice()
{}

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

void ofxUrgDevice::draw(float x, float y, int mode) const
{
    pImpl->draw(x, y, mode);
}

void ofxUrgDevice::drawDifference(float x, float y) const
{
    pImpl->drawDifference(x, y);
}

void ofxUrgDevice::connect()
{
    pImpl->connect();
}

void ofxUrgDevice::disconnect()
{
    pImpl->disconnect();
}

void ofxUrgDevice::setThresholds(bool bNear, bool bFar, bool bDiff, int near_th, int far_th, int diff_th)
{
    pImpl->setThresholds(bNear, bFar, bDiff, near_th, far_th, diff_th);
}

vector<std::string> ofxUrgDevice::getDevices() const
{
    return pImpl->getDevices();
}

vector<long> ofxUrgDevice::getData() const
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