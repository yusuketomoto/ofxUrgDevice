//
//  UrgData.h
//  example_ofxUrgDevice
//
//  Created by Yusuke on 2013/03/04.
//
//

#ifndef __example_ofxUrgDevice__UrgData__
#define __example_ofxUrgDevice__UrgData__

#include "ofMain.h"

namespace ofxUrg {

    class UrgData
    {
    public:
        UrgData() {}
        UrgData(vector<long> const& _data, vector<float> const& _angles)
        :data(_data), data_angles(_angles)
        {}
        
        UrgData(vector<long> const& _data, vector<float> const& _angles, float _sensor_angle)
        :data(_data), data_angles(_angles), sensor_angle(_sensor_angle)
        {}
        
        int size() const { return data.size()==data_angles.size() ? data.size() : -1; }
        void resize(int size)
        {
            data.resize(size);
            data_angles.resize(size);
        }
        
        void clear() { data.clear(); data_angles.clear(); }
        void setData(vector<long> const& _data) { data = _data; }
        void setDataAngles(vector<float> const& _angles) { data_angles = _angles; }
        void setSensorAngle(float angle) { sensor_angle = angle; }
        
        void addData(long _data) { data.push_back(_data); }
        void addAngle(float _angle) { data_angles.push_back(_angle); }
        
        vector<long>& getDataRef() { return data; }
        vector<long> const& getDataRef() const { return data; }
        vector<float>& getDataAnglesRef() { return data_angles; }
        vector<float> const& getDataAnglesRef() const { return data_angles; }
        float getSensorAngle() const { return sensor_angle; }
        
        void draw(float x, float y) const;
        void drawShape(float x, float y) const;
        
    private:
        vector<long> data;
        vector<float> data_angles;
        float sensor_angle;
    };

}


#endif /* defined(__example_ofxUrgDevice__UrgData__) */
