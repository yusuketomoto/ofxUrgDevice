//
//  UrgData.cpp
//  example_ofxUrgDevice
//
//  Created by Yusuke on 2013/03/04.
//
//

#include "UrgData.h"

using namespace ofxUrg;

void UrgData::draw(float x, float y) const
{
    if (data.size() != data_angles.size()) {
        return;
    }
    
    ofPushStyle();
    ofPushMatrix();
    ofNoFill();
    ofTranslate(x, y);
    
    float offset = ofDegToRad(sensor_angle + 90);
    for (int i=0; i<data.size(); i++) {
        ofLine(0, 0, data[i]*cos(data_angles[i]+offset), - data[i]*sin(data_angles[i]+offset));
    }
    
    ofPopMatrix();
    ofPopStyle();
    

}

void UrgData::drawShape(float x, float y) const
{
    if (data.size() != data_angles.size()) {
        return;
    }
    
    ofPushStyle();
    ofPushMatrix();
    ofNoFill();
    ofSetLineWidth(2);
    ofTranslate(x, y);
    
    float offset = ofDegToRad(sensor_angle + 90);
    ofPolyline line;
    line.addVertex(0,0);
    for (int i=0; i<data.size(); i++) {
        line.addVertex(data[i]*cos(data_angles[i]+offset), -data[i]*sin(data_angles[i]+offset));
    }
    line.close();
    
    line.draw();
    ofPopMatrix();
    ofPopStyle();
}