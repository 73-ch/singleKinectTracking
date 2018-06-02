#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    kinect.open(true, true, 0, 2);
    kinect.start();

    
    mesh.setUsage(GL_DYNAMIC_DRAW);
    mesh.setMode(OF_PRIMITIVE_POINTS);
    
    ecam.setAutoDistance(false);
    ecam.setDistance(200);
    ecam.setFarClip(10000);
    
    irShader.setupShaderFromSource(GL_FRAGMENT_SHADER, irFragmentShader);
    irShader.linkProgram();
    
    // plane setting
    plane.set(600, 600, 12,12);
    plane.rotate(90, ofVec3f(1,0,0));
    plane.setPosition(0, 180, 0);
    
    // kinectの位置にカメラを作成
    kinect_cam.setFov(60);
    kinect_cam.rotate(90, ofVec3f(0,0,1));
    kinect_cam.setPosition(0, 300, -400);
    kinect_look_at = ofVec3f(0);
    kinect_cam.lookAt(kinect_look_at);
    kinect_cam.setAspectRatio(kinect_width/kinect_height);
    
    kinect_view.allocate(kinect_width, kinect_height);
    
    kinect_window.allocate(kinect_width, kinect_height);
    
    // ose setup
    receiver.setup(PORT);
    
    // ofxcv setup
    contourFinder.setMinAreaRadius(1);
    contourFinder.setMaxAreaRadius(100);
    contourFinder.setThreshold(15);
    contourFinder.getTracker().setPersistence(15);
    contourFinder.getTracker().setMaximumDistance(32);
    
    // kinect color image setup
    kinect_color.allocate(kinect_width, kinect_height, OF_IMAGE_COLOR);
    
    
    // irshader
    irShader.setupShaderFromSource(GL_FRAGMENT_SHADER, irFragmentShader);
    irShader.linkProgram();
    
    depth_fbo.allocate(kinect_width,kinect_height, GL_RGB);
    constant = 65550.0;
    threshold = 128;
    
    // adjust_plane
    adjust_plane = false;
}

void setupKinectView () {
    
}

void setupKinectImage() {
    
}

void ofApp::update(){
    
    kinect.update();
    if(kinect.isFrameNew()){
        kinect_color.setFromPixels(kinect.getColorPixelsRef());
        texture.loadData(kinect.getIrPixelsRef());
        if (texture.isAllocated()) {
            
            depth_fbo.begin();
            irShader.begin();
            irShader.setUniform1f("constant", constant);
            texture.draw(kinect_width,0,-kinect_width, kinect_height);
            irShader.end();
            depth_fbo.end();
            
            ofPixels d_pix;
            
            depth_fbo.readToPixels(d_pix);
            
            cIrImage.setFromPixels(d_pix, kinect_width, kinect_height);
            gIrImage = cIrImage;
            gIrImage.threshold(threshold);
            
            contourFinder.findContours(gIrImage);
            
            float sum_x, sum_y;
            
            int size = contourFinder.size();
            
            for (int i = 0; i < size; i++) {
                ofPoint center = ofxCv::toOf(contourFinder.getCenter(i));
                sum_x += center.x;
                sum_y += center.y;
//                cout << center.x  <<  "," << center.y << endl;
            }
            
            kinect_window_pos = ofVec2f(sum_x / size, sum_y / size);
            
            ofVec3f kinect_tracking_point = ofVec3f(kinect_window_pos.x / kinect_width * 2., (1. - kinect_window_pos.y/kinect_height), 1.);
            human_pos = getWorldPosition(kinect_tracking_point, plane, kinect_cam.getGlobalPosition(), kinect_cam.getModelViewProjectionMatrix());
        }
    }
    
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        if (m.getAddress() == "/kinect_cam/position") {
            kinect_cam.setPosition(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2));
            kinect_cam.lookAt(kinect_look_at);
        } else if (m.getAddress() == "/kinect_cam/look_at") {
            kinect_look_at = ofVec3f(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2));
            kinect_cam.lookAt(ofVec3f(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2)));
        } else if (m.getAddress() == "/threshold") {
            threshold = m.getArgAsInt(0);
        } else if (m.getAddress() == "/constant") {
            constant = m.getArgAsFloat(0);
        } else if (m.getAddress() == "/adjust_plane") {
            adjust_plane = !adjust_plane;
        }
    }
    
    float time = ofGetElapsedTimef();
//    human_pos = getWorldrPosition(ofVec3f(cos(time), sin(time), 1.0), plane, kinect_cam.getGlobalPosition(), kinect_cam.getModelViewProjectionMatrix());
}

//--------------------------------------------------------------
void ofApp::draw(){
    // debug
    ofPushMatrix();
    ofClear(0);
    ecam.begin();
    test_mat = ecam.getModelViewProjectionMatrix();
    
    ofDrawAxis(5);
    ofSetColor(255);
    plane.draw(OF_MESH_WIREFRAME);
    
    ofSetColor(255, 0, 0);
    ofDrawSphere(human_pos, 50);
    
    ofSetColor(255);
    
    kinect_cam.draw();
    
    ecam.end();
    
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
    ofDrawBitmapStringHighlight("Device Count : " + ofToString(ofxMultiKinectV2::getDeviceCount()), 10, 40);
}

void ofApp::drawKinectWindow(ofEventArgs &args) {
    kinect_window.begin();
    depth_fbo.draw(0,0);
    // draw contour
    ofxCv::RectTracker& tracker = contourFinder.getTracker();
    ofSetColor(255, 255, 255, 255);
    for(int i = 0; i < contourFinder.size(); i++) {
        ofPoint center = ofxCv::toOf(contourFinder.getCenter(i));
        ofPushMatrix();
        ofTranslate(center.x, center.y);
        
        int label = contourFinder.getLabel(i);
        string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
        ofDrawBitmapString(msg, 0, 0);
        ofVec2f velocity = ofxCv::toOf(contourFinder.getVelocity(i));
        ofScale(5, 5);
        ofDrawLine(0, 0, velocity.x, velocity.y);
        ofPopMatrix();
    }
    kinect_window.end();
    
    kinect_window.draw(0,0);
    ofDrawCircle(kinect_window_pos, 4);
}

void ofApp::drawKinectViewWindow(ofEventArgs &args) {
    ofEnableAlphaBlending();
    kinect_cam.begin();
    
    ofClear(0);
    ofSetColor(255);
    ofDrawSphere(human_pos, 50);
    ofDrawAxis(5);
    ofSetColor(255, 255, 255, 150);
    plane.draw(OF_MESH_WIREFRAME);
    
    ofPushMatrix();
    ofTranslate(0,-180, 0);
    ofSetColor(255, 255, 0, 255);
    plane.draw(OF_MESH_WIREFRAME);
    ofPopMatrix();
    
    
//    cout << plane.getPosition() << endl;
    
//        cout << kinect_cam.getModelViewProjectionMatrix() << endl;
    
    
    kinect_cam.end();
    
    ofDrawCircle(kinect_window_pos, 4);
    
    if (adjust_plane) {
        cout << "read" << endl;
        ofSetColor(255, 255, 255, 80);
//        kinect_window.draw(0, 0);
        kinect_color.draw(0,0, kinect_width, kinect_height);
    }
}

ofVec3f ofApp::getWorldPosition(ofVec3f g_pos, ofPlanePrimitive target, ofVec3f eye_pos, ofMatrix4x4 cam_mvp_matrix) {
    ofVec4f out = ofVec4f(g_pos.x, g_pos.y, g_pos.z, 1.0) * cam_mvp_matrix.getInverse();
    ofVec3f s = ofVec3f(out.x/out.w, out.y/out.w, out.z/out.w) - eye_pos;
    
//    cout << s << endl;
    
    ofVec3f n = ofVec3f(0,1,0);
    
    return n.dot(target.getGlobalPosition() - eye_pos)/n.dot(s) * s + eye_pos;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    test_vec = getWorldPosition(ofVec3f(float(x)/ofGetWidth()* 2. - 1., float(y)/ofGetHeight() * -2. +1., 1.0), plane, ecam.getGlobalPosition(), test_mat);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
