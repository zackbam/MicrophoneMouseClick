#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	 
	
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(54, 54, 54);	
	
	// 0 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	
	soundStream.printDeviceList();
	
	//if you want to set a different device id 
	//soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	
	int bufferSize = 256;
	
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	
	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
	scaledVol		= 0.0;

	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
	FILE *input;
	input = fopen("settings.txt", "r");
	threshold = 40.f;
	if (input != NULL) {
		fscanf(input, "%f", &threshold);
		printf("Threshold = %d", (int)threshold);
	}
	MOUSE_STATE = false;
	threshold /= 100.f;
	rightClick = false;
	ofSetWindowShape(ofGetScreenWidth()*0.06f, ofGetScreenHeight()*0.1f);
	ofSetWindowPosition(ofGetScreenWidth()*0.93f, ofGetScreenHeight()*0.86f);
}

//--------------------------------------------------------------

void ofApp::update(){
	//lets scale the vol up to a 0-1 range 
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
	INPUT Inputs[3] = { 0 };
	POINT p;
	GetCursorPos(&p);
	INPUT    Input = { 0 };
	if (scaledVol > threshold && !MOUSE_STATE) {
		// left down 
		Input.type = INPUT_MOUSE;
		if (rightClick) 
			Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		else
			Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		::SendInput(1, &Input, sizeof(INPUT));
		MOUSE_STATE = true;
	}
	else if(scaledVol <= threshold && MOUSE_STATE){
		// left up
		::ZeroMemory(&Input, sizeof(INPUT));
		Input.type = INPUT_MOUSE;
		if (rightClick)
			Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		else
			Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		::SendInput(1, &Input, sizeof(INPUT));
		MOUSE_STATE = false;
	}
	//lets record the volume into an array
	volHistory.push_back( scaledVol );
	//if we are bigger the the size we want to record - lets drop the oldest value
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
	HWND AppWindow = GetActiveWindow();
	SetWindowPos(AppWindow, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
}


//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(100 + MOUSE_STATE * 150, 58, 135);
	ofFill();
	ofDrawCircle(ofGetWindowWidth() / 2.f, ofGetHeight() / 2.f, scaledVol * ofGetWindowWidth());

	ofSetColor(225);
	if (rightClick)
		ofDrawBitmapString("Right Click", ofGetWindowWidth()*0.1, ofGetWindowHeight()*0.5);
	else
		ofDrawBitmapString("Left Click", ofGetWindowWidth()*0.1, ofGetWindowHeight()*0.5);
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){	
	curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;	

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;

		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	
	//this is how we get the mean of rms :) 
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :) 
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.8;
	smoothedVol += 0.2 * curVol;
	
	bufferCounter++;
	
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
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
	rightClick = !rightClick;
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

