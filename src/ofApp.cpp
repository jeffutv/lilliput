//         __     ________
//        / /__  / __/ __/_  __
//   __  / / _ \/ /_/ /_/ / / /
//  / /_/ /  __/ __/ __/ /_/ /
//  \____/\___/_/ /_/  \__,_/   LILLIPUT �2017     */

//  ofApp.cpp
//  Lilliput

#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
    
    defineShaders();
    
<<<<<<< HEAD
    isRecording = false;
=======
    recordingState = PAUSED;
>>>>>>> fixKinectAgain
    humanDetected = false;
    gnomeDirectory = "/Jeffu Documents/ART/2017 Lilliput/Saved Gnomes";
    // Would like to read this from a .txt file - even better, to pick a folder.
    
    
    w = 1920;
    h = 1080;
    depthW = 1500;
    depthH = 1080;
    saveW = 1024; //256 / 512 / 1024;
    saveH = 848; //212 / 424 / 848;
    
    screenRotation = 90;
    ofSetFrameRate(30);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetVerticalSync(true);
    process_occlusion = false;
//    draw_video = true;
    maxFramesPerGnome = 900;
    minFramesPerGnome = 90;
    recordingDelay = 0.5;
    recordingTimer = 0.0;
    gnomeInterval = 3.0;
    gnomeTimer = 0.0;
    calibrate = true;
    
    
    // Allocate FBOs
    fboBlurOnePass.allocate(saveW, saveH, GL_RGBA);
    fboBlurTwoPass.allocate(saveW, saveH, GL_RGBA);
    frameFbo.allocate(saveW, saveH, GL_RGBA);
    depthFbo.allocate(saveW, saveH, GL_RGBA);
    irFbo.allocate(depthW, depthH, GL_RGB);
    
    // Allocate CV Images
    colorImg.allocate(depthW, depthH);
    grayImage.allocate(depthW, depthH);
    grayBg.allocate(depthW, depthH);
    grayDiff.allocate(depthW, depthH);
    threshold = 80;
    
    
    // Initialize Kinect
>>>>>>> fixKinectAgain
    kinect0.open(true, true, 0, 2);
    // Note :
    // Default OpenCL device might not be optimal.
    // e.g. Intel HD Graphics will be chosen instead of GeForce.
    // To avoid it, specify OpenCL device index manually like following.
    // kinect1.open(true, true, 0, 2); // GeForce on MacBookPro Retina
    
    kinect0.start();
    gr.setup(kinect0.getProtonect(), 2);
    
    // Set threadRecorder defaults
    threadRecorder.setPrefix("/gnome_");
    threadRecorder.setFormat("png");
    
    // Loop through and initialize Gnomes
    numGnomes = 5;
    for (int i=0; i<numGnomes; i++) {
        gnomes[i].setup();
        
        // If we use a vector we should use this code
//        gnome tempGnome;
//        tempGnome.setup();
//        gnomes.push_back(tempGnome);
    }
}


//--------------------------------------------------------------
void ofApp::update(){
    detectHuman();
    kinect0.update();
    if (kinect0.isFrameNew()){
        
    // Get Kinect Frame Data
        colorTex0.loadData(kinect0.getColorPixelsRef());    // Get Kinect Color data
        depthTex0.loadData(kinect0.getDepthPixelsRef());    // Get Kinect Depth data
        irTex0.loadData(kinect0.getIrPixelsRef());          // Get Kinect IR data if needed

        
    // Set Depth Texture
        depthTex0.setTextureMinMagFilter(GL_LINEAR, GL_LINEAR); // or GL_LINEAR
        gr.update(depthTex0, colorTex0, process_occlusion);
        // any chance we can feather the edge and get rid of single outlier pixels?
        blurDepth();
        
    // Draw IR to FBO
        irFbo.begin();
        ofClear(0, 0, 0, 0);
        irShader.begin();
        irTex0.draw(0,0, depthW, depthH);
        irShader.end();
        irFbo.end();
    
    // Calculate Open CV BG difference
        ofPixels pix; // allocate pix
        irFbo.readToPixels(pix);
        colorImg = pix;
        grayImage = colorImg;
        // grayImage = kinect0.getDepthPixelsRef();
        
    // Calibrate Background
        if (calibrate) {
            calibrateBackground();
        }
        
        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayDiff, 20, (depthW*depthH)/3, 10, true);	// find holes
        
        
    // Check Recording
        checkRecording();

    // Update Gnomes
        // Loop through & Update Gnomes if active
        for (int i=0; i<numGnomes; i++) {
            if (gnomes[i].activeGnome) {
                gnomes[i].update();
            }
        }
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255);
    ofSetColor(255);
    
//    testTex0.draw(0, 0, w, h);
    
    if (colorTex0.isAllocated() && depthTex0.isAllocated()) {
        
    // Draw Kinect video frame
        colorTex0.draw(0, 0, w, h);
        
    // Draw Depth
        if (draw_depth) {
            depthShader.begin();
            depthTex0.draw((w-depthW+50)/2, -50, depthW+10, depthH+100);
            depthShader.end();
>>>>>>> fixKinectAgain
        }
        
    // Draw Blurred Depth
        if (draw_blur) {
            fboBlurTwoPass.draw((w-depthW+50)/2, -50, depthW+10, depthH+100);
//            fboBlurTwoPass.draw(210,0,depthW, depthH);
        }
        
    // Draw IR
        if (draw_ir) {
            irFbo.draw((w-depthW+50)/2, -50, depthW+10, depthH+100);
        }
        
    // Draw Registered
        if (draw_registered) {
            gr.getRegisteredTexture(process_occlusion).draw((w-depthW+50)/2, -50, depthW+10, depthH+100);
            // gr.getRegisteredTexture(process_occlusion).draw(0, 0, w, h);
        }
        
    // Draw Gray CV Image
        if (draw_gray) {
            grayDiff.draw(210, 0, depthW, depthH);
        }
        
    // Draw Contours
        ofSetHexColor(0xffffff);
        contourFinder.draw(210, 0); // Draw the whole contour finder
        
        // or, instead we can draw each blob individually from the blobs vector,
        // this is how to get access to them:
        /*
        for (int i = 0; i < contourFinder.nBlobs; i++){
            contourFinder.blobs[i].draw(210, 0);
            
            // draw over the centroid if the blob is a hole
            ofSetColor(255);
            if(contourFinder.blobs[i].hole){
                ofDrawBitmapString("hole",
                                   contourFinder.blobs[i].boundingRect.getCenter().x + 210,
                                   contourFinder.blobs[i].boundingRect.getCenter().y + 0);
            }
        }
         */

        
    // Loop through & Draw Gnomes
        for (int i=0; i<numGnomes; i++) {
            if (gnomes[i].activeGnome) {
                gnomes[i].draw();
            }
        }
        
    // Draw Recording Icon
        if (recordingState == RECORDING || recordingState == WAITING_TO_STOP) {
            // Draw Recorded frame
            // frameFbo.draw(0, 0, w, h);
            
            ofSetColor(255, 0, 0);
            ofDrawCircle(30, 50, 15);
            
            ofSetColor(255, 255, 255);
//            ofRotate(-90);
            ofDrawBitmapString(ofToString(frameCount), 30, 80);
        }
        
    // Draw Frame Rate to screen
        ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
        
    }

}


//--------------------------------------------------------------
void ofApp::checkRecording(){
    
    if (recordingState == PAUSED) {
        return;
    }
    
    if (recordingState == WAITING) {
        if (ofGetElapsedTimef() > recordingTimer) {
            startRecording();
        }
    }
    
    else if (recordingState == WAITING_TO_STOP && ofGetElapsedTimef() > recordingTimer) {
        stopRecording();
    }
    
    else if (frameCount > maxFramesPerGnome) {
        stopRecording();
        // HMMM - what to do if someone wants to stay & play longer?
    }
    
    else {
        saveFrame();
        frameCount ++;
        if (ofGetElapsedTimef() > gnomeTimer) {
            activateGnome();
        }
    }
}


//--------------------------------------------------------------
void ofApp::detectHuman(){
    
    if (humanDetected) {
        if (recordingState == PAUSED) {
            waitToStartRecording();
        }
        else if (recordingState == WAITING_TO_STOP) {
            recordingState = RECORDING;
        }
    } else {
        if (recordingState == WAITING) {
            recordingState = PAUSED;
        }
        else if (recordingState == RECORDING) {
            waitToStopRecording();
        }
    }
}


//--------------------------------------------------------------
void ofApp::waitToStartRecording(){
    
    recordingState = WAITING;
    recordingTimer = ofGetElapsedTimef() + recordingDelay;

}

//--------------------------------------------------------------
void ofApp::waitToStopRecording(){
    
    recordingState = WAITING_TO_STOP;
    recordingTimer = ofGetElapsedTimef() + recordingDelay;
    
}


//--------------------------------------------------------------
void ofApp::startRecording(){
    
    recordingState = RECORDING;
    frameCount = 0;
    makeNewDirectory();
    activateGnome();
}


//--------------------------------------------------------------
void ofApp::activateGnome(){

    for (int i=0; i<numGnomes; i++) {
        if (!gnomes[i].activeGnome) {
            gnomes[i].reset();
            gnomeTimer = ofGetElapsedTimef() + gnomeInterval + ofRandom(5);
            return;
        }
    }
    //    theGnome.chooseRandomGnome();
    
}


//--------------------------------------------------------------
void ofApp::makeNewDirectory(){
    currentPath = gnomeDirectory + "/gnome_" + ofGetTimestampString();
    ofDirectory dir(currentPath);
    dir.createDirectory(currentPath);
    
    // IT WORKS!!!  http://openframeworks.cc/documentation/utils/ofDirectory/
    
    //recorder.setPrefix(ofToDataPath("recording1/frame_")); // this directory must already exist
    
    threadRecorder.setPath(currentPath); // this directory must already exist
    threadRecorder.setCounter(0);
    
    if(!threadRecorder.isThreadRunning()){
        threadRecorder.startThread();
        //        threadRecorder.startThread(false, true);
    }
}


//--------------------------------------------------------------
void ofApp::blurDepth(){
    
    // Draw Depth texture into a Frame Buffer Object
    depthFbo.begin();
    ofClear(0, 0, 0, 0);
    depthShader.begin();
    ofSetColor(ofColor::white);
    depthTex0.draw(0, 0, saveW, saveH);
    //    fboBlurTwoPass.draw(0, 0, saveW, saveH);   // draw blurred version instead
    depthShader.end();
    depthFbo.end();
    
    
    // Blur Shader - Pass 1
    fboBlurOnePass.begin();
    ofClear(0, 0, 0, 0);
    shaderBlurX.begin();
    shaderBlurX.setUniform1f("blurAmnt", 1.0);
    depthFbo .draw(0, 0, saveW, saveH);
    shaderBlurX.end();
    fboBlurOnePass.end();
    
    
    // Blur Shader - Pass 2
    fboBlurTwoPass.begin();
    ofClear(0, 0, 0, 0);
    shaderBlurY.begin();
    shaderBlurY.setUniform1f("blurAmnt", 1.0);
    fboBlurOnePass.draw(0, 0, saveW, saveH);
    shaderBlurY.end();
    fboBlurTwoPass.end();
    
}


//--------------------------------------------------------------
void ofApp::saveFrame(){
    
    // Draw the Registered video texture into a Frame Buffer Object
    //    ofFbo fbo;
    //    fbo.allocate(saveW, saveH, GL_RGBA);
    //    fbo.begin();	//Start drawing into buffer
    //    ofClear(0, 0, 0, 0);    // clear fbo
    ////    float vidW = saveH * w / h;
    ////    float vidH = saveH * 1.05;
    ////    colorTex0.draw((vidW - saveW) / -2 , (vidH - saveH) / -2, vidW, vidH);
    //    // Draw Registered Texture, use Depth as alpha
    //    gr.getRegisteredTexture(process_occlusion).draw(0, 0, saveW, saveH);
    //    fbo.end();
    //   fbo.getTexture().setAlphaMask(depthFbo.getTexture());
    // Set the alpha mask -- no longer needed since we are using a shader
    
    
    // Draw Registered Texture into frameFbo using Depth as alpha shader
    frameFbo.begin();
    ofClear(0, 0, 0, 0);
    alphaShader.begin();    // pass depth fbo to the alpha shader
    //    alphaShader.setUniformTexture("maskTex", depthFbo.getTexture(), 1 );
    alphaShader.setUniformTexture("maskTex", fboBlurTwoPass.getTexture(), 1 );
    gr.getRegisteredTexture(process_occlusion).draw(0, 0, saveW, saveH);
    alphaShader.end();
    frameFbo.end();
    
    
    // Prepare pixels object
    ofPixels pix; // allocate pix
    frameFbo.readToPixels(pix);
    threadRecorder.addFrame(pix);
    
    //  IT WORKS!!! Saves .png sequence with alpha channel
    //    fileName = currentPath + "/gnome_" + ofToString(frameCount, 3, '0') + ".png";
    //    ofSaveImage(pix, fileName, OF_IMAGE_QUALITY_BEST);
    
    //  how to draw into fbo and save fbo to png file....
    //  https://forum.openframeworks.cc/t/convert-int-to-char/1632/7
    //  https://forum.openframeworks.cc/t/saving-frames-with-transparent-background/26363/7
    //  http://openframeworks.cc/documentation/gl/ofFbo/
    
    // Try to simply use depthTex0 as an alpha for colorTex0. Like... draw it onto a 1920x1080 rect?
    // NO, this will not work.
}


//--------------------------------------------------------------
void ofApp::stopRecording(){
    
    recordingState = PAUSED;
    threadRecorder.addFrame();
        // add a blank frame to trigger closeFolder on threadRecorder
        // addFrame() with no image will allocate a 1x1 px black image
}


//--------------------------------------------------------------
void ofApp::defineShaders(){
    
    // Define Shaders - thanks to Yuya Hanai
    #ifdef TARGET_OPENGLES
    alphaShader.load("shadersES2/shader");
    shaderBlurX.load("shadersES2/shaderBlurX");
    shaderBlurY.load("shadersES2/shaderBlurY");
    
    #else
        if(ofIsGLProgrammableRenderer()){
            alphaShader.load("shadersGL3/shader");
            shaderBlurX.load("shadersGL3/shaderBlurX");
            shaderBlurY.load("shadersGL3/shaderBlurY");
        }else{
            alphaShader.load("shadersGL2/shader");
            shaderBlurX.load("shadersGL2/shaderBlurX");
            shaderBlurY.load("shadersGL2/shaderBlurY");
        }
    #endif
    
    depthShader.setupShaderFromSource(GL_FRAGMENT_SHADER, depthFragmentShader);
    depthShader.linkProgram();
    
    irShader.setupShaderFromSource(GL_FRAGMENT_SHADER, irFragmentShader);
    irShader.linkProgram();
    
    
    fboBlurOnePass.allocate(depthW, depthH);
    fboBlurTwoPass.allocate(depthW, depthH);
    
}


//--------------------------------------------------------------
void ofApp::calibrateBackground(){
    grayBg = grayImage;
    calibrate = false;
}


//--------------------------------------------------------------
void ofApp::calculateAlpha(){
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        humanDetected = true;
    }
    if (key =='r') {
        draw_registered = !draw_registered;
    }
    
    if (key == 'd') {
        draw_depth = !draw_depth;
    }
    
    if (key == 'b') {
        draw_blur = !draw_blur;
    }
    
    if (key == 'g') {
        draw_gray = !draw_gray;
    }
    
    if (key == 'c') {
        calibrate = true;
    }
    
    if (key == 'o') {
        process_occlusion = !process_occlusion;
    }
    
    if (key == 'i') {
        draw_ir = !draw_ir;
    }
    
    if (key == 'c') {
        calibrate = true;
    }
    
    if (key == '0') {
        gnomes[0].reset();
    }
    
    if (key == '1') {
        gnomes[1].reset();
    }
    
    if (key == '2') {
        gnomes[2].reset();
    }
    
    if (key == '3') {
        gnomes[3].reset();
    }
    
    if (key == '4') {
        gnomes[4].reset();
    }
    
//    if (key == 'v') {
//        draw_video = !draw_video;
//    }
    
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == ' ') {
        humanDetected = false;
    }
    // This will fail if a viewer jumps - or if they aren't actually on the mat
}


//--------------------------------------------------------------
void ofApp::exit() {
    threadRecorder.waitForThread();
}


