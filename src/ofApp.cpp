#include "ofApp.h"
//int xSize           = 1600;
//int ySize           = 512;
//int xMid            = xSize/2;
//int yMid            = ySize/2;
//int xRes            = 128;
//int yRes            = 128;
//float _decay        = .991;
//float _decay        = 0.99;
//int res             = 8;
float heightMulti = 8;
float xm = 0, ym = 0;
#ifdef USE_VIDEO
#else
#endif
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetupScreenOrtho();
    sImage.load("tunnel_s.png");
#ifdef USE_VIDEO
    
//    HPV::InitHPVEngine();
//
//    /* Create resources for new player */
//    player.init(HPV::NewPlayer());
    
    /* Try to load file and start playback */
    if (player.load("movie.mov"))
    {
        player.setUseTexture(true);
        
        player.setLoopState(OF_LOOP_NORMAL);
        player.play();
    }else{
        std::exit(-1);
    }
    fbo.allocate(player.getWidth(),player.getHeight());
#else
    lImage.load("tunnel.png");
    fbo.allocate(lImage.getWidth(),lImage.getHeight());
#endif
    img.allocate(sImage.getWidth(),sImage.getHeight(), OF_IMAGE_GRAYSCALE);

    dot.load("dot.png");
    shader.load("shader");
    shader.setUniformTexture("tex0", fbo, 0);
    float planeScale = 1;//0.85;
    int planeWidth = ofGetWidth() * planeScale;
    int planeHeight = ofGetHeight() * planeScale;
    int planeGridSize = 4;
    int planeColumns = planeWidth / planeGridSize;
    int planeRows = planeHeight / planeGridSize;
    
    plane.set(planeWidth, planeHeight, planeColumns, planeRows, OF_PRIMITIVE_TRIANGLES);
    #ifdef USE_VIDEO
    plane.mapTexCoordsFromTexture(player.getTexture());
    #else
    plane.mapTexCoordsFromTexture(lImage.getTexture());
    #endif
    
    
    int xRes = img.getWidth();
    int yRes = img.getHeight();
    r0 = new float* [xRes];
    r1 = new float* [xRes];
    r2 = new float* [xRes];
    
    for(int x = 0 ; x < xRes ; x++)
    {
        r0[x] = new float [yRes];
        r1[x] = new float [yRes];
        r2[x] = new float [yRes];
        for(int y = 0 ; y < yRes ; y++)
        {
            r0[x][y] = 0;
            r1[x][y] = 0;
            r2[x][y] = 0;
        }
    }
    
    ofxKinectV2 tmp;
    vector <ofxKinectV2::KinectDeviceInfo> deviceList = tmp.getDeviceList();
    
    //allocate for this many devices
    kinects.resize(deviceList.size());
    texDepth.resize(kinects.size());
    
    
    gui.setup("", "settings.xml", 10, 100); // most of the time you don't need a name
    //Note you don't have to use ofxKinectV2 as a shared pointer, but if you want to have it in a vector ( ie: for multuple ) it needs to be.
    for(int d = 0; d < kinects.size(); d++){
        kinects[d] = shared_ptr <ofxKinectV2> (new ofxKinectV2());
        ofLogWarning() << deviceList[d].serial;
        kinects[d]->open(deviceList[d].serial);
        
        gui.add(kinects[d]->params);
    }
    //testing
    
    gui.add(fps.setup("fps",""));
    gui.add(offsetX.setup("offsetX", 0, -2000, 2000));
    gui.add(offsetY.setup("offsetY", 0, -2000, 1000));
    gui.add(offsetZ.setup("offsetZ", 0, -2000, 2000));
    gui.add(damping.setup("damping", 0.999, 0.9, 0.9999));
    gui.add(scale.setup("scale", 10, 1, 1000));
    gui.add(power.setup("power", 0.1, 0.00001, 0.1));
    gui.add(distance.setup("distance", 3, 1, 200));
    gui.add(threshold.setup("threshold", 50, 1, 255));
    gui.add(minArea.setup("minArea", 20, 1, (340*240)/3));
    gui.add(maxArea.setup("maxArea", (340*240)/3, 1, (340*240)));
    gui.add(cooldownInterval.setup("cooldownInterval",1, 0.00001,2));
    gui.add(bLearnBakground.setup("bLearnBakground",false));
    gui.add(initBackground.setup("initBackground",3,1,10));
    gui.add(fullScreen.setup("fullScreen",false));
    gui.add(bMirror.setup("bMirror",false));
    
    fullScreen.addListener(this, &ofApp::toggleFullScreen);
    gui.loadFromFile("settings.xml");
    
    bHide = false;
    cam.setPosition(ofGetWidth()*0.5, ofGetHeight()*0.5, 500);
    cam.setVFlip(true);
    
//    cam.disableMouseInput();
    server.setName("displacement");
    binitBackground = false;
    
}
void ofApp::toggleFullScreen(bool &b){
    ofSetFullscreen(b);
}
//--------------------------------------------------------------
void ofApp::exit()
{
#ifdef USE_VIDEO
    /* Cleanup and destroy HPV Engine upon exit */
//    HPV::DestroyHPVEngine();
#endif
}
//--------------------------------------------------------------
void ofApp::update(){
    if((ofGetElapsedTimef() - initBackground) > 3 && !binitBackground ){
        bLearnBakground = true;
        binitBackground = true;
    }
    fps = ofToString(ofGetFrameRate());
    
#ifdef USE_VIDEO
    player.update();
//    HPV::Update();
#else
#endif
    for(int d = 0; d < kinects.size(); d++){
        kinects[d]->update();
        if( kinects[d]->isFrameNew() ){
            if(!colorImg.bAllocated){
                int w = kinects[d]->getDepthPixels().getWidth()*kinects.size()*0.5;
                int h = kinects[d]->getDepthPixels().getHeight()*0.5;
                kinectsFbo.allocate(w,h,GL_RGB);
                colorImg.allocate(w, h);
                grayImage.allocate(w, h);
                grayBg.allocate(w, h);
                grayDiff.allocate(w, h);
                pixels.allocate(w, h, OF_IMAGE_COLOR);
            }
            texDepth[d].loadData( kinects[d]->getDepthPixels() );
        }
    }
    
//            texRGB[d].loadData( kinects[d]->getRgbPixels() );
    if(colorImg.bAllocated){
        kinectsFbo.begin();
        for(int d = 0; d < kinects.size(); d++){
            texDepth[d].draw(kinects[d]->getDepthPixels().getWidth()*d*0.5,0,kinects[d]->getDepthPixels().getWidth()*0.5,kinects[d]->getDepthPixels().getHeight()*0.5);
        }
        kinectsFbo.end();
        kinectsFbo.readToPixels(pixels);
        colorImg.setFromPixels(pixels);
        
        grayImage = colorImg;
        if (bLearnBakground){
            grayBg = grayImage;        // the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBakground = false;
        }
        
        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayDiff, minArea, maxArea, 10, false);    // find holes
        
        for (int i = 0; i < contourFinder.nBlobs; i++){
            ofPoint input = ofPoint((contourFinder.blobs[i].centroid.x / kinectsFbo.getWidth()) *ofGetWidth() ,
                                    (contourFinder.blobs[i].centroid.y / kinectsFbo.getHeight()) *ofGetHeight() );
            if(abs(ofGetElapsedTimef()-coolDown) > cooldownInterval){
                makeRipples(input.x, input.y);
                coolDown = ofGetElapsedTimef();
            }
            
        }
    }
    
    float noiseScale = ofMap(mouseX, 0, ofGetWidth(), 0, 0.1);
    float noiseVel = ofGetElapsedTimef()*0.07;
    
    ofPixels & pixels = img.getPixels();
    int xRes = img.getWidth();
    int yRes = img.getHeight();
    findRipples();
    swapBuffers();
    for (int y=0; y<yRes; y++){
        for (int x=0; x<xRes; x++){
            int i = y * xRes + x;
            pixels[i]  = 255 - ((r1[x][y]+0.5) * 255);
        }
    }
    
    
    img.update();
#ifdef USE_VIDEO
    xm = ofGetMouseX()/(ofGetWidth()/player.getWidth());
    ym = ofGetMouseY()/(ofGetHeight()/player.getHeight());
    fbo.begin();
    img.draw(0, 0, player.getWidth(), player.getHeight());
    fbo.end();
#else
    xm = ofGetMouseX()/(ofGetWidth()/lImage.getWidth());
    ym = ofGetMouseY()/(ofGetHeight()/lImage.getHeight());
    fbo.begin();
    img.draw(0, 0, lImage.getWidth(), lImage.getHeight());
    //    dot.draw(xm-(dot.getWidth()*0.5), ym-(dot.getHeight()*0.5));
    fbo.end();
#endif
    
}
void ofApp::findRipples(){
    int xRes = img.getWidth();
    int yRes = img.getHeight();
    for (int y=1; y<yRes-1; y++){
        for (int x=1; x<xRes-1; x++){
            r0[x][y] = (r1[x-1][y] + r1[x+1][y] + r1[x][y-1] + r1[x][y+1]) / 4.0;
            r0[x][y] = r0[x][y] * 2.0 - r2[x][y];
            r0[x][y] *= damping;
        }
    }
}

void ofApp::swapBuffers(){
    int xRes = img.getWidth();
    int yRes = img.getHeight();
    for (int y=0; y<yRes; y++){
        for (int x=0; x<xRes; x++){
            r2[x][y] -= (r2[x][y] - r1[x][y]) * damping;
            r1[x][y] -= (r1[x][y] - r0[x][y]) * damping;
        }
    }
}
//--------------------------------------------------------------
void ofApp::makeRipples(float _x, float  _y){
    int xRes = img.getWidth();
    int yRes = img.getHeight();
    
    xm = ofClamp((int)(_x/(ofGetWidth()/xRes)),0,xRes-1);
    ym = ofClamp((int)(_y/(ofGetHeight()/yRes)),0,yRes-1);
    //    std::printf("x: %.0f", xm);
//        std::printf(" y: %.0f\n", ym);
    int _distance = distance;
    for (int y=1; y<yRes-1; y++){
        for (int x=1; x<xRes-1; x++){
            float d = ofDist(xm,ym,x,y);
            if (d < _distance){
                r1[x][y] -= pow(((_distance - d)/_distance),2) * power;
                //                std::printf("r1: %f\n", r1);
            }
        }
    }
    
    
}
//--------------------------------------------------------------
void ofApp::draw(){

    glEnable(GL_DEPTH_TEST);
    ofEnableAlphaBlending();
    cam.begin();
    // bind our texture. in our shader this will now be tex0 by default
    // so we can just go ahead and access it there.
    img.getTexture().bind();
    
    shader.begin();
    shader.setUniformTexture("tex0", fbo, 0);
#ifdef USE_VIDEO
    shader.setUniformTexture("tex1", player.getTexture(), 1);
#else
    shader.setUniformTexture("tex1", lImage, 1);
#endif
    
    shader.setUniform1f("scale", scale);
    ofPushMatrix();
    
    // translate plane into center screen.
    float tx = 0;//ofGetWidth() / 2;
    float ty = 0;//ofGetHeight() / 2;
    ofTranslate(tx+offsetX, ty+offsetY,offsetZ);
    
    // the mouse/touch Y position changes the rotation of the plane.
    float percentY = mouseY / (float)ofGetHeight();
    float rotation = ofMap(percentY, 0, 1, -60, 60, true) + 60;
    if(bMirror){
        ofRotate(180, 0, 1, 0);
    }
    plane.draw();

//    plane.drawWireframe();
    ofPopMatrix();
    
    shader.end();
    img.getTexture().unbind();
    cam.end();
    
    glDisable(GL_DEPTH_TEST);
    ofDisableAlphaBlending();
    ofSetColor(ofColor::white);
//    screen.end();
    server.publishScreen();
    if(bHide){
        player.draw(0,0,ofGetWidth(), ofGetHeight());
        fbo.draw(0, 0,128,128);
        img.draw(128, 0, 128,128);
//        for(int d = 0; d < kinects.size(); d++){
//            float dwHD = 1920/4;
//            float dhHD = 1080/4;
//
//            float shiftY = 100 + ((10 + texDepth[d].getHeight()) * d);
//
//            texDepth[d].draw(200, shiftY);
//
//        }
        if(grayImage.bAllocated){
            grayImage.draw(256,0);
        }
        ofPushMatrix();
        ofTranslate(256, 0);
        contourFinder.draw();
        ofPopMatrix();
        for (int i = 0; i < contourFinder.nBlobs; i++){
            ofPushMatrix();
            ofPoint input = ofPoint((contourFinder.blobs[i].centroid.x / grayImage.getWidth()) *ofGetWidth() ,
                                    (contourFinder.blobs[i].centroid.y / grayImage.getHeight()) *ofGetHeight() );
            ofTranslate(input);
            ofDrawAxis(10);
            ofPopMatrix();
        }
        gui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case OF_KEY_TAB:
            bHide = !bHide;
            break;
        case '1':
//            cam.disableMouseInput();
            break;
        case '2':
//            cam.enableMouseInput();
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(abs(ofGetElapsedTimef()-coolDown) > cooldownInterval){
        makeRipples(x,y);
        coolDown = ofGetElapsedTimef();
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    makeRipples(x,y);
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

//--------------------------------------------------------------
#ifdef USE_VIDEO
//void ofApp::onHPVEvent(const HPVEvent& event)
//{
//    switch (event.type)
//    {
//        case HPV::HPVEventType::HPV_EVENT_PLAY:
//            cout << "'" << event.player->getFilename() << "': play event" << endl;
//            break;
//        case HPV::HPVEventType::HPV_EVENT_PAUSE:
//            cout << "'" << event.player->getFilename() << "': pause event" << endl;
//            break;
//        case HPV::HPVEventType::HPV_EVENT_RESUME:
//            cout << "'" << event.player->getFilename() << "': resume event" << endl;
//            break;
//        case HPV::HPVEventType::HPV_EVENT_STOP:
//            cout << "'" << event.player->getFilename() << "': stop event" << endl;
//            break;
//        case HPV::HPVEventType::HPV_EVENT_LOOP:
//            cout << "'" << event.player->getFilename() << "': loop event" << endl;
//            break;
//        case HPV::HPVEventType::HPV_EVENT_NUM_TYPES:
//        default:
//            break;
//    }
//}

#endif
