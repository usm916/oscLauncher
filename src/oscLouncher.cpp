#include "oscLouncher.h"

//--------------------------------------------------------------
void OscLauncher::setup()
{
	ofSetFrameRate(60);
	ofSetEscapeQuitsApp(false);
	ofLogToFile("../log.txt");

	// read config --------------------------------------------------------
	configJson = ofLoadJson("../config.json");
	string cjs = configJson.dump();
	ofLog(OF_LOG_VERBOSE) << "dump : " << cjs;
	bLogNotice = configJson["logNotice"].get<bool>();
	if (bLogNotice)
	{
		ofSetLogLevel(OF_LOG_NOTICE);
		ofLog(OF_LOG_VERBOSE) << "OF_LOG_NOTICE";
	}
	else
	{
		ofSetLogLevel(OF_LOG_VERBOSE);
		ofLog(OF_LOG_VERBOSE) << "OF_LOG_VERBOSE";
	}
	listeningPort = configJson["listenPort"].get<int>();
	defaultPage = configJson["defaultTabNum"].get<int>();
	defaultBrowserLocation = configJson["defaultBrowserLocation"].get<string>();
	// listen on the given port
	ofLog() << "listening for osc messages on port " << listeningPort;
	receiver.setup(listeningPort);

	for (int s = 0; s < configJson["keyTypes"].size(); s++)
	{
		vKeyTypes.push_back(configJson["keyTypes"][s]["type"].get<string>());
		vKeyDisplayTypeName.push_back(configJson["keyTypes"][s]["display"].get<string>());
		ofLog(OF_LOG_VERBOSE) << "key : ID" << s << " :: " << vKeyTypes.back() << " / " << vKeyDisplayTypeName.back();
	}

	// read -------------------------------------------------------- Link data
	appListJson = ofLoadJson("../appList.json");
	cjs = appListJson.dump();
	ofLog(OF_LOG_VERBOSE) << "KeyType : " << vKeyTypes.size();
	ofLog(OF_LOG_VERBOSE) << "dump apps settings : " << cjs;
	int searchCnt = 0;
	for (int s = 0; s < vKeyTypes.size(); s++)
	{
		dataLists.push_back(vector< ofPtr<DataList> >());
		if (s>0)
		{
			for (int i = 0; i < appListJson[vKeyTypes[s]].size(); i++)
			{
				dataLists.back().push_back(ofPtr<DataList>(new DataList));
				dataLists.back().back()->type = vKeyTypes[s];
				dataLists.back().back()->bEdit = false;
				dataLists.back().back()->bEnable = true;
				for (int k = 0; k < appListJson[vKeyTypes[s]][i]["keys"].size(); k++)
				{
					ofLog(OF_LOG_VERBOSE) << "Multiple keys : ";
					dataLists.back().back()->keys.push_back(appListJson[vKeyTypes[s]][i]["keys"][k].get<string>());
					keySearchIndex[dataLists.back().back()->keys.back()] = ofToString(s) + "_" + ofToString(i);
				}
				dataLists.back().back()->data = appListJson[vKeyTypes[s]][i]["data"].get<string>();
				dataLists.back().back()->option = appListJson[vKeyTypes[s]][i]["option"].get<string>();
			}
		}
		else if(s==0)
		{
			ofLog() << "Num of targets - " << configJson["targets"].size();
			for (int i = 0; i < configJson["targets"].size(); i++)
			{
				dataLists.back().push_back(ofPtr<DataList>(new DataList));
				dataLists.back().back()->bEdit = false;
				dataLists.back().back()->type = "network";
				dataLists.back().back()->keys.push_back(configJson["targets"][i]["name"].get<string>());
				dataLists.back().back()->data = configJson["targets"][i]["address"].get<string>();
				dataLists.back().back()->option = ofToString(configJson["targets"][i]["port"].get<string>());
				dataLists.back().back()->bEnable = configJson["targets"][i]["enable"].get<bool>();
				senders.push_back(ofxOscSender());
				senders.back().setup(dataLists.back().back()->data, ofToInt(dataLists.back().back()->option));
			}
		}
	}
	for (auto itr = keySearchIndex.begin(); itr != keySearchIndex.end(); ++itr) {
		ofLog(OF_LOG_VERBOSE) << "SearchKeys :: key = " << itr->first << ", val = " << hex << itr->second;
	}
	ofLogNotice() << "keys pairs : " << keyPairs.size();

	// ------------------------------------------------------- Keyword to Key mapping settings
	keyList = ofLoadJson("../key.json");
	cjs = keyList.dump();
	ofLog(OF_LOG_VERBOSE) << "pair dump 2 : " << cjs;
	for (int s = 0; s < keyList["pair"].size(); s++)
	{
		string ckey = keyList["pair"][s]["key"].get<string>();
		string chex = keyList["pair"][s]["hex"].get<string>();
		keyPairs[ckey] = ofHexToInt(chex);
		ofLog(OF_LOG_VERBOSE) << ckey << " : 0x" << std::hex << ofHexToInt(chex);
	}
	for (auto itr = keyPairs.begin(); itr != keyPairs.end(); ++itr) {
		ofLog(OF_LOG_VERBOSE) << "key = " << itr->first << ", val = " << hex << itr->second;
	}
	ofLogNotice() << "num of keys : " << keyPairs.size();

	// for sound state
	vMuteState.push_back(false);
	vMuteState.push_back(false);
	volCache.push_back(0.0);
	volCache.push_back(0.0);

	setDeviceVol(0.0, eCapture, MUTE_GET);
	SetNumLock(TRUE);

	initializeTouch();
	ofLogVerbose() << "keystate numlock" << GetKeyState(VK_NUMLOCK);
}

//--------------------------------------------------------------
void OscLauncher::update()
{	
	// -------------------------------------------------------- check window title
	if (ofGetFrameNum() % 10 == 0)
	{
		hWnd = GetForegroundWindow();
		GetWindowText(hWnd, buf, 1000);
		GetWindowRect(hWnd, &activeRect);

		windowName = buf;

		if (windowName != pWindowName)
		{
			ofLog(OF_LOG_VERBOSE) << "WTXT : " << windowName;
			bool bYoutube = windowName.find("youtube") < 1000000 ? true:false;
			bool bChrome = windowName.find("Chrome") < 1000000 ? true:false;
			if (bChrome)
			{
				chromeBack = 2.5;
			}
			else
			{
				chromeBack = 1.0;
			}
			
				//ofLog() << "Blender :" << windowName.find("Blender");
			//ofLog() << "Houdini :" << windowName.find("Houdini");
			if (windowName.find("Blender") < 1000000 && !bYoutube && !bChrome)
			{
				ofxOscMessage m;
				m.setAddress("/pager");
				m.addInt32Arg(2);
				sendOSC(m);
				currentPage = 2;
				ofLog(OF_LOG_VERBOSE) << "change 2 blender";
			}
			else if (windowName.find("Houdini") < 1000000 && !bYoutube && !bChrome)
			{
				ofxOscMessage m;
				m.setAddress("/pager");
				m.addInt32Arg(3);
				sendOSC(m);
				currentPage = 3;
				ofLog(OF_LOG_VERBOSE) << "change 2 HoudiniFX";
			}
			else if (windowName.find("Unreal") < 1000000 && !bYoutube && !bChrome)
			{
				ofxOscMessage m;
				m.setAddress("/pager");
				m.addInt32Arg(5);
				sendOSC(m);
				currentPage = 5;
				ofLog(OF_LOG_VERBOSE) << "change 2 Unreal";
			}
			else
			{
				if (currentPage != 1)
				{
					ofxOscMessage m;
					m.setAddress("/pager");
					m.addInt32Arg(defaultPage);
					sendOSC(m);
					currentPage = defaultPage;
					ofLog(OF_LOG_VERBOSE) << "change 2 deafault pageNum = " << defaultPage;
				}
			}
		}
		pWindowName = windowName;
	}

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

	// ///////////--------------------------------------------------- /////////////////////////////////  get OSC messages
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

		if (vMsg[0] == "/pinch")
		{
			updateTouch++;
			bA[0] = bA[1] = true;
			pinchUpdate(m.getArgAsFloat(0), m.getArgAsBool(2));
		}
		else if (vMsg[0] == "/xy" || vMsg[0] == "/multixy/1")
		{
			updateTouch++;
			int touchID = ofToInt(vMsg[1]);
			bA[touchID] = true;
			touchUpdate(touchID, m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsBool(2));
		}
		else if (vMsg[0] == "/key") {
			ofLog(OF_LOG_VERBOSE) << "key : " << mAdr;
			applyMessage(vMsg);
		}
		else if (vMsg[0] == "/app")
		{
			ofLog(OF_LOG_VERBOSE) << "que ==> " << vMsg[1];
			queSystem(dataLists[TYPE_APP], vMsg[1]);
		}
		else if (vMsg[0] == "/f")
		{
			ofLog(OF_LOG_VERBOSE) << "que ==> " << vMsg[1];
			queSystem(dataLists[TYPE_FOLDER], vMsg[1]);
		}
		else if (vMsg[0] == "/url")
		{
			ofLog(OF_LOG_VERBOSE) << "que ==> " << vMsg[1];
			queSystem(dataLists[TYPE_URL], vMsg[1], defaultBrowserLocation);
		}
		else if (vMsg[0] == "/cmd")
		{
			ofLog(OF_LOG_VERBOSE) << "command : " << mAdr;
			if (vMsg[1] == "qres")
			{
				if (vMsg[2] == "s")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes2048.bat\"");
				if (vMsg[2] == "4k")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes_4K.bat\"");
				if (vMsg[2] == "24")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes_24.bat\"");
			}
			else
			{
				queSystem(dataLists[TYPE_CMD], vMsg[1], "");
			}
		}
		else if (vMsg[0] == "/WI") {
			vector<string> cvStr;
			cvStr.push_back("");
			cvStr.push_back("W");
			cvStr.push_back("i");
			applyMessage(cvStr);
			ofSleepMillis(400);
			for (int i = 0; i < vMsg[1].length(); i++)
			{
				cvStr[1] = "";
				cvStr[2] = vMsg[1].at(i);
				applyMessage(cvStr);
			}
			ofSleepMillis(100);
			ofLog(OF_LOG_VERBOSE) << "================================== " << vMsg[0] << " : " << vMsg[1];
			cvStr[2] = ("down");
			applyMessage(cvStr);
			ofSleepMillis(10);
			applyMessage(cvStr);
		}
		else if (vMsg[0] == "/WR") {
			vector<string> cvStr;
			cvStr.push_back("");
			cvStr.push_back("W");
			cvStr.push_back("r");
			applyMessage(cvStr);
			ofSleepMillis(50);
			setClipboardText(vMsg[1].c_str());
			cvStr[1] = "C";
			cvStr[2] = "v";
			ofSleepMillis(50);
			applyMessage(cvStr);
			ofLog(OF_LOG_VERBOSE) << "================================== " << vMsg[0] << " : " << vMsg[1];
			cvStr[1] = "";
			cvStr[2] = ("return");
			applyMessage(cvStr);
		}
		else if (vMsg[0] == "/encoderLR")
		{
		}
		else if (vMsg[0] == "/hou")
		{
			vector<string> cvStr;
			cvStr.push_back("");
			cvStr.push_back("");
			cvStr.push_back("tab");
			applyMessage(cvStr);
			ofSleepMillis(100);
			for (int i = 0; i < vMsg[1].length(); i++)
			{
				cvStr[2] = vMsg[1].at(i);
				applyMessage(cvStr);
			}
			ofLog(OF_LOG_VERBOSE) << "================================== Hou : " << vMsg[1];
			cvStr[2] = ("return");
			applyMessage(cvStr);
		}
		else if (vMsg[0] == "/txt")
		{
			vector<string> cvStr;
			cvStr.push_back("");
			cvStr.push_back("");
			cvStr.push_back("");
			stringPast(dataLists[TYPE_TXT], vMsg[1]);
			cvStr[1] = "C";
			cvStr[2] = "v";
			applyMessage(cvStr);
		}
		else if (vMsg[0] == "/3/xy/z")
		{
			bool bCtouch = bool(m.getArgAsBool(0));
			mk1TouchStaet = bCtouch;

			// UP
			if (!bCtouch)
			{
				mk1TouchStaet = 0;
				startPos = ofVec2f(0.0, 0.0);
				touchinject(px, py, 0 ,0);
				ofLog(OF_LOG_VERBOSE) << "MK ONE_ xy out :";
			}
		}
		else if (vMsg[0] == "/3/xy")
		{
			float scale = 900;

			POINT cMSPos;
			GetCursorPos(&cMSPos);
			ofVec2f checkPos = ofVec2f(m.getArgAsFloat(0), m.getArgAsFloat(1));
			float cx = cMSPos.x + 2.0 * (scale * (checkPos.x - startPos.x));
			float cy = cMSPos.y + (scale * (checkPos.y - startPos.y));

			if (mk1TouchStaet==1)
			{
				startPos = checkPos;
				cx = cMSPos.x;
				cy = cMSPos.y;
				touchinject(cx, cy, 1, 0);
				ofLog(OF_LOG_VERBOSE) << "MK ONE_ xy In : " << cx << " , " << cy << " : " << mk1TouchStaet;
				mk1TouchStaet += 1;
			}
			touchinject(cx, cy, 2, 0);
			ofLog(OF_LOG_VERBOSE) << "MK ONE_ xy update : " << cx << " , " << cy << " : " << mk1TouchStaet;

			px = cx;
			py = cy;
		}
		else if (vMsg[0] == "/sys")
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
				else
				{
					ofLog(OF_LOG_VERBOSE) << "unknow device name" << vMsg[2];
				}
			}
		}
		else if (vMsg[0] == "/ping")
		{
			ofLog(OF_LOG_VERBOSE) << "ping c : " << ofGetTimestampString();
		}
		else if (vMsg[0] == "/req_spkv")
		{ }
		else if (vMsg[0] == "/req_micv")
		{ }
		else 
		{

			// unrecognized message: display on the bottom of the screen
			string msgString;
			msgString = m.getAddress();
			msgString += " : " + ofGetTimestampString();
			for(size_t i = 0; i < m.getNumArgs(); i++){

				// get the argument type
				msgString += " ";
				msgString += m.getArgTypeName(i);
				msgString += ":";

				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msgString += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msgString += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msgString += m.getArgAsString(i);
				}
				else{
					msgString += "unhandled argument type " + m.getArgTypeName(i);
				}
			}

			// add to the list of strings to display
			msgStrings[currentMsgString] = msgString;
			timers[currentMsgString] = ofGetElapsedTimef() + 5.0f;
			currentMsgString = (currentMsgString + 1) % NUM_MSG_STRINGS;

			ofLog(OF_LOG_VERBOSE) << msgString;

			// clear the next line
			msgStrings[currentMsgString] = "";
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
void OscLauncher::draw()
{
	//ofBackgroundGradient(100, 0);
	//ofDrawCircle(ofRandom(1000), ofRandom(1000), 20);

	//// if image exists, draw it
	//if(receivedImage.getWidth() > 0){
	//	ofSetColor(255);
	//	receivedImage.draw(ofGetWidth()/2 - receivedImage.getWidth()/2,
	//	                   ofGetHeight()/2 - receivedImage.getHeight()/2);
	//}

	//// draw recent unrecognized messages
	//for(int i = 0; i < NUM_MSG_STRINGS; i++){
	//	ofDrawBitmapStringHighlight(msgStrings[i], 10, 40 + 15 * i);
	//}

	//string buf = "listening for osc messages on port " + ofToString(PORT);
	//ofDrawBitmapStringHighlight(buf, 10, 20);

	//// draw mouse state
	//glm::vec3 mouseIn(mouseXf*ofGetWidth(), mouseYf*ofGetHeight(),0);
	//if(mouseButtonInt == 0){
	//	ofSetColor(255);
	//} else{
	//	ofSetColor(ofColor::salmon);
	//}
	//ofDrawCircle(mouseIn, 20);
	//ofDrawBitmapStringHighlight(mouseButtonState, mouseIn);
}

//--------------------------------------------------------------
void OscLauncher::keyPressed(int key){

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

//--------------------------------------------------------------
void OscLauncher::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void OscLauncher::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void OscLauncher::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void OscLauncher::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void OscLauncher::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void OscLauncher::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void OscLauncher::windowResized(int w, int h){

}

//--------------------------------------------------------------
void OscLauncher::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void OscLauncher::dragEvent(ofDragInfo dragInfo){

}

void OscLauncher::applyMessage(vector<string> &_msg)
{
	//for (int i = 0; i < _msg.size(); i++)
	//{
	//	ofLog(OF_LOG_VERBOSE) << _msg[i];
	//}

	unsigned int numKey = _msg[1].length();// (_msg.size() - 1 + _msg[1].length()) * 2;
	//INPUT inputs[2];
	INPUT inputs[14];
	
	ZeroMemory(inputs, sizeof(inputs));
	
	int step = 0;
	bool bExtended = false;

	if (numKey == 0)
	{
		bExtended = false;
		ofLog(OF_LOG_VERBOSE) << "no extender";
	}
	else
	{
		bExtended = true;
		applyModKeys(inputs, _msg[1], step);
		ofLog(OF_LOG_VERBOSE) << "APPLY extender";
	}

	applyKeys(inputs, _msg[2], step, bExtended);

	if (numKey == 0)
	{
		bExtended = false;
		ofLog(OF_LOG_VERBOSE) << "no extender";
	}
	else
	{
		applyModKeys(inputs, _msg[1], step, true);
		ofLog(OF_LOG_VERBOSE) << "APPLY extender";
	}

	UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
	if (uSent != ARRAYSIZE(inputs))
	{
		ofLog(OF_LOG_VERBOSE) << "failed";
	}
}

void OscLauncher::applyModKeys(INPUT *_inputs, string _msg, int &_step, bool _rise)
{
	string cMsg = _msg;
	if (!_rise)
	{
		ofLog(OF_LOG_VERBOSE) << "DOWN " << cMsg;
	}
	else
	{
		ofLog(OF_LOG_VERBOSE) << "UP " << cMsg;
	}

	string sid[4];
	if (_rise)
	{
		sid[3] = "S";
		sid[2] = "W";
		sid[1] = "C";
		sid[0] = "A";
	}
	else
	{
		sid[0] = "S";
		sid[1] = "W";
		sid[2] = "C";
		sid[3] = "A";
	}

	for (int i = 0; i < 4; i++)
	{
		int p = -1;
		p = _msg.find(sid[i]);
		if (p!=-1)
		{
			_msg.erase(p, 1);
			ofLog(OF_LOG_VERBOSE) << "p=" << p << " step=" << _step << " " << &(_inputs[_step]);
			modWork(&(_inputs[_step]), sid[i], _rise);
			_step++;
		}
		else
		{
			ofLog(OF_LOG_VERBOSE) << i << " : p = " << p << " " << sid[i];
		}
	}
}

void OscLauncher::modWork(INPUT *_input, string _str, bool _rise)
{
	DWORD modType;
	if (_str == "W") { modType = VK_LWIN; }
	else if(_str == "C") { modType = VK_CONTROL; }
	else if (_str == "A") { modType = VK_MENU; }
	else if (_str == "S") { modType = VK_SHIFT; }

	ofLog(OF_LOG_VERBOSE) << "<mod> modType = 0x" << std::hex << modType << "RISE " << _rise;

	_input->type = INPUT_KEYBOARD;
	_input->ki.wVk = modType;
	if (_rise)
	{
		_input->ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
	}
	else
	{
		_input->ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
	}
	ofSleepMillis(5);
}

void OscLauncher::applyKeys(INPUT *_inputs, string str, int &_step, bool extended)
{
	WORD key = 0;
	if (str.length() == 1)
	{
		/*
		* VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
		* 0x3A - 0x40 : unassigned
		* VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
		*/
		//WORD baseKey = 65;
		key = ofToChar(str);
		if (key >= 95)key = key - 32;
	}
	else
	{
		auto itr = keyPairs.find(str);
		if (itr != keyPairs.end())
		{
			ofLog(OF_LOG_VERBOSE) << "key from map :: " << itr->first;
			key = itr->second;
		}
		else
		{
			ofLog(OF_LOG_VERBOSE) << "key not found.";
		}
	}
	ofLog(OF_LOG_VERBOSE) << "=======>> Key HEX = 0x" << std::hex << key;

	_inputs[_step].type = INPUT_KEYBOARD;
	_inputs[_step].ki.wVk = key;
	if (extended)
	{
		_inputs[_step].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
	}
	_step++;

	ofSleepMillis(5);

	_inputs[_step].type = INPUT_KEYBOARD;
	_inputs[_step].ki.wVk = key;
	if (extended)
	{
		_inputs[_step].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
	}
	else
	{
		_inputs[_step].ki.dwFlags = KEYEVENTF_KEYUP;
	}
	_step++;
	ofSleepMillis(5);
}

int OscLauncher::findAppList(vector< ofPtr<DataList>> &_lists, string _key, ofPtr<DataList> &_app)
{
	ofLog(OF_LOG_VERBOSE) << "LIST SIZE " << _lists.size();
	for (int i=0;i<_lists.size();i++)
	{
		ofPtr<DataList> &list = _lists[i];
		ofLog(OF_LOG_VERBOSE) << "list name = " << list->keys[0]<< "   _key = " << _key;
		if (list->keys[0] == _key)
		{
			ofLog(OF_LOG_VERBOSE) << "find key " << list->keys[0];
			_app = list;
			return 1;
		}
	}

	return -1;
}

void OscLauncher::queSystem(vector< ofPtr<DataList> > &_lists, string _key, string _cmd)
{
	ofLog(OF_LOG_VERBOSE) << "_key " << _key;
	ofPtr<DataList> appData;
	int num = findAppList(_lists, _key, appData);
	
	if (num > 0)
	{
		stringstream cmd;
		if (_cmd == "start")
		{
			cmd << _cmd << " " << appData->data;
		}
		else
		{
			cmd << _cmd << " \"" << appData->data << "\"" << " ";
		}
		int a = system(cmd.str().c_str());
		ofLog(OF_LOG_VERBOSE) << "app name : " << appData->keys[0] << " : " << cmd.str().c_str();
	}
}

void OscLauncher::stringPast(vector< ofPtr<DataList> > &_lists, string _key)
{
	ofLog(OF_LOG_VERBOSE) << "string::_key " << _key;
	ofPtr<DataList> appData;
	int num = findAppList(_lists, _key, appData);

	if (num > 0)
	{
		vector<string> cvStr;
		cvStr.resize(3,"");
		setClipboardText(appData->data.c_str());
		cvStr[1] = "C";
		cvStr[2] = "v";
		applyMessage(cvStr);
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

wstring OscLauncher::multi_to_wide_winapi(std::string const& src)
{
	auto const dest_size = ::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, nullptr, 0U);
	std::vector<wchar_t> dest(dest_size, L'\0');
	if (::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, dest.data(), dest.size()) == 0)
	{
		throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
	}
	dest.resize(std::char_traits<wchar_t>::length(dest.data()));
	dest.shrink_to_fit();
	return std::wstring(dest.begin(), dest.end());
}

void OscLauncher::setDeviceVol(float _vol, EDataFlow _type, MUTETYPE _mute)
{
	HRESULT hr;
	IMMDeviceEnumerator *pEnum = NULL;
	IMMDeviceCollection *pDevices = NULL;

	//// UNICODE set input locales
	setlocale(LC_ALL, "");
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	// MMDevice
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_ALL, IID_PPV_ARGS(&pEnum));
	if (FAILED(hr))
	{
		CoUninitialize();
		ofLog(OF_LOG_VERBOSE) << "get failed";
	}
	// IMMDevice out put active devices
	hr = pEnum->EnumAudioEndpoints(_type, DEVICE_STATE_ACTIVE, &pDevices);
	UINT count, n;
	pDevices->GetCount(&count);	// IMMDevice num
	int deviceType = (_type == eRender)? 0 : 1;

	for (n = 0; n < count; n++)
	{
		PROPVARIANT vName;
		IMMDevice *pEndpoint;
		IPropertyStore *pProperties;
		IAudioEndpointVolume *pAudioEndVol = NULL;

		pDevices->Item(n, &pEndpoint);
		pEndpoint->OpenPropertyStore(STGM_READ, &pProperties);
		PropVariantInit(&vName);
		pProperties->GetValue(PKEY_Device_FriendlyName, &vName);
		//wprintf(L"%u : %s ", n, vName.pwszVal);
		PropVariantClear(&vName);
		// create volume objects
		hr = pEndpoint->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void **)&pAudioEndVol);
		if (FAILED(hr))
		{
			if (pDevices)
				pDevices->Release();
			if (pEnum)
				pEnum->Release();
			CoUninitialize();
			ofLog() << "failed to Create volume objects...";
		}

		float getVol;
		hr = pAudioEndVol->GetMasterVolumeLevelScalar(&getVol);

		// mute
		if (_mute != MUTE_IGNORE)
		{
			BOOL pbGetMute;
			hr = pAudioEndVol->GetMute(&pbGetMute);
			vMuteState[deviceType] = pbGetMute;
			if (_mute == MUTE_GET)
			{
				vMuteState[deviceType] = pbGetMute;
			}
			else if (_mute == MUTE_SET_TRUE)
			{
				cout << "set mute true" << endl;
				vMuteState[deviceType] = true;
				pAudioEndVol->SetMute(vMuteState[deviceType], NULL);
			}
			else if (_mute == MUTE_SET_FALSE)
			{
				cout << "set mute false" << endl;
				vMuteState[deviceType] = false;
				pAudioEndVol->SetMute(vMuteState[deviceType], NULL);
			}
			else
			{
				vMuteState[deviceType] = !pbGetMute;
				pAudioEndVol->SetMute(vMuteState[deviceType], NULL);
				ofLog(OF_LOG_VERBOSE) << "mute state : devType : " << deviceType << " : " << vMuteState[deviceType];
			}
			//ofLog(OF_LOG_VERBOSE) << "Type" << _type << " : Mute state       : " << pbGetMute;
			//ofLog(OF_LOG_VERBOSE) << "Type" << _type << " : Mute state Cache : " << vMuteState[deviceType];
		}

		// set Volume
		if (_mute == MUTE_IGNORE || _mute != MUTE_GET)
		{
			volCache[deviceType] = _vol;
			hr = pAudioEndVol->SetMasterVolumeLevelScalar(_vol, 0);
		}
		if (FAILED(hr))
		{
			if (pAudioEndVol)
				pAudioEndVol->Release();
			if (pDevices)
				pDevices->Release();
			if (pEnum)
				pEnum->Release();
			CoUninitialize();
			ofLog(OF_LOG_VERBOSE) << "faile to get Volume.";
		}
		ofLog(OF_LOG_VERBOSE) << "VOL -- " << double(getVol);

		if (pAudioEndVol)
			pAudioEndVol->Release();
	}
	if (pDevices)
		pDevices->Release();
	if (pEnum)
		pEnum->Release();

	CoUninitialize();

}

void OscLauncher::setClipboardText(const char *text) {
	int size = ::MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
	if (size < 0) {
		return;
	}

	if (::OpenClipboard(NULL)) {
		::EmptyClipboard();
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, (size + 1) * sizeof(WCHAR));
		if (hGlobal != NULL) {
			LPWSTR lpszData = (LPWSTR)::GlobalLock(hGlobal);
			if (lpszData != nullptr) {
				::MultiByteToWideChar(CP_UTF8, 0, text, -1, lpszData, size);
				::GlobalUnlock(hGlobal);
				::SetClipboardData(CF_UNICODETEXT, hGlobal);
			}
		}
		::CloseClipboard();
	}
}


void OscLauncher::touchinject(int _x, int _y, int _bTouch, int _tID)
{
	POINTER_TOUCH_INFO *contact = &(vContact[_tID]);
	BOOL bRet = TRUE;

	//
	// assume a maximum of 10 contacts and turn touch feedback off
	//
	//if(_bTouch==1)
	//	InitializeTouchInjection(2, TOUCH_FEEDBACK_DEFAULT);

	//
	// initialize the touch info structure
	//
	ofLog(OF_LOG_VERBOSE) << _tID;
	contact->pointerInfo.pointerType = PT_TOUCH; //we're sending touch input
	contact->pointerInfo.pointerId = _tID;          //contact 0
	contact->pointerInfo.ptPixelLocation.x = _x;
	contact->pointerInfo.ptPixelLocation.y = _y;

	//contact.pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
	if (_bTouch==1)
	{
		contact->pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
	}
	else if (_bTouch == 2)
	{
		contact->pointerInfo.pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
	}
	else
	{
		vbTouchDown[_tID] = true;
		contact->pointerInfo.pointerFlags = POINTER_FLAG_UP | POINTER_FLAG_INRANGE;
	}
	contact->touchFlags = TOUCH_FLAG_NONE;
	contact->touchMask = TOUCH_MASK_CONTACTAREA | TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE;
	contact->orientation = 90;
	contact->pressure = 32000;

	//
	// set the contact area depending on thickness
	//
	int margin = 5;
	contact->rcContact.top = _x - margin;
	contact->rcContact.bottom = _x + margin;
	contact->rcContact.left = _y - margin;
	contact->rcContact.right = _y + margin;

	//
	// inject a touch down
	//
	//InjectTouchInput(1, contact);
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



void OscLauncher::initializeTouch()
{
	int touchNum = 2;
	vContact.clear();
	vContact.reserve(touchNum);

	for (int i = 0; i < touchNum; i++)
	{
		vbTouchDown.push_back(false);
		vbTouchUp.push_back(false);
		vtPos.push_back(ofVec2f(0,0));
		vpPos.push_back(ofVec2f(0, 0));
		vSetCounter.push_back(0);
		vCtouch.push_back(false);
		vPTouch.push_back(false);
		vContact.push_back(POINTER_TOUCH_INFO());

		//
		memset(&(vContact.back()), 0, sizeof(POINTER_TOUCH_INFO));
		vContact.back().pointerInfo.pointerType = PT_TOUCH;
		vContact.back().pointerInfo.pointerId = i;
		vContact.back().touchFlags = TOUCH_FLAG_NONE;
		vContact.back().touchMask = TOUCH_MASK_CONTACTAREA | TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE;
		vContact.back().pointerInfo.ptPixelLocation.x = 100;
		vContact.back().pointerInfo.ptPixelLocation.y = 100;
	}
	ofLog(OF_LOG_VERBOSE) << vContact.size();

	InitializeTouchInjection(touchNum, TOUCH_FEEDBACK_DEFAULT);
}

void OscLauncher::touchUpdate(int touchID, float x, float y, bool bPush)
{
	ofLog(OF_LOG_VERBOSE) << "touch ID :::: " << touchID;
	//			ofLog() << "MS pos " <<  << " " << cMSPos.x << " : " << cMSPos.y;
	touchScale = 700;
	//vtPos[touchID].x = cMSPos.x + 2.0 * touchScale * (x - 0.5);
	//vtPos[touchID].y = cMSPos.y + touchScale * (y - 0.5);
	float cx = 0.5 * (activeRect.left + activeRect.right);
	float cy = 0.5 * (activeRect.bottom + activeRect.top);
	float cw = activeRect.right - activeRect.left;
	float ch = activeRect.bottom - activeRect.top;
	ofLog(OF_LOG_VERBOSE) << "cx : cy " << cx << " , " << cy << " : " << activeRect.right - activeRect.left << " , " << activeRect.bottom - activeRect.top;
	vtPos[touchID].x = cx + chromeBack * touchScale * (x - 0.5);
	vtPos[touchID].y = cy + touchScale * (y - 0.5);
	vCtouch[touchID] = bPush;
	if (vSetCounter[touchID] % 2 == 1)
	{
		if (vSetCounter[touchID] == 3)
		{
			// Push
			vbTouchDown[touchID] = true;
			touchinject(vtPos[touchID].x, vtPos[touchID].y, 1, touchID);
			//ofLog() << "MK ONE_ xy In : " << cx << " , " << cy << " : " << mk1TouchStaet;
		}
		else if (vSetCounter[touchID] > 3 && !vbTouchDown[touchID])
		{
			// Update
			touchinject(vtPos[touchID].x, vtPos[touchID].y, 2, touchID);
			//ofLog() << "MK ONE_ xy update : " << cx << " , " << cy << " : " << mk1TouchStaet;
		}
		vpPos[touchID] = vtPos[touchID];
		vSetCounter[touchID]++;
	}
	else
	{
		vSetCounter[touchID]++;
	}

	if (vPTouch[touchID] != vCtouch[touchID] && !vCtouch[touchID])
	{
		// Release
		touchinject(vtPos[touchID].x, vtPos[touchID].y, 0, touchID);
		//ofLog() << "MK ONE_ xy OUT : " << cx << " , " << cy << " : " << mk1TouchStaet;
		vSetCounter[touchID] = 0;
	}
	vPTouch[touchID] = vCtouch[touchID];

}

void OscLauncher::pinchUpdate( float x, bool bPush)
{
	ofLog(OF_LOG_VERBOSE) << "pinch :::: ";
	//			ofLog() << "MS pos " <<  << " " << cMSPos.x << " : " << cMSPos.y;
	for (int i = 0; i < 2; i++)
	{
		float dir = -1.0 + float(i) * 2.0;
		touchScale = 600;
		int touchID = i;
		float cx = 0.5 * (activeRect.left + activeRect.right);
		float cy = 0.5 * (activeRect.bottom + activeRect.top);

		vtPos[touchID].x = cx + touchScale * x * dir;
		vtPos[touchID].y = cy;
		vCtouch[touchID] = bPush;
		if (vSetCounter[touchID] % 2 == 1)
		{
			if (vSetCounter[touchID] == 3)
			{
				// Push
				touchinject(vtPos[touchID].x, vtPos[touchID].y, 1, touchID);
				//ofLog() << "MK ONE_ xy In : " << cx << " , " << cy << " : " << mk1TouchStaet;
			}
			else if (vSetCounter[touchID] > 3)
			{
				// Update
				touchinject(vtPos[touchID].x, vtPos[touchID].y, 2, touchID);
				//ofLog() << "MK ONE_ xy update : " << cx << " , " << cy << " : " << mk1TouchStaet;
			}
			vpPos[touchID] = vtPos[touchID];
			vSetCounter[touchID]++;
		}
		else
		{
			vSetCounter[touchID]++;
		}

		if (vPTouch[touchID] != vCtouch[touchID] && !vCtouch[touchID])
		{
			// Release
			touchinject(vtPos[touchID].x, vtPos[touchID].y, 0, touchID);
			//ofLog() << "MK ONE_ xy OUT : " << cx << " , " << cy << " : " << mk1TouchStaet;
			vSetCounter[touchID] = 0;
		}
		vPTouch[touchID] = vCtouch[touchID];
	}
}
