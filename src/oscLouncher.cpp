#include "oscLouncher.h"

//--------------------------------------------------------------
void OscLauncher::setup()
{
	ofSetFrameRate(60);
	ofSetEscapeQuitsApp(false);
	ofLogToFile("../log.txt");
	//ofLogToConsole();
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofLog(OF_LOG_VERBOSE) << "OF_LOG_VERBOSE";

	dataManager.setup();
	bLogNotice = dataManager.configJson["logNotice"].get<bool>();
	listeningPort = dataManager.configJson["listenPort"].get<int>();
	defaultPage = dataManager.configJson["defaultTabNum"].get<int>();
	defaultBrowserLocation = dataManager.configJson["defaultBrowserLocation"].get<string>();
	// listen on the given port
	ofLog() << "listening for osc messages on port " << listeningPort;
	receiver.setup(PORT);

	for (int i = 0; i < dataManager.getSenderSize(); i++)
	{
		vector<ofPtr<DataList> > &dataPool = dataManager.dataLists[0][i];
		senders.push_back(ofxOscSender());
		senders.back().setup(.back()->data[0], ofToInt(dataLists.back().back()->data[1]));
	}
	SetNumLock(TRUE);

}

//--------------------------------------------------------------
void OscLauncher::update()
{	
	// --------------------------------------- send OSC ------------- aliving check
	if (ofGetFrameNum() % 500 == 0)
	{
		setDeviceVol(0.0, eRender, MUTE_GET);
		ofxOscMessage m;
		m.setAddress("/slive");
		m.addBoolArg(vMuteState[0]);
		sendOSC(m);
	}
	else if(ofGetFrameNum() % 500 == 250)
	{
		setDeviceVol(0.0, eCapture, MUTE_GET);
		ofxOscMessage m;
		m.setAddress("/mlive");
		m.addBoolArg(vMuteState[1]);
		sendOSC(m);
	}
	// just alive Connection
	if (ofGetFrameNum() % 300 == 100)
	{
		setDeviceVol(0.0, eCapture, MUTE_GET);
		ofxOscMessage m;
		m.setAddress("/alive");
		m.addBoolArg(vMuteState[1]);
		sendOSC(m);
	}

	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msgStrings[i] = "";
		}
	}

	// ///////////----------------------- /////////////////////////////////  get OSC messages
	bool bA[2];
	GetCursorPos(&cMSPos);
	// reset vlue every frame
	for (int i = 0; i < vbTouchDown.size(); i++)
	{
		vbTouchDown[i] = vbTouchUp[i] = bA[i] = false;
	}
	// check for waiting messages
	while(receiver.hasWaitingMessages())
	{

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);

		// check for mouse moved message
		string mAdr2;
		string mAdr = mAdr2 = m.getAddress();
		vector<string> &vMsg = ofSplitString(mAdr, "_");
		

		if (vMsg[0] == "/sys")
		{
			if (vMsg[1] == "snd")
			{
				if (vMsg[2] == "spk")
				{
					volCache[0] = m.getArgAsFloat(0);
					setDeviceVol(volCache[0], eRender);
				}
				else if (vMsg[2] == "mic")
				{
					volCache[1] = m.getArgAsFloat(0);
					setDeviceVol(volCache[1], eCapture);
				}
				else if (vMsg[2] == "smute")
				{
					bool bmm = m.getArgAsFloat(0);
					MUTETYPE MT = bmm ? MUTE_SET_FALSE : MUTE_SET_TRUE;
					ofLog(OF_LOG_VERBOSE) << "smute " << bmm;
					setDeviceVol(volCache[0], eRender, MT);
					ofxOscMessage m;
					m.setAddress("/slive");
					m.addBoolArg(vMuteState[eRender]);
					sendOSC(m);
				}
				else if (vMsg[2] == "mmute")
				{
					bool bmm = m.getArgAsFloat(0);
					MUTETYPE MT = bmm ? MUTE_SET_FALSE : MUTE_SET_TRUE;
					ofLog(OF_LOG_VERBOSE) << "mmute " << bmm;
					setDeviceVol(volCache[1], eCapture, MT);
					ofxOscMessage m;
					m.setAddress("/mlive");
					m.addBoolArg(vMuteState[eCapture]);
					sendOSC(m);
				}
			}
			else if (vMsg[1] == "touch")
			{
				if (vMsg[2] == "pinch")
				{
					updateTouch++;
					bA[0] = bA[1] = true;
					pinchUpdate(m.getArgAsFloat(0), m.getArgAsBool(2));
				}
				else if (vMsg[2] == "/xy" || vMsg[0] == "/multixy/1")
				{
					updateTouch++;
					int touchID = ofToInt(vMsg[1]);
					bA[touchID] = true;
					touchUpdate(touchID, m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsBool(2));
				}
			}
			else
			{
				ofLog(OF_LOG_VERBOSE) << "unknow device name" << vMsg[2];
			}
		}
		else if (vMsg[0] == "/ping")
		{
			ofLog(OF_LOG_VERBOSE) << "ping c : " << ofGetTimestampString();
		}
		else
		{
			dataExcutor.excuteMessage(vMsg);
		}
	}

	updateTouch = (int)bA[0] + (int)bA[1];
	if (updateTouch)
	{
		ofLog(OF_LOG_VERBOSE) << "update touch injection. num = " << updateTouch;
		InjectTouchInput(updateTouch, &(vContact[0]));
		updateTouch = 0;
	}

}

//--------------------------------------------------------------
void OscLauncher::keyReleased(int key){
	if (key == '1')
	{
		ofxOscMessage m;
		m.setAddress("/pager");
		m.addInt32Arg(0);
		sendOSC(m);
	}
	else if (key == '2')
	{
		ofxOscMessage m;
		m.setAddress("/pager");
		m.addInt32Arg(1);
		sendOSC(m);
	}
	else if (key == '3')
	{
		ofxOscMessage m;
		m.setAddress("/pager");
		m.addInt32Arg(2);
		sendOSC(m);
	}
	else if (key == 'v')
	{
		setDeviceVol(0.4, eRender);
	}
	else if (key == 'm')
	{
		setDeviceVol(0.4, eCapture);
	}
	else if (key == 't')
	{
		//touchinject(640, 480);
	}

}


void OscLauncher::sendOSC(ofxOscMessage &m)
{
	for (int i = 0; i < senders.size();i++)
	{
		if (dataLists[0][i]->bEnable)
		{
			senders[i].sendMessage(m);
		}
	}
}

void OscLauncher::SetNumLock(BOOL bState)
{
	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);
	if ((bState && !(keyState[VK_NUMLOCK] & 1)) ||
		(!bState && (keyState[VK_NUMLOCK] & 1)))
	{
		// Simulate a key press
		keybd_event(VK_NUMLOCK,
			0x45,
			KEYEVENTF_EXTENDEDKEY | 0,
			0);

		// Simulate a key release
		keybd_event(VK_NUMLOCK,
			0x45,
			KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
			0);
	}
}

