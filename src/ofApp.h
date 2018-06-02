#pragma once

#include "ofMain.h"
#include "ofxMultiKinectV2.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"

#define STRINGIFY(x) #x

#define PORT 1872

class ofApp : public ofBaseApp{

	public:
		void setup();
    void setupKinectView();
    void setupKinectImage();
		void update();
		void draw();
        void drawKinectWindow(ofEventArgs & args);
        void drawKinectViewWindow(ofEventArgs & args);
    
        ofVec3f getWorldPosition(ofVec3f g_pos, ofPlanePrimitive target, ofVec3f eye_pos, ofMatrix4x4 cam_mvp_matrix);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    ofxMultiKinectV2 kinect;
    ofTexture texture;
    ofImage kinect_color;
    ofEasyCam ecam;
    ofVboMesh mesh;
    ofShader irShader;
    ofFbo depth_fbo;
    float constant;
    int threshold;
    int kinect_width = 512, kinect_height = 424;
    
    ofPlanePrimitive plane;
    
    ofCamera kinect_cam;
    ofFbo kinect_view;
    
    ofFbo kinect_window;
    
    ofxOscReceiver receiver;
    ofxOscSender sender;
    
    ofVec3f test_vec;
    ofMatrix4x4 test_mat;
    
    ofVec3f human_pos;
    
    ofVec3f kinect_look_at;
    
    ofVec2f kinect_window_pos;

    bool adjust_plane;
    
    
    ofxCvColorImage cIrImage;
    ofxCvGrayscaleImage gIrImage;
    ofxCv::ContourFinder contourFinder;
};

static string irFragmentShader =
STRINGIFY(
          uniform sampler2DRect tex;
          uniform float constant;
          void main()
          {
          vec4 col = texture2DRect(tex, gl_TexCoord[0].xy);
          float value = col.r / constant;
          gl_FragColor = vec4(vec3(value), 1.0);
          }
          );