#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1920,1080,OF_WINDOW);			// <-------- setup the GL context
//    ofGLFWWindowSettings settings;
//    settings.width = 1920;
//    settings.height = 1080;
//    settings.setPosition(ofVec2f(0,0));
//    settings.resizable = true;
//    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
//    
//    settings.width = 512;
//    settings.width = 424;
//    shared_ptr<ofAppBaseWindow> kinectWindow = ofCreateWindow(settings);
//    
//    shared_ptr<ofApp> mainApp(new ofApp);
//    ofAddListener(kinectWindow->events().draw,mainApp.get(),&ofApp::drawKinectWindow);
//    
//    ofRunApp(mainWindow, mainApp);
//    ofRunMainLoop();
    
    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
