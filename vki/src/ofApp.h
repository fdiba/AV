#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxKinectForWindows2.h"

class ofApp : public ofBaseApp, public ofxMidiListener {

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

		void newMidiMessage(ofxMidiMessage& eventArgs);
		void exit();

		void displayMidiInfos();

		stringstream text;

		ofxMidiIn midiIn;
		ofxMidiMessage midiMessage;

		//ofImage img;

		boolean useKinect;
		ofxKFW2::Device kinect;

		//---------------------------
		ICoordinateMapper* coordinateMapper;

		ofImage bodyIndexImg, foregroundImg;
		vector<ofVec2f> colorCoords;
		int numBodiesTracked;
		bool bHaveAllStreams;

		void exampleBodyIndexColor();

		//--------------------------
		float	nearClipping, farClipping;

		void updateRawDepthLookupTable();
		void generateDepthDisplayImage();

		vector<unsigned char>		depthLookupTable;
		ofPixels_<unsigned char>	depthDisplay;
		ofImage						depthDisplayImage;

		//---------------------------

		boolean up;
		int state;
		float m_val, m_maxVal;
		float m_speed;
		
};
