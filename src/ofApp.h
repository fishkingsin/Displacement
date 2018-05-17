#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxKinectV2.h"
#include "ofxSyphon.h"
//#define USE_VIDEO
#ifdef USE_VIDEO
#include "ofxHPVPlayer.h"
#endif
#include "ofxAutoReloadedShader.h"
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
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
    ofImage img;
    ofShader shader;
    
    ofPlanePrimitive plane;
    ofImage sImage;
#ifdef USE_VIDEO
    ofxHPVPlayer player;
    void onHPVEvent(const HPVEvent& event);
#else
    ofImage lImage;
#endif
    void findRipples();
    void swapBuffers();
    void makeRipples(float _x, float  _y);
    float                **r0;
    float                **r1;
    float                **r2;
    ofxPanel gui;
    ofxFloatSlider damping;
    ofxFloatSlider scale;
    
    ofxFloatSlider offsetX;
    ofxFloatSlider offsetY;
    ofxFloatSlider offsetTrackingY;
    ofxFloatSlider offsetZ;
    ofxFloatSlider power;
    ofxFloatSlider distance;
    ofxFloatSlider noiseScale;
    ofxFloatSlider noiceSpeed;
    ofxFloatSlider masterNoiseScale;
    ofxToggle fullScreen;
    ofxToggle bMirror;
    ofxToggle bInvert;
    ofxIntSlider initBackground;
    void toggleFullScreen(bool &b);
    bool binitBackground;
    
    ofxLabel fps;
    bool bHide;
    
    vector < shared_ptr<ofxKinectV2> > kinects;
    vector <ofTexture> texDepth;
//    vector <ofTexture> texRGB;
    
    ofxCvColorImage            colorImg;
    
    ofxCvGrayscaleImage     grayImage;
    ofxCvGrayscaleImage     grayBg;
    ofxCvGrayscaleImage     grayDiff;
    
    ofxCvContourFinder     contourFinder;
    
    ofxFloatSlider                 threshold;
    ofxFloatSlider                 maxArea;
    ofxFloatSlider                 minArea;
    ofxFloatSlider                 cooldownInterval;
    ofxToggle                bLearnBakground;
    float coolDown;
    ofCamera cam;
    ofFbo fbo;
    ofFbo kinectsFbo;
    ofFbo videoFbo;
    ofPixels pixels;
    ofImage dot;
    ofxSyphonServer server;
    
    
    
    
};
