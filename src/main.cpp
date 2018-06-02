#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
//	ofSetupOpenGL(1920,1080,OF_WINDOW);			// <-------- setup the GL context
    ofGLFWWindowSettings settings;
    settings.width = 960;
    settings.height = 540;
    settings.setPosition(ofVec2f(0,0));
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
    shared_ptr<ofApp> mainApp(new ofApp);
    
    settings.width = 512;
    settings.height = 424;
    settings.setPosition(ofVec2f(960,0));
    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> kinectWindow = ofCreateWindow(settings);
    kinectWindow->setVerticalSync(false);
    
    settings.setPosition(ofVec2f(960,424));
    shared_ptr<ofAppBaseWindow> kinectViewWindow = ofCreateWindow(settings);
    kinectViewWindow->setVerticalSync(false);
    
    
    ofAddListener(kinectWindow->events().draw,mainApp.get(),&ofApp::drawKinectWindow);
    ofAddListener(kinectViewWindow->events().draw,mainApp.get(),&ofApp::drawKinectViewWindow);
    
    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
    
    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
//	ofRunApp(new ofApp());

}
