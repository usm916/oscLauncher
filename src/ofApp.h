#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <stdio.h>
#include <locale.h>
#include <tchar.h>

// listening port
#define PORT 12233
#define TARGETPORT 12234

// max number of strings to display
#define NUM_MSG_STRINGS 20

struct Target
{
	string name;
	bool bEnable;
	string address;
	unsigned int targetPort;
};

struct AppList
{
	string type;
	string name;
	string location;
	string option;
};

// demonstrates receiving and processing OSC messages with an ofxOscReceiver,
// use in conjunction with the oscSenderExample
class ofApp : public ofBaseApp{
	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void sendOSC(ofxOscMessage &m);

		int findAppList(vector<AppList> &_lists, string _key, AppList &_app);
		void queSystem(vector<AppList> &_lists, string _key, string _cmd = "explorer");

		void applyMessage(vector<string> &_msg);
		void applyKeys(INPUT *_inputs, string _ms, int &_step, bool extended = false);
		void applyModKeys(INPUT *_inputs, string _ms, int &_step, bool _rise = false);
		void modWork(INPUT *_input, string _str, bool _rise = false);
		
		ofTrueTypeFont font;
		ofxOscReceiver receiver;
		unsigned int listeningPort;
		vector<ofxOscSender> senders;

		int currentMsgString;
		int currentPage;
		int defaultPage;
		string msgStrings[NUM_MSG_STRINGS];
		float timers[NUM_MSG_STRINGS];

		float mouseXf = 0;
		float mouseYf = 0;
		int mouseButtonInt = 0;
		string mouseButtonState = "";

		void setOutputDeviceVol(float _vol);
		void setInputDeviceVol(float _vol);

		ofImage receivedImage;

		vector<Target> targetAddress; 

		ofJson appListJson;
		vector<AppList> appsLists;
		vector<AppList> urlLists;
		vector<AppList> folderLists;
		vector<AppList> cmdsLists;

		string defaultBrowserLocation;
		char buf[1000];
		HWND hWnd;

		float pEncoder;
		float sum = 0.0;
		float pEnTime = 0.0;

		string windowName, pWindowName;
		wstring multi_to_wide_winapi(std::string const& src);
};
