//
//  BoundingBox.cpp
//  example_ofxUrgDevice
//
//  Created by Yusuke on 2013/03/03.
//
//

#include "BoundingBox.h"

void BoundingBox::mouseDragged(int x, int y, int button)
{
    bounding_box.width = x - bounding_box.x;
    bounding_box.height = y - bounding_box.y;
}

void BoundingBox::mousePressed(int x, int y, int button)
{
    bounding_box.x = x;
    bounding_box.y = y;
    bounding_box.width = 0;
    bounding_box.height = 0;
}

void BoundingBox::mouseReleased(int x, int y, int button)
{
    bounding_box.width = x - bounding_box.x;
    bounding_box.height = y - bounding_box.y;
}

ofRectangle const& BoundingBox::getBox() const
{
    return bounding_box;
}

void BoundingBox::registerBox()
{
    if (bounding_box.width == 0 || bounding_box.height == 0) {
        return;
    }
    registered_boxes.push_back(bounding_box);
}

void BoundingBox::drawRegisteredBoxes() const
{
    {
        ofPushStyle();
        ofFill();
        for (int i=0; i<registered_boxes.size(); i++) {
            ofSetColor(ofColor::fromHsb(((int) i*60 + 40) % 256, 255, 255, 50));
            ofRect(registered_boxes[i]);
        }
        ofPopStyle();
    }
}

bool isInside(float x, float y, ofRectangle r)
{
    if (r.width < 0) {
        r.x += r.width;
        r.width *= -1;
    }
    if (r.height < 0) {
        r.y += r.height;
        r.height *= -1;
    }
    
    if (r.x > x) return false;
    if (x > r.x+r.width) return false;
    if (r.y > y) return false;
    if (y > r.y+r.height) return false;
    
    return true;
}

set<int> BoundingBox::hitCheck(ofxUrg::UrgData const& data)
{
    set<int> indices;
    float offset = ofDegToRad(data.getSensorAngle() + 90);

    for (int i=0; i<data.size(); i++) {
        float x =  data.getDataRef()[i] * cos(data.getDataAnglesRef()[i] + offset);
        float y = -data.getDataRef()[i] * sin(data.getDataAnglesRef()[i] + offset);
        
        for (int j=0; j<registered_boxes.size(); j++) {
            bool hit = isInside(x, y, registered_boxes[j]);
            
            if (isInside(x, y, registered_boxes[j])) {
                indices.insert(j);
            }
        }
    }
    return indices;
}

void BoundingBox::drawHitRegion(set<int> idx) const
{
    ofPushStyle();
    ofSetLineWidth(3);
    ofNoFill();

    set<int>::iterator it;
    for (it=idx.begin(); it!=idx.end(); ++it) {
        ofSetColor(ofColor::fromHsb(((int) (*it)*60 + 40) % 256, 255, 255));
        ofRect(registered_boxes[*it]);
    }
    
    ofPopStyle();
    
}