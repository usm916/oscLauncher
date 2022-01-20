#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFrameRate(60);
	//ofSetEscapeQuitsApp(false);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofLogToFile("log.txt");

	appListJson = ofLoadJson("appList.json");
	string cjs = appListJson.dump();
	ofLog() << "dump : " << cjs;

	ofLog() << "num apps " << appListJson["apps"].size();
	for (int i = 0; i < appListJson["apps"].size();i++)
	{
		appsLists.push_back(AppList());
		appsLists.back().type = "app";
		appsLists.back().name = appListJson["apps"][i]["key"].get<string>();
		appsLists.back().location = appListJson["apps"][i]["location"].get<string>();
		appsLists.back().option = appListJson["apps"][i]["option"].get<string>();

	}
	ofLog() << "num folder list " << appsLists.size();
	for (int i = 0; i < appListJson["f"].size(); i++)
	{
		folderLists.push_back(AppList());
		folderLists.back().type = "folder";
		folderLists.back().name = appListJson["f"][i]["key"].get<string>();
		folderLists.back().location = appListJson["f"][i]["location"].get<string>();
		folderLists.back().option = appListJson["f"][i]["option"].get<string>();

	}
	ofLog() << "num folder Lists " << folderLists.size();
	
	ofLog() << "num URL list " << appsLists.size();
	for (int i = 0; i < appListJson["url"].size(); i++)
	{
		urlLists.push_back(AppList());
		urlLists.back().type = "url";
		urlLists.back().name = appListJson["url"][i]["key"].get<string>();
		urlLists.back().location = appListJson["url"][i]["location"].get<string>();
		urlLists.back().option = appListJson["url"][i]["option"].get<string>();

	}
	ofLog() << "num URL Lists " << folderLists.size();

	for (int i = 0; i < appListJson["cmds"].size(); i++)
	{
		cmdsLists.push_back(AppList());
		cmdsLists.back().type = "cmd";
		cmdsLists.back().name = appListJson["cmds"][i]["key"].get<string>();
		cmdsLists.back().location = appListJson["cmds"][i]["location"].get<string>();
		cmdsLists.back().option = appListJson["cmds"][i]["option"].get<string>();
	}
	ofLog() << "num cmdsLists " << cmdsLists.size();

	ofLog() << appListJson["targets"].size();
	for (int i = 0; i < appListJson["targets"].size(); i++)
	{
		targetAddress.push_back(Target());
		targetAddress.back().name = appListJson["targets"][i]["name"].get<string>();
		targetAddress.back().address = appListJson["targets"][i]["address"].get<string>();
		targetAddress.back().targetPort = appListJson["targets"][i]["port"].get<int>();
		targetAddress.back().bEnable = appListJson["targets"][i]["enable"].get<bool>();
		senders.push_back(ofxOscSender());
		senders.back().setup(targetAddress.back().address, targetAddress.back().targetPort);
	}
	ofLog() << "num targetLists " << targetAddress.size();

	listeningPort = appListJson["listenPort"].get<int>();
	defaultPage = appListJson["defaultTabNum"].get<int>();
	defaultBrowserLocation = appListJson["defaultBrowserLocation"].get<string>();
	// listen on the given port
	ofLog() << "listening for osc messages on port " << listeningPort;
	receiver.setup(listeningPort);
}

//--------------------------------------------------------------
void ofApp::update()
{	
	if (ofGetFrameNum() % 10 == 0)
	{
		hWnd = GetForegroundWindow();
		GetWindowText(hWnd, buf, 1000);
		windowName = buf;

		if (windowName != pWindowName)
		{
			ofLog() << "WTXT : " << windowName;
			//ofLog() << "Blender :" << windowName.find("Blender");
			//ofLog() << "Houdini :" << windowName.find("Houdini");
			if (windowName.find("Blender") < 1000000)
			{
				ofxOscMessage m;
				m.setAddress("/pager");
				m.addInt32Arg(3);
				sendOSC(m);
				currentPage = 3;
				ofLog() << "change 2 blender";
			}
			else if (windowName.find("Houdini") < 1000000)
			{
				ofxOscMessage m;
				m.setAddress("/pager");
				m.addInt32Arg(4);
				sendOSC(m);
				currentPage = 4;
				ofLog() << "change 2 HoudiniFX";
			}
			else if (windowName.find("Unreal") < 1000000)
			{
				ofxOscMessage m;
				m.setAddress("/pager");
				m.addInt32Arg(6);
				sendOSC(m);
				currentPage = 6;
				ofLog() << "change 2 Unreal";
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
					ofLog() << "change 2 deafault pageNUum = " << defaultPage;
				}
			}
		}
		pWindowName = windowName;
	}

	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msgStrings[i] = "";
		}
	}

	// check for waiting messages
	while(receiver.hasWaitingMessages()){

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);

		// check for mouse moved message
		string mAdr2;
		string mAdr = mAdr2 = m.getAddress();
		vector<string> &vMsg = ofSplitString(mAdr, "_");

		if(vMsg[0] == "/key"){
			ofLog() << "key : " << mAdr;
			applyMessage(vMsg);
		}
		else if (vMsg[0] == "/app")
		{
			ofLog() << "que ==> " << vMsg[1];
			queSystem(appsLists, vMsg[1]);
		}
		else if (vMsg[0] == "/f")
		{
			ofLog() << "que ==> " << vMsg[1];
			queSystem(folderLists, vMsg[1]);
		}
		else if (vMsg[0] == "/url")
		{
			ofLog() << "que ==> " << vMsg[1];
			queSystem(urlLists, vMsg[1], defaultBrowserLocation);
		}
		else if (vMsg[0] == "/cmd")
		{
			ofLog() << "command : " << mAdr;
			if (vMsg[1] == "qres")
			{
				if (vMsg[2] == "s")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes2048.bat\"");
				if (vMsg[2] == "4k")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes_4K.bat\"");
				if (vMsg[2] == "24")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes_24.bat\"");
			}
		}
		else if (vMsg[0] == "/encoderLR")
		{
		}
		else if (vMsg[0] == "/xy1"|| vMsg[0] == "/group5/1")
		{
			INPUT inputs[1];

			ZeroMemory(inputs, sizeof(inputs));
			
			//float x = 

			inputs[0].type = INPUT_MOUSE;
			inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
			inputs[0].mi.dx = 65536 * (m.getArgAsFloat(0));// -0.5);
			inputs[0].mi.dy = 65536 * (m.getArgAsFloat(1));// -0.5);
			ofLog() << "dx = " << inputs[0].mi.dx;
			ofLog() << "dx = " << inputs[0].mi.dy;
			inputs[0].mi.mouseData = 0;
			ofSleepMillis(5);
		

			UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
		}
		else if (vMsg[0] == "/sys")
		{
			if(vMsg[1] == "snd")
			if (vMsg[2] == "spk")
			{
				setOutputDeviceVol(m.getArgAsFloat(0));
			}
		}
		else if (vMsg[0] == "/ping")
		{
			ofLog() << "ping c : " << ofGetTimestampString();
		}
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

			ofLog() << msgString;

			// clear the next line
			msgStrings[currentMsgString] = "";
		}
	}
}


//--------------------------------------------------------------
void ofApp::draw()
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
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
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
		setOutputDeviceVol(0.4);
	}

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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

void ofApp::applyMessage(vector<string> &_msg)
{
	for (int i = 0; i < _msg.size(); i++)
	{
		printf("%s_", _msg[i]);
	}

	unsigned int numKey = (_msg.size()-1 + _msg[1].length()) * 2;
	INPUT inputs[14];
	
	ZeroMemory(inputs, sizeof(inputs));
	
	int step = 0;
	bool bExtended = false;

	if (numKey == 0)
	{
		bExtended = false;
	}
	else
	{
		bExtended = true;
		applyModKeys(inputs, _msg[1], step);
	}

	applyKeys(inputs, _msg[2], step, bExtended);

	if (numKey == 0)
	{
		bExtended = false;
	}
	else
	{
		applyModKeys(inputs, _msg[1], step, true);
	}

	UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
	if (uSent != ARRAYSIZE(inputs))
	{
		ofLog() << "failed";
	}
}

void ofApp::applyModKeys(INPUT *_inputs, string _msg, int &_step, bool _rise)
{
	string cMsg = _msg;
	if (!_rise)
	{
		ofLog() << "down " << cMsg;
	}
	else
	{
		ofLog() << "UP " << cMsg;
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
		//ofLog() << i <<" : p = " << p;
		if (p!=-1)
		{
			_msg.erase(p, 1);
			//ofLog() << "p=" << p << " step=" << _step << " " << &(_inputs[_step]);
			modWork(&(_inputs[_step]), sid[i], _rise);
			_step++;
		}
	}
}

void ofApp::modWork(INPUT *_input, string _str, bool _rise)
{
	DWORD modType;
	if (_str == "W") { modType = VK_LWIN; }
	else if(_str == "C") { modType = VK_CONTROL; }
	else if (_str == "A") { modType = VK_MENU; }
	else if (_str == "S") { modType = VK_SHIFT; }

	ofLog() << "<mod> modType = 0x" << std::hex << modType << "RISE " << _rise;

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

void ofApp::applyKeys(INPUT *_inputs, string str, int &_step, bool extended)
{
	WORD key = 0;
	if (str.length() == 1)
	{
		//WORD baseKey = 65;
		key = ofToChar(str);
		if (key >= 97)key = key - 32;
	}
	else
	{
		if (str == "esc")key = VK_ESCAPE;
		else if (str == "bs")key = VK_BACK;
		else if (str == "del")key = VK_DELETE;
		else if (str == "tab")key = VK_TAB;
		else if (str == "end")key = VK_END;
		else if (str == "up")key = VK_UP;
		else if (str == "down")key = VK_DOWN;
		else if (str == "left")key = VK_LEFT;
		else if (str == "right")key = VK_RIGHT;
		else if (str == "kanji")key = VK_KANJI;
		else if (str == "pgup")key = VK_PRIOR;
		else if (str == "pgdown")key = VK_NEXT;
		else if (str == "slash")key = VK_DIVIDE;
		else if (str == "asta")key = VK_MULTIPLY;
		else if (str == "plus")key = VK_ADD;
		else if (str == "minus")key = VK_SUBTRACT;
		else if (str == "clear")key = VK_CLEAR;
		else if (str == "enter")key = VK_RETURN;
		else if (str == "period")key = VK_OEM_PERIOD;
		else if (str == "numlock")key = VK_NUMLOCK;
		else if (str == "num0")key = VK_NUMPAD0;
		else if (str == "num1")key = VK_NUMPAD1;
		else if (str == "num2")key = VK_NUMPAD2;
		else if (str == "num3")key = VK_NUMPAD3;
		else if (str == "num4")key = VK_NUMPAD4;
		else if (str == "num5")key = VK_NUMPAD5;
		else if (str == "num6")key = VK_NUMPAD6;
		else if (str == "num7")key = VK_NUMPAD7;
		else if (str == "num8")key = VK_NUMPAD8;
		else if (str == "num9")key = VK_NUMPAD9;
	}
	ofLog() << "=======>> Key = 0x" << std::hex << key;

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

int ofApp::findAppList(vector<AppList> &_lists, string _key, AppList &_app)
{
	ofLog() << "LIST SIZE " << _lists.size();
	for (int i=0;i<_lists.size();i++)
	{
		AppList &list = _lists[i];
		ofLog() << "list name = " << list.name << "   _key = " << _key;
		if (list.name == _key)
		{
			ofLog() << "find key " << list.name;
			_app = list;
			return 1;
		}
	}

	return -1;
}

void ofApp::queSystem(vector<AppList> &_lists, string _key, string _cmd)
{
	ofLog() << "_key " << _key;
	AppList appData;
	int num = findAppList(_lists, _key, appData);
	
	if (num > 0)
	{
		stringstream cmd;
		if (_cmd == "start")
		{
			cmd << _cmd << "" << appData.location;
		}
		else
		{
			cmd << _cmd << " " << "\"" << appData.location << "\"" << " ";
		}
		int a = system(cmd.str().c_str());
		ofLog() << "app name : " << appData.name << " : " << cmd.str().c_str();
	}
}

void ofApp::sendOSC(ofxOscMessage &m)
{
	for (int i = 0; i < senders.size();i++)
	{
		senders[i].sendMessage(m);
	}
}

void ofApp::setOutputDeviceVol(float _vol)
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
		ofLog() << "get failed";
	}
	// IMMDevice out put active devices
	hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
	UINT count, n;
	pDevices->GetCount(&count);	// IMMDevice num
	for (n = 0; n < count; n++)
	{
		PROPVARIANT vName;
		IMMDevice *pEndpoint;
		IPropertyStore *pProperties;
		IAudioEndpointVolume *pAudioEndVol = NULL;
		IAudioInputSelector *pAudioInput_u;
		pDevices->Item(n, &pEndpoint);
		pEndpoint->OpenPropertyStore(STGM_READ, &pProperties);
		PropVariantInit(&vName);
		pProperties->GetValue(PKEY_Device_FriendlyName, &vName);
		wprintf(L"%u : %s ", n, vName.pwszVal);
		PropVariantClear(&vName);
		// create volume objects
		hr = pEndpoint->Activate(__uuidof(IAudioEndpointVolume),
			CLSCTX_ALL, NULL, (void **)&pAudioEndVol);
		if (FAILED(hr))
		{
			if (pDevices)
				pDevices->Release();
			if (pEnum)
				pEnum->Release();
			CoUninitialize();
			ofLog() << "failed to Create volume objects...";
		}
		// getVol
		float getVol;
		hr = pAudioEndVol->GetMasterVolumeLevelScalar(&getVol);
		hr = pAudioEndVol->SetMasterVolumeLevelScalar(_vol, 0);
		if (FAILED(hr))
		{
			if (pAudioEndVol)
				pAudioEndVol->Release();
			if (pDevices)
				pDevices->Release();
			if (pEnum)
				pEnum->Release();
			CoUninitialize();
			ofLog() << "faile to get Volume.";
		}
		printf(" %.2f\n", double(getVol));
		if (pAudioEndVol)
			pAudioEndVol->Release();
	}
	if (pDevices)
		pDevices->Release();
	if (pEnum)
		pEnum->Release();
	
	CoUninitialize();

	//////////////////////////////////////////////////////////////////////
	//IDeviceTopology  *pDevTopoEndpt = NULL;
	//hr = pDevice->Activate(
	//	__uuidof(IDeviceTopology), CLSCTX_ALL, NULL, (void**)&pDevTopoEndpt);

	//// The device topology for an endpoint device always
	//// contains just one connector (connector number 0).
	//IConnector  *pConnEndpt = NULL;
	//hr = pDevTopoEndpt->GetConnector(0, &pConnEndpt);

	//IConnector  *pConnHWDev = NULL;
	//hr = pConnEndpt->GetConnectedTo(&pConnHWDev);

	//// Query the connector in the audio hardware device for
	//// its IPart interface.
	//IPart  *pPartConn = NULL;
	//hr = pConnHWDev->QueryInterface(__uuidof(IPart), (void**)&pPartConn);
}

wstring ofApp::multi_to_wide_winapi(std::string const& src)
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

void ofApp::setInputDeviceVol(float _vol)
{

}


