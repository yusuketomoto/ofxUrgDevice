#include <memory>
#include <vector>
#include "UrgData.h"

using std::vector;

class ofxUrgDevice
{
public:
    ofxUrgDevice();
    ~ofxUrgDevice();
    
    void setup();
    void setup(const std::string& device);
    void update();
    void draw(float x, float y) const;
    void drawCoordinate(float x, float y) const;
    void connect();
    void disconnect();
    
    void setSensorAngle(float degree);
    
    std::vector<std::string> getDevices() const;
    ofxUrg::UrgData getData() const;
    long minDistance() const;
    long maxDistance() const;
    int maxScanIndex() const;
    double maxScanAngle() const;
    double index2rad(const int index) const;
    
private:
    struct Impl;
//    std::auto_ptr<Impl> pImpl;
    Impl* pImpl;
    
};
