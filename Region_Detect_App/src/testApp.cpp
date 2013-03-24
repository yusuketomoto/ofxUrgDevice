#include "testApp.h"

float scale = 0.12;
ofPoint shift = ofPoint(0,0);
float sensor_angle = 180;

bool register_region = false;
bool clear_region = false;

bool draw_data = true;

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(100);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
    urg.setup();
    
    guisetup();
}

//--------------------------------------------------------------
void testApp::update(){
    urg.setSensorAngle(sensor_angle);
    urg.update();
    
    if (register_region) {
        box.registerBox();
        register_region = false;
    }
    
    if (clear_region) {
        box.clear();
        clear_region = false;
    }
    
    hit_indices = box.hitCheck(urg.getData());

}

//--------------------------------------------------------------
void testApp::draw(){
    // Draw Sensor Output
    ofPushStyle();
    ofPushMatrix();
        ofTranslate(ofGetWindowWidth() *0.5*(1+shift.x),
                    ofGetWindowHeight()*0.5*(1+shift.y));
        ofScale(scale, scale);
    
    ofSetColor(255, 255, 255, 100);
    urg.drawCoordinate(0, 0);
    
    if (draw_data) {
        urg.draw(0, 0);
    }
    
    // Hit Check Result
    box.drawHitRegion(hit_indices);

        ofNoFill();
        ofSetLineWidth(2);
        ofSetHexColor(0xffee00);
        ofRect(box.getBox());
        box.drawRegisteredBoxes();
    ofPopMatrix();
    ofPopStyle();
    
    // Mouse Position
    ofPushStyle();
    ofSetHexColor(0x00abec);
    ofDrawBitmapString(ofToString(ofVec2f( (mouseX-ofGetWindowWidth() *0.5*(1+shift.x))/scale,
                                          -(mouseY-ofGetWindowHeight()*0.5*(1+shift.y))/scale)),
                       mouseX, mouseY);
    ofPopStyle();
    
    
}

void testApp::guisetup()
{
    float dim = 20;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
	float length = 200 - xInit;
	gui = new ofxUICanvas(0, 0, length + xInit * 2, 2560);
	gui->setFont("/System/Library/Fonts/Geneva.dfont");
	ofColor cb(64, 192),
	co(192, 192),
	coh(128, 192),
	cf(240, 255),
	cfh(128, 255),
	cp(96, 192),
	cpo(255, 192);
	gui->setUIColors(cb, co, coh, cf, cfh, cp, cpo);
	   
    gui->addLabel("Sensor", OFX_UI_FONT_LARGE);
    gui->addSlider("Zoom", 0.01, 1.0, &scale, length, dim);
    gui->add2DPad("Shift", ofPoint(-1.0,1.0), ofPoint(-1.0,1.0), &shift, length, length);
    gui->addSlider("Sensor Angle", 0, 360, &sensor_angle, length, dim);

    gui->addLabel("Region", OFX_UI_FONT_LARGE);
	gui->addLabelButton("Register", &register_region, length, dim);
    gui->addLabelButton("Clear", &clear_region, length, dim);
    
    gui->addLabel("Visualization");
    gui->addLabelToggle("Data", &draw_data, length, dim);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (key == ' ') {
        register_region = true;
    }
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    if (!gui->isHit(x, y)) {
        float scale_factor = scale;
        box.mouseDragged( (mouseX-ofGetWindowWidth() *0.5*(1+shift.x))/scale_factor,
                          (mouseY-ofGetWindowHeight()*0.5*(1+shift.y))/scale_factor, button);
    }

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    if (!gui->isHit(x, y)) {
        float scale_factor = scale;
        box.mousePressed( (mouseX-ofGetWindowWidth() *0.5*(1+shift.x))/scale_factor,
                          (mouseY-ofGetWindowHeight()*0.5*(1+shift.y))/scale_factor, button);
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    if (!gui->isHit(x, y)) {
        float scale_factor = scale;
        box.mouseReleased( (mouseX-ofGetWindowWidth() *0.5*(1+shift.x))/scale_factor,
                           (mouseY-ofGetWindowHeight()*0.5*(1+shift.y))/scale_factor, button);
    }
}

