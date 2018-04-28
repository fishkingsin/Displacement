#include "ofApp.h"
//int xSize           = 1600;
//int ySize           = 512;
//int xMid            = xSize/2;
//int yMid            = ySize/2;
//int xRes            = 128;
//int yRes            = 128;
//float _decay        = .991;
float _decay        = 0.99;
//int res             = 8;
float heightMulti = 8;
float xm = 0, ym = 0;

//--------------------------------------------------------------
void ofApp::setup(){
    busesImg.load("image.jpeg");
    img.allocate(busesImg.getWidth(),busesImg.getHeight(), OF_IMAGE_GRAYSCALE);
    shader.load("shader");
//    shader.setUniformTexture("tex0", img, 0);
    float planeScale = 1;//0.85;
    int planeWidth = ofGetWidth() * planeScale;
    int planeHeight = ofGetHeight() * planeScale;
    int planeGridSize = 6;
    int planeColumns = planeWidth / planeGridSize;
    int planeRows = planeHeight / planeGridSize;
    
    plane.set(planeWidth, planeHeight, planeColumns, planeRows, OF_PRIMITIVE_TRIANGLES);
    plane.mapTexCoordsFromTexture(img.getTexture());
    int xRes = img.getWidth();
    int yRes = img.getHeight();
    r0 = new float* [xRes];
    r1 = new float* [xRes];
    r2 = new float* [xRes];
    
    for(int y = 0 ; y < yRes ; y++)
    {
        r0[y] = new float [yRes];
        r1[y] = new float [yRes];
        r2[y] = new float [yRes];
        for(int x = 0 ; x < xRes ; x++)
        {
            r0[y][x] = 0;
            r1[y][x] = 0;
            r2[y][x] = 0;
        }
    }
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    float noiseScale = ofMap(mouseX, 0, ofGetWidth(), 0, 0.1);
    float noiseVel = ofGetElapsedTimef()*0.07;
    
    ofPixels & pixels = img.getPixels();
//    int w = img.getWidth();
//    int h = img.getHeight();
//    for(int y=0; y<h; y++) {
//        for(int x=0; x<w; x++) {
//            int i = y * w + x;
//            float noiseVelue = ofNoise(x * noiseScale, y * noiseScale, noiseVel);
//            pixels[i] = 255 * noiseVelue;
//        }
//    }
    int xRes = img.getWidth();
    int yRes = img.getHeight();
    findRipples();
    swapBuffers();
    for (int y=0; y<yRes; y++){
        for (int x=0; x<xRes; x++){
            int i = y * xRes + x;
            pixels[i]  = r1[x][y] * 255;
        }
    }
    
    
    img.update();
}
void ofApp::findRipples(){
    int xRes = img.getWidth();
    int yRes = img.getHeight();
    for (int y=1; y<yRes-1; y++){
        for (int x=1; x<xRes-1; x++){
            r0[x][y] = (r1[x-1][y] + r1[x+1][y] + r1[x][y-1] + r1[x][y+1]) / 4.0;
            r0[x][y] = r0[x][y] * 2.0 - r2[x][y];
            r0[x][y] *= _decay;
        }
    }
}

void ofApp::swapBuffers(){
    int xRes = img.getWidth();
    int yRes = img.getHeight();
    for (int y=0; y<yRes; y++){
        for (int x=0; x<xRes; x++){
            r2[x][y] -= (r2[x][y] - r1[x][y]) * _decay;
            r1[x][y] -= (r1[x][y] - r0[x][y]) * _decay;
        }
    }
}
//--------------------------------------------------------------
void ofApp::makeRipples(float _x, float  _y){
    int xRes = img.getWidth();
    int yRes = img.getHeight();
    xm = ofClamp((int)(_x/(ofGetWidth()/xRes)),0,xRes-1);
    ym = ofClamp((int)(_y/(ofGetHeight()/yRes)),0,xRes-1);
    //    std::printf("x: %.0f", xm);
    //    std::printf(" y: %.0f\n", ym);
    for (int y=1; y<yRes-1; y++){
        for (int x=1; x<xRes-1; x++){
            float d = ofDist(xm,ym,x,y);
            if (d < 1){
                r1[x][y] -= pow(((3 - d)/3.0),2) * 5.0;
                //                std::printf("r1: %f\n", r1);
            }
        }
    }
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
//    ofRotate(rotation, 1, 0, 0);
    
    plane.drawWireframe();
//    plane.draw();

    ofPopMatrix();
    
    shader.end();
    img.getTexture().unbind();
    
    ofSetColor(ofColor::white);
    img.draw(0, 0,128,128);
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
    makeRipples(x,y);
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
