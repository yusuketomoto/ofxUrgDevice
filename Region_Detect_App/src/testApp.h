#pragma once

#include "ofMain.h"
#include "ofxUrgDevice.h"
#include "ofxUI.h"

#include "BoundingBox.h"
#include <set>

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
    
    void guisetup();
    
    ofxUrgDevice urg;
    ofxUICanvas *gui;
    
    BoundingBox box;
    
    set<int> hit_indices;
		
};
