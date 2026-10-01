#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
//    ofBackground(255);
    takecolor("VPSST_11.jpg");
    ofSetBackgroundColor(0, 0, 0);     //framerate(30);
    
    ofSetFrameRate(25);
    mainOutputSyphonServer.setName("Happy place");
    
    ofxConnexion::start();
    
    // if you want to get data events, you can add a listener to ofxConnexion::connexionEvent
    ofAddListener(ofxConnexion::connexionEvent, this, &ofApp::connexionMoved);
    begin();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(doDollyForwards){
        cam.dolly(-3.5);
    }
    if(doDollyBack){
        cam.dolly(5.5);
    }
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();
    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
    if(ofGetFrameRate() > 20){
        for (int n=0;n<num;n++) {
            move(n);
        }
    }
        
        
        for (int i=0;i<num;i++) {
            glPointSize(2);
            cracks[i].sp.sandMesh.draw();
        }
    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'b'){
        begin();
    }
    if(key == 'd'){
        if (doDollyBack) {
            doDollyBack = false;
        }
        doDollyForwards = true;
    }
    if(key == 'D'){
        if (doDollyForwards) {
            doDollyForwards = false;
        }
        doDollyBack = true;
    }
    if (key == 's') {
        doDollyBack = false;
        doDollyForwards = false;
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

ofColor ofApp::somecolor() {
    // pick some random good color
    int colourSelect = int(ofRandom(0, numpal));
    ofColor outcolour = goodcolor[colourSelect];
    return outcolour;
    
}

void ofApp::takecolor(string fn) {
    ofImage b;
    b.load(fn);
    
    
    for (int x=0;x<b.getWidth();x+=100){
        for (int y=0;y<b.getHeight();y+=50) {
            ofColor c = b.getColor(x, y);
            bool exists = false;
            
            for (int n=0;n<numpal;n++) {
                if (c==goodcolor[n]) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                // add color to pal
                if (numpal<maxpal) {
                    goodcolor[numpal] = c;
                    numpal++;
                }
            }
        }
    }
}

void ofApp::makeCrack() {
    if (num<maxnum) {
        // make a new crack instance
        Crack tempCrack;
        cracks.push_back(tempCrack);
        
        
        //cout<<"making crack " + ofToString(tempColor) << endl;
        cracks[num].initCrack(goodcolor[int(ofRandom(0, numpal))]);
        findStart(num);
        num++;
        // cout << "We have " + ofToString(num) + " Cracks"<< endl;
    }
}


void ofApp::begin() {
    // erase crack grid
    for (int y=0;y<dimensionY;y++) {
        for (int x=0;x<dimensionX;x++) {
            cgrid[y*dimensionX+x] = 10001;
        }
    }
    // make random crack seeds
    for (int k=0;k<16;k++) {
        int i = int(ofRandom(dimensionX*dimensionY-1));
        cgrid[i]=int(ofRandom(360));
    }
    
    // make just three cracks
    num=0;
    cracks.clear();
    for (int i=0;i<num;i++) {
        cracks[i].sp.sandMesh.clear();
        
    }
    
    for (int k=0;k<3;k++) {
        makeCrack();
    }
    //background(255);
}

void ofApp::move(int iter) {
    // continue cracking
    cracks[iter].x+=0.42*cos(cracks[iter].t*PI/180);
    cracks[iter].y+=0.42*sin(cracks[iter].t*PI/180);
    
    // bound check
    float z = 0.33;
    int cx = int(cracks[iter].x+ofRandom(-z,z));  // add fuzz
    int cy = int(cracks[iter].y+ofRandom(-z,z));
    
    // draw sand painter
    regionColor(iter);
    
    // draw black crack
    cracks[iter].crackMesh.addColor(ofColor(0,0,0,85));
    cracks[iter].crackMesh.addVertex(glm::vec3(cracks[iter].x+ofRandom(-z,z),cracks[iter].y+ofRandom(-z,z),0));
    // stroke(0,85);
    //point(x+ofRandom(-z,z),y+ofRandom(-z,z));
    
    
    if ((cx>=0) && (cx<dimensionX) && (cy>=0) && (cy<dimensionY)) {
        // safe to check
        if ((cgrid[cy*dimensionX+cx]>10000) || (abs(cgrid[cy*dimensionX+cx]-cracks[iter].t)<5)) {
            // continue cracking
            cgrid[cy*dimensionX+cx]=int(cracks[iter].t);
        } else if (abs(cgrid[cy*dimensionX+cx]-cracks[iter].t)>2) {
            // crack encountered (not self), stop cracking
            findStart(iter);
            makeCrack();
        }
    } else {
        // out of bounds, stop cracking
        findStart(iter);
        makeCrack();
    }
}

void ofApp::regionColor(int iter) {
    // start checking one step away
    float rx=cracks[iter].x;
    float ry=cracks[iter].y;
    bool openspace=true;
    
    // find extents of open space
    while (openspace) {
        // move perpendicular to crack
        rx+=0.81*sin(cracks[iter].t*PI/180);
        ry-=0.81*cos(cracks[iter].t*PI/180);
        int cx = int(rx);
        int cy = int(ry);
        if ((cx>=0) && (cx<dimensionX) && (cy>=0) && (cy<dimensionY)) {
            // safe to check
            if (cgrid[cy*dimensionX+cx]>10000) {
                // space is open
            } else {
                openspace=false;
            }
        } else {
            openspace=false;
        }
    }
    // draw sand painter
    cracks[iter].sp.render(rx,ry,cracks[iter].x,cracks[iter].y);
}


void ofApp::findStart(int iter) {
    // pick random point
    int px=0;
    int py=0;
    
    // shift until crack is found
    bool found=false;
    int timeout = 0;
    while ((!found) || (timeout++>1000)) {
        px = int(ofRandom(dimensionX));
        py = int(ofRandom(dimensionY));
        if (cgrid[py*dimensionX+px]<10000) {
            found=true;
        }
    }
    
    if (found) {
        // start crack
        int a = cgrid[py*dimensionY+px];
        if (ofRandom(100)<50) {
            a-=90+int(ofRandom(-2.0,2.1));
        } else {
            a+=90+int(ofRandom(-2.0,2.1));
        }
        startCrack(px,py,a, iter);
    } else {
        //println("timeout: "+timeout);
    }
}

void ofApp::startCrack(int X, int Y, int T, int iter) {
    cracks[iter].x=X;
    cracks[iter].y=Y;
    cracks[iter].t=T;//%360;
    cracks[iter].x+=0.61*cos(cracks[iter].t*PI/180);
    cracks[iter].y+=0.61*sin(cracks[iter].t*PI/180);
    
    // cout<<"Starting crack x= " + ofToString(cracks[iter].x) + " y= " + ofToString(cracks[iter].y)<< endl;
}

void ofApp::connexionMoved(ConnexionData& data) {
    // this would be a good place to get data from multiple devices
   // cout << data.getButton(0) << " " << data.getButton(1) << endl;
    
    // use the button state to set the LED
    //ofxConnexion::setLed(data.getButton(0) || data.getButton(1));
    //cam.setPosition(data.position[0]/1000, data.position[1]/1000, data.position[2]/1000);
    if (ofGetElapsedTimeMillis()>2000) {
        cam.truck(data.position[0]/-50.0);
        cam.dolly(data.position[1]/50.0);
        cam.boom(data.position[2]/50.0);
        
        cam.panDeg(data.rotation[2]/500.0);
        cam.tiltDeg(data.rotation[0]/500.0);
        cam.rollDeg(data.rotation[1]/500.0);
        
        if (data.getButton(0)) {
            cam.reset();
        }
    }
        
}

