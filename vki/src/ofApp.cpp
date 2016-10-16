#include "ofApp.h"

int previewWidth = 640*1.3;
int previewHeight = 480*1.3;

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(true);
	ofBackground(255, 255, 255);
	ofSetLogLevel(OF_LOG_VERBOSE);

	ofSetWindowShape(previewWidth * 1.1 , previewHeight * 1.1);

	midiIn.listPorts();
	midiIn.openPort(1);

	// add ofApp as a listener
	midiIn.addListener(this);

	// print received messages to the console
	midiIn.setVerbose(true);

	//thread.startThread(true);    // blocking, non verbose

	

	kinect.open();

	//kinect.initDepthSource();
	kinect.initColorSource();
	//kinect.initBodySource();
	kinect.initInfraredSource();

}

//--------------------------------------------------------------
void ofApp::exit() {

	// clean up
	midiIn.closePort();
	midiIn.removeListener(this);
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	// make a copy of the latest message
	midiMessage = msg;
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

	float m = 2;
	//kinect.getDepthSource()->draw(0, 0, previewWidth, previewHeight);
	//kinect.getColorSource()->draw(0, 0, 320*m, 180*m);
	m = 1.2;
	kinect.getInfraredSource()->draw(0, 0, 640*m, 480*m);

	ofSetColor(255);

	// draw the last recieved message contents to the screen
	text << "Received: " << ofxMidiMessage::getStatusString(midiMessage.status);
	ofDrawBitmapString(text.str(), 20, 20);
	text.str(""); // clear

	text << "channel: " << midiMessage.channel;
	ofDrawBitmapString(text.str(), 20, 34);
	text.str(""); // clear

	text << "pitch: " << midiMessage.pitch;
	ofDrawBitmapString(text.str(), 20, 48);
	text.str(""); // clear
	ofDrawRectangle(20, 58, ofMap(midiMessage.pitch, 0, 127, 0, ofGetWidth() - 40), 20);

	text << "velocity: " << midiMessage.velocity;
	ofDrawBitmapString(text.str(), 20, 96);
	text.str(""); // clear
	ofDrawRectangle(20, 105, ofMap(midiMessage.velocity, 0, 127, 0, ofGetWidth() - 40), 20);

	text << "control: " << midiMessage.control;
	ofDrawBitmapString(text.str(), 20, 144);
	text.str(""); // clear
	ofDrawRectangle(20, 154, ofMap(midiMessage.control, 0, 127, 0, ofGetWidth() - 40), 20);

	text << "value: " << midiMessage.value;
	ofDrawBitmapString(text.str(), 20, 192);
	text.str(""); // clear
	if (midiMessage.status == MIDI_PITCH_BEND) {
		ofDrawRectangle(20, 202, ofMap(midiMessage.value, 0, MIDI_MAX_BEND, 0, ofGetWidth() - 40), 20);
	}
	else {
		ofDrawRectangle(20, 202, ofMap(midiMessage.value, 0, 127, 0, ofGetWidth() - 40), 20);
	}

	text << "delta: " << midiMessage.deltatime;
	ofDrawBitmapString(text.str(), 20, 240);
	text.str(""); // clear

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
	std::cout << 640 * 480 << " foo " << endl;
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
