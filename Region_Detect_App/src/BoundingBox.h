//
//  BoundingBox.h
//  example_ofxUrgDevice
//
//  Created by Yusuke on 2013/03/03.
//
//

#ifndef __example_ofxUrgDevice__BoundingBox__
#define __example_ofxUrgDevice__BoundingBox__

#include "ofMain.h"
#include "UrgData.h"
#include <set>

class BoundingBox
{
public:
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    ofRectangle const& getBox() const;
    void registerBox();

    inline void clear() { registered_boxes.clear(); }
    void drawRegisteredBoxes() const;
    void drawHitRegion(set<int> idx) const;
    
    set<int> hitCheck(ofxUrg::UrgData const& data);
    
private:
    ofRectangle bounding_box;
    vector<ofRectangle> registered_boxes;
};

#endif /* defined(__example_ofxUrgDevice__BoundingBox__) */
