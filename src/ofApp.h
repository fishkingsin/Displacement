#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxKinectV2.h"
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
    ofImage busesImg;
    
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
    ofxFloatSlider offsetZ;
    ofxFloatSlider power;
    ofxFloatSlider distance;
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
    ofxToggle                bLearnBakground;
    float coolDown;
    ofEasyCam cam;
};
