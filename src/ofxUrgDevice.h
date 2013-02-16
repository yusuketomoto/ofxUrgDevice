#include <memory>
#include <vector>

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
    void draw(float x, float y, int mode) const;
    void drawDifference(float x, float y) const;
    void connect();
    void disconnect();
    
    void setThresholds(bool bNear, bool bFar, bool bDiff, int near_th, int far_th, int diff_th);
    
    std::vector<std::string> getDevices() const;
    std::vector<long> getData() const;
    long minDistance() const;
    long maxDistance() const;
    int maxScanIndex() const;
    double maxScanAngle() const;
    double index2rad(const int index) const;
    
private:
    struct Impl;
    std::auto_ptr<Impl> pImpl;
};
