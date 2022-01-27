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

struct DataList
{
	string type;
	string name;
	string data;
	string option;
};

enum EXECTYPE
{
	TYPE_APP,
	TYPE_URL,
	TYPE_FOLDER,
	TYPE_CMD,
	TYPE_TXT,
	TYPE_END
};

enum MUTETYPE
{
	MUTE_SET_FALSE,
	MUTE_SET_TRUE,
	MUTE_TOGGLE,
	MUTE_IGNORE,
	MUTE_GET
};

class ofApp : public ofBaseApp{
	public:
		//ofApp(bool state){}

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

		int findAppList(vector<DataList> &_lists, string _key, DataList &_app);
		void queSystem(vector<DataList> &_lists, string _key, string _cmd = "explorer");
		void stringPast(vector<DataList> &_lists, string _key);

		void applyMessage(vector<string> &_msg);
		void applyKeys(INPUT *_inputs, string _ms, int &_step, bool extended = false);
		void applyModKeys(INPUT *_inputs, string _ms, int &_step, bool _rise = false);
		void modWork(INPUT *_input, string _str, bool _rise = false);
		
		void touchinject(int x, int y, int _bTouch, int _tID);
		bool pTouch;

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
		float px = 0;
		float py = 0;
		int mouseButtonInt = 0;
		string mouseButtonState = "";

		void setClipboardText(const char *text);
		void setDeviceVol(float _vol, EDataFlow _type, MUTETYPE mute = MUTE_IGNORE);
		vector <bool> vMuteState;
		bool bLogNotice = false;
		ofImage receivedImage;

		vector<Target> targetAddress; 

		ofJson appListJson, keyList;
		vector < vector<DataList> > dataLists;
		vector<string> vKeyTypes;
		map<string, WORD> keyPairs;

		string defaultBrowserLocation;
		char buf[1000];
		HWND hWnd;
		ofVec2f startPos;
		vector<float> volCache;

		float pEncoder;
		float sum = 0.0;
		float pEnTime = 0.0;
		int mk1TouchStaet = 0;
		int setcounter = 0;
		bool bPTouch;

		string windowName, pWindowName;
		wstring multi_to_wide_winapi(std::string const& src);
};
