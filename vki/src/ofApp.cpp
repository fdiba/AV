#include "ofApp.h"

#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424

#define DEPTH_SIZE DEPTH_WIDTH * DEPTH_HEIGHT

#define COLOR_WIDTH 1920
#define COLOR_HEIGHT 1080

int previewWidth = 640;
int previewHeight = 480;

//--------------------------------------------------------------
void ofApp::setup(){

	//ofSetWindowShape(previewWidth * 1.1 , previewHeight * 1.1);
	//ofSetWindowShape(DEPTH_WIDTH * 2, DEPTH_HEIGHT);
	ofSetWindowPosition(2024, 200);

	//------------------------

	m_val = 0;
	m_speed = 5;
	m_maxVal = ofGetHeight();
	//-----------------------

	

	
	ofSetVerticalSync(true);
	//ofBackground(255, 255, 255);
	ofSetLogLevel(OF_LOG_VERBOSE);

	

	

	midiIn.listPorts();
	midiIn.openPort(1);

	// add ofApp as a listener
	midiIn.addListener(this);

	// print received messages to the console
	midiIn.setVerbose(true);

	//thread.startThread(true);    // blocking, non verbose

	useKinect = false;

	if (useKinect) {
		kinect.open();
		kinect.initDepthSource();

		kinect.initColorSource();


		kinect.initInfraredSource();
		kinect.initBodySource();
		kinect.initBodyIndexSource();

		if (kinect.getSensor()->get_CoordinateMapper(&coordinateMapper) < 0) {
			ofLogError() << "Could not acquire CoordinateMapper!";
		}

		numBodiesTracked = 0;
		bHaveAllStreams = false;

		bodyIndexImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
		foregroundImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);

		colorCoords.resize(DEPTH_WIDTH * DEPTH_HEIGHT);


		//--------------------------------------
		depthDisplay.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);

		nearClipping = 50.0;
		farClipping = 2000.0;
		updateRawDepthLookupTable();
	}

	//kinect.initBodySource();
	//kinect.initInfraredSource();

}
//--------------------------------------------------------------
void ofApp::generateDepthDisplayImage() {

	// MAKE SURE WE HAVE A DEPTH SOURCE BEFORE WE ATTEMPT TO MANIPULATE
	if (kinect.getDepthSource()==nullptr) return;

	unsigned short* rawdepth = kinect.getDepthSource()->getPixels();

	int rawindex = 0;
	int displayindex = 0;

	for (int y = 0; y < DEPTH_HEIGHT; y++) {
		for (int x = 0; x < DEPTH_WIDTH; x++) {

			/*if (x == 10) {
				if (rawdepth[rawindex] == 0)std::cout << rawdepth[rawindex] << " foo " << endl;

			}*/

			/*unsigned char greyValue = depthLookupTable[rawdepth[rawindex++]];

			depthDisplay.getPixels()[displayindex++] = greyValue;
			depthDisplay.getPixels()[displayindex++] = greyValue;
			depthDisplay.getPixels()[displayindex++] = greyValue;*/
		}
	}

	depthDisplayImage.setFromPixels(depthDisplay);

}
void ofApp::updateRawDepthLookupTable() {
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

	string str = ofxMidiMessage::getStatusString(midiMessage.status);
	if (str.find("Note Off") == -1)up = true;
	else up = false;

}
//--------------------------------------------------------------
void ofApp::update(){

	if (up && m_val < m_maxVal) {
		m_val+= m_speed;
	}
	else if (!up && m_val>0) {
		m_val-= m_speed;
	}
		
		

	if (useKinect) {
		kinect.update();

		generateDepthDisplayImage();

		//exampleBodyIndexColor();

	}
}
void ofApp::exampleBodyIndexColor() {

	// Get pixel data
	auto& depthPix = kinect.getDepthSource()->getPixels();
	auto& bodyIndexPix = kinect.getBodyIndexSource()->getPixels();
	auto& colorPix = kinect.getColorSource()->getPixels();

	// Make sure there's some data here, otherwise the cam probably isn't ready yet
	if (!depthPix.size() || !bodyIndexPix.size() || !colorPix.size()) {
		bHaveAllStreams = false;
		return;
	}
	else {
		bHaveAllStreams = true;
	}

	// Count number of tracked bodies
	numBodiesTracked = 0;
	auto& bodies = kinect.getBodySource()->getBodies();
	for (auto& body : bodies) {
		if (body.tracked) {
			numBodiesTracked++;
		}
	}

	// Do the depth space -> color space mapping
	// More info here:
	// https://msdn.microsoft.com/en-us/library/windowspreview.kinect.coordinatemapper.mapdepthframetocolorspace.aspx
	// https://msdn.microsoft.com/en-us/library/dn785530.aspx
	coordinateMapper->MapDepthFrameToColorSpace(DEPTH_SIZE, (UINT16*)depthPix.getPixels(), DEPTH_SIZE, (ColorSpacePoint*)colorCoords.data());



	for (int y = 0; y < DEPTH_HEIGHT; y++) {
		for (int x = 0; x < DEPTH_WIDTH; x++) {



			int index = x + y*DEPTH_WIDTH;


			bodyIndexImg.setColor(x, y, ofColor::white);
			foregroundImg.setColor(x, y, ofColor::white);

			// This is the check to see if a given pixel is inside a tracked  body or part of the background.
			// If it's part of a body, the value will be that body's id (0-5), or will > 5 if it's
			// part of the background
			// More info here:
			// https://msdn.microsoft.com/en-us/library/windowspreview.kinect.bodyindexframe.aspx
			float val = bodyIndexPix[index];
			if (val >= bodies.size()) {
				continue;
			}

			// Give each tracked body a color value so we can tell
			// them apart on screen
			ofColor c = ofColor::fromHsb(val * 255 / bodies.size(), 200, 255);
			bodyIndexImg.setColor(x, y, c);

			// For a given (x,y) in the depth image, lets look up where that point would be
			// in the color image
			ofVec2f mappedCoord = colorCoords[index];

			// Mapped x/y coordinates in the color can come out as floats since it's not a 1:1 mapping
			// between depth <-> color spaces i.e. a pixel at (100, 100) in the depth image could map
			// to (405.84637, 238.13828) in color space
			// So round the x/y values down to ints so that we can look up the nearest pixel
			mappedCoord.x = floor(mappedCoord.x);
			mappedCoord.y = floor(mappedCoord.y);

			// Make sure it's within some sane bounds, and skip it otherwise
			if (mappedCoord.x < 0 || mappedCoord.y < 0 || mappedCoord.x >= COLOR_WIDTH || mappedCoord.y >= COLOR_HEIGHT) {
				continue;
			}

			// Finally, pull the color from the color image based on its coords in
			// the depth image
			foregroundImg.setColor(x, y, colorPix.getColor(mappedCoord.x, mappedCoord.y));

		}

	}


	// Update the images since we manipulated the pixels manually. This uploads to the
	// pixel data to the texture on the GPU so it can get drawn to screen
	bodyIndexImg.update();
	foregroundImg.update();

}
//--------------------------------------------------------------
void ofApp::draw(){

	
	if (useKinect) {


		bodyIndexImg.draw(0, 0);
		foregroundImg.draw(DEPTH_WIDTH, 0);

		stringstream ss;
		ss << "fps : " << ofGetFrameRate() << endl;
		ss << "Tracked bodies: " << numBodiesTracked;
		if (!bHaveAllStreams) ss << endl << "Not all streams detected!";
		ofDrawBitmapStringHighlight(ss.str(), 20, 20);

		
	}
	//kinect.getColorSource()->draw(0, 0, 320*m, 180*m);
	//kinect.getInfraredSource()->draw(0, 0, 640*m, 480*m);


	
	if (m_val > 0) {
		ofSetColor(0);
		int r_width = m_val;
		ofDrawRectangle(ofGetWidth()/2-r_width/2, ofGetHeight()/2-r_width/2, r_width, r_width);
	}

	displayMidiInfos();

}

void ofApp::displayMidiInfos() {

	ofSetColor(255, 255, 0);

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

	ofSetColor(255);

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
	//std::cout << 640 * 480 << " foo " << endl;
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
