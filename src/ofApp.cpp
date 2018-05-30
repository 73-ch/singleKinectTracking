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
//    plane.setPosition(ofVec3f(-300, -300,0));
    
    // kinectの位置にカメラを作成
    kinect_cam.setFov(60);
    kinect_cam.rotate(90, ofVec3f(0,0,1));
    
    kinect_cam.setPosition(0, 300, -400);
    kinect_cam.lookAt(ofVec3f(0));
    kinect_cam.setAspectRatio(512/424);
    
    kinect_view.allocate(512, 424);
    
    // ose setup
    receiver.setup(PORT);
}

void ofApp::update(){
    kinect.update();
    if(kinect.isFrameNew()){
        texture.loadData(kinect.getIrPixelsRef());
        cout << texture.getHeight() << "," << texture.getWidth() << endl;
        
        
//        mesh.clear();
//        {
//            int step = 1;
//            int h = kinect.getDepthPixelsRef().getHeight();
//            int w = kinect.getDepthPixelsRef().getWidth();
//            for(int y = 0; y < h; y += step) {
//                for(int x = 0; x < w; x += step) {
//                    float dist = kinect.getDistanceAt(x, y);
//                    if(dist > 50 && dist < 500) {
//                        ofVec3f pt = kinect.getWorldCoordinateAt(x, y, dist);
//                        
//                        ofColor c;
//                        float h = ofMap(dist, 50, 200, 0, 255, true);
//                        c.setHsb(h, 255, 255);
//                        mesh.addColor(c);
//                        mesh.addVertex(pt);
//                    }
//                }
//            }
//            
//        }

    }
    
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        if (m.getAddress() == "/kinect_cam/position") {
            kinect_cam.setPosition(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2));
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(texture.isAllocated()){
        irShader.begin();
        texture.draw(0, 0, 600, 400);
        irShader.end();
    }
    ofClear(0);
    
//    if (mesh.getVertices().size()) {
        ofPushStyle();
        glPointSize(2);
        ecam.begin();
    
        ofDrawAxis(100);
        ofPushMatrix();
        ofTranslate(0, 0, -100);
        mesh.draw();
        ofPopMatrix();
        ecam.end();
        ofPopStyle();
//    }
    
    // view from kinect
    kinect_view.begin();
    kinect_cam.begin();
    ofClear(0);
    
    ofDrawAxis(5);
    plane.draw(OF_MESH_WIREFRAME);
    
    ofVec3f test_vec = getWorldPosition(ofVec3f(256, 212, 1.0), plane, kinect_cam.getPosition(), kinect_cam.getModelViewProjectionMatrix());
    
    cout << test_vec << endl;
    
    kinect_cam.end();
    kinect_view.end();
    
    
    
    // debug
    ofPushMatrix();
    ofClear(0);
    ecam.begin();
    
    ofDrawAxis(5);
    plane.draw(OF_MESH_WIREFRAME);
    kinect_cam.draw();
    
    ecam.end();
    
    
    // kinect view draw
    kinect_view.draw(ofGetWidth(), 0, -204, 168);
    
    
    
    
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
    ofDrawBitmapStringHighlight("Device Count : " + ofToString(ofxMultiKinectV2::getDeviceCount()), 10, 40);
}

ofVec3f getWorldPosition(ofVec3f g_pos, ofPlanePrimitive target, ofVec3f eye_pos, ofMatrix4x4 cam_mvp_matrix) {
    ofMatrix4x4 inv_mat = cam_mvp_matrix.getInverse();
    ofVec4f out = ofVec4f(g_pos.x, g_pos.y, g_pos.z,1.0)*inv_mat;

    
    ofVec3f screen_world_pos = ofVec3f(out.x/out.w, out.y/out.w, out.z/out.w);
    
    ofVec3f normal = ofVec3f(0,1,0);
    
    screen_world_pos -= eye_pos;
    
    return normal.dot(target.getPosition() - eye_pos)/normal.dot(screen_world_pos) * screen_world_pos + eye_pos;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
