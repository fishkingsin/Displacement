#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    busesImg.load("buses.jpg");
    img.allocate(busesImg.getWidth(), busesImg.getHeight(), OF_IMAGE_GRAYSCALE);
    shader.load("shader");
//    shader.setUniformTexture("tex0", img, 0);
    float planeScale = 1;//0.85;
    int planeWidth = ofGetWidth() * planeScale;
    int planeHeight = ofGetHeight() * planeScale;
    int planeGridSize = 10;
    int planeColumns = planeWidth / planeGridSize;
    int planeRows = planeHeight / planeGridSize;
    
    plane.set(planeWidth, planeHeight, planeColumns, planeRows, OF_PRIMITIVE_TRIANGLES);
    plane.mapTexCoordsFromTexture(img.getTexture());
    
}

//--------------------------------------------------------------
void ofApp::update(){
    float noiseScale = ofMap(mouseX, 0, ofGetWidth(), 0, 0.1);
    float noiseVel = ofGetElapsedTimef()*0.07;
    
    ofPixels & pixels = img.getPixels();
    int w = img.getWidth();
    int h = img.getHeight();
    for(int y=0; y<h; y++) {
        for(int x=0; x<w; x++) {
            int i = y * w + x;
            float noiseVelue = ofNoise(x * noiseScale, y * noiseScale, noiseVel);
            pixels[i] = 255 * noiseVelue;
        }
    }
    img.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    // bind our texture. in our shader this will now be tex0 by default
    // so we can just go ahead and access it there.
    img.getTexture().bind();
    
    shader.begin();
    shader.setUniformTexture("tex0", img, 0);
    shader.setUniformTexture("tex1", busesImg, 1);
    ofPushMatrix();
    
    // translate plane into center screen.
    float tx = ofGetWidth() / 2;
    float ty = ofGetHeight() / 2;
    ofTranslate(tx, ty);
    
    // the mouse/touch Y position changes the rotation of the plane.
    float percentY = mouseY / (float)ofGetHeight();
    float rotation = ofMap(percentY, 0, 1, -60, 60, true) + 60;
    ofRotate(rotation, 1, 0, 0);
    
//    plane.drawWireframe();
    plane.draw();

    ofPopMatrix();
    
    shader.end();
    img.getTexture().unbind();
    
    ofSetColor(ofColor::white);
    img.draw(0, 0);
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
