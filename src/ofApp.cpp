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
    kinect_cam.setAspectRatio(512/424);
    
    kinect_view.allocate(kinect_width, kinect_height);
    
    // ose setup
    receiver.setup(PORT);
    
    // ofxcv setup
    contourFinder.setMinAreaRadius(1);
    contourFinder.setMaxAreaRadius(100);
    contourFinder.setThreshold(15);
    contourFinder.getTracker().setPersistence(15);
    contourFinder.getTracker().setMaximumDistance(32);
    
    
    // irshader
    irShader.setupShaderFromSource(GL_FRAGMENT_SHADER, irFragmentShader);
    irShader.linkProgram();
    
    depth_fbo.allocate(kinect_width,kinect_height, GL_RGB);
    constant = 65550.0;
    threshold = 128;
}

void ofApp::update(){
    kinect.update();
    if(kinect.isFrameNew()){
        texture.loadData(kinect.getIrPixelsRef());
        if (texture.isAllocated()) {
            depth_fbo.begin();
            
            irShader.begin();
            cout << constant << endl;
            irShader.setUniform1f("constant", constant);
            texture.draw(0,0,kinect_width, kinect_height);
            irShader.end();
            depth_fbo.end();
            
            ofPixels d_pix;
            
            depth_fbo.readToPixels(d_pix);
            
            cIrImage.setFromPixels(d_pix, kinect_width, kinect_height);
            gIrImage = cIrImage;
            gIrImage.threshold(threshold);
            
            contourFinder.findContours(gIrImage);
            
            float sum_x, sum_y;
            
            for (int i = 0; i < contourFinder.size(); i++) {
                ofPoint center = ofxCv::toOf(contourFinder.getCenter(i));
                sum_x += center.x;
                sum_y += center.y;
//                cout << center.x  <<  "," << center.y << endl;
            }
            
            ofVec3f kinect_tracking_point = ofVec3f(sum_y/kinect_width, sum_x/kinect_height, 1.);
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
        }
    }
    
    float time = ofGetElapsedTimef();
//    human_pos = getWorldrPosition(ofVec3f(cos(time), sin(time), 1.0), plane, kinect_cam.getGlobalPosition(), kinect_cam.getModelViewProjectionMatrix());
}

//--------------------------------------------------------------
void ofApp::draw(){
//    if(texture.isAllocated()){
//        irShader.begin();
//        texture.draw(0, 0, 600, 400);
//        irShader.end();
//    }
    ofClear(0);
    
    // view from kinect
    kinect_view.begin();
    kinect_cam.begin();
    ofClear(0);
    ofDrawSphere(human_pos, 50);
    ofDrawAxis(5);
    plane.draw(OF_MESH_WIREFRAME);
    
//    cout << kinect_cam.getModelViewProjectionMatrix() << endl;
    
    
    kinect_cam.end();
    kinect_view.end();
    
    
    // debug
    ofPushMatrix();
    ofClear(0);
    ecam.begin();
    test_mat = ecam.getModelViewProjectionMatrix();
    
    ofDrawAxis(5);
    ofSetColor(255);
    plane.draw(OF_MESH_WIREFRAME);
    
    ofSetColor(255, 0, 0);
//    cout << test_vec << endl;
    ofDrawSphere(human_pos, 50);
    
    ofSetColor(255);
    
    kinect_cam.draw();
    
    ecam.end();
    
    
    // kinect view draw
    kinect_view.draw(ofGetWidth()-kinect_width, 0, kinect_width, kinect_height);
//    ofSetColor
    depth_fbo.draw(ofGetWidth()-kinect_width*2, 0, kinect_width, kinect_height );
    
    
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
    ofDrawBitmapStringHighlight("Device Count : " + ofToString(ofxMultiKinectV2::getDeviceCount()), 10, 40);
}

void ofApp::drawKinectWindow(ofEventArgs &args) {
    
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
