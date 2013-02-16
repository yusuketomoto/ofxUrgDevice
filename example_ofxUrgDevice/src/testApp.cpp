#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(100);
    ofSetCircleResolution(64);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
    urg.setup();
    
    panelsetup();
}

//--------------------------------------------------------------
void testApp::update(){
    urg.setThresholds(panel.getValueB("Enable Near Thresholding"),
                      panel.getValueB("Enable Far  Thresholding"),
                      panel.getValueB("Enable Diff Thresholding"),
                      panel.getValueI("Near Threshold"),
                      panel.getValueI("Far Threshold"),
                      panel.getValueI("Diff Threshold"));
    urg.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    vector<long> data = urg.getData();
    ofPushMatrix();
    ofTranslate(ofGetWindowWidth()*0.5, ofGetWindowHeight()*0.5);
    
    urg.draw(0, 0, panel.getValueI("What Draw?"));
    ofPopMatrix();
}

void testApp::exit()
{
}

void testApp::panelsetup()
{
    vector<string> draw_modes;
    draw_modes.push_back("DEFAULT");
    draw_modes.push_back("DIFFERENCE");
    draw_modes.push_back("DIFFERENCE_ONLY");
    
    panel.setup();
    panel.addPanel("Settings");
    panel.addTextDropDown("What Draw?", "What Draw?", 0, draw_modes);
    panel.addToggle("Enable Near Thresholding", "Enable Near Thresholding", false);
    panel.addToggle("Enable Far  Thresholding", "Enable Far  Thresholding", false);
    panel.addToggle("Enable Diff Thresholding", "Enable Diff Thresholding", false);
    panel.addSlider("Near Threshold", "Near Threshold", 60, 0, 6000, true);
    panel.addSlider("Far  Threshold", "Far Threshold", 5800, 0, 6000, true);
    panel.addSlider("Diff THreshold", "Diff Threshold", 1500, 0, 2000, true);}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}