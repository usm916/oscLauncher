#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "DataExcutor.h"
#include "DataManager.h"
#include "SysExcutor.h"

// listening port
#define PORT 12233
#define TARGETPORT 12234

// max number of strings to display
#define NUM_MSG_STRINGS 20


enum MUTETYPE
{
	MUTE_SET_FALSE,
	MUTE_SET_TRUE,
	MUTE_TOGGLE,
	MUTE_IGNORE,
	MUTE_GET
};

class OscLauncher : public ofBaseApp{
	public:
		void setup();
		void update();
		void getSenderSize();

		void keyReleased(int key);
		void sendOSC(ofxOscMessage &m);

		void SetNumLock(BOOL bState);

		DataManager dataManager;
		DataExcutor dataExcutor;

		int listeningPort;
		ofTrueTypeFont font;
		ofxOscReceiver receiver;
		vector<ofxOscSender> senders;

		int currentMsgString;
		int currentPage;
		int defaultPage;
		string msgStrings[NUM_MSG_STRINGS];
		float timers[NUM_MSG_STRINGS];

		bool bLogNotice;

};


