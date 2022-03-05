#include "DataExcutor.h"

void DataExcutor::drawEnterGui()
{

}

void DataExcutor::drawListGui()
{

}

void DataExcutor::excuteMessage(vector<string> &vMsg)
{
	if (vMsg[0] == "/app")
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
	else if (vMsg[0] == "/comb")
	{
		queCombination(vMsg[1]);
	}
	else if (vMsg[0] == "/cmd")
	{
		ofLog(OF_LOG_VERBOSE) << "command : " << mAdr;
		if (vMsg[1] == "qres")
		{
			if (vMsg[2] == "s")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes2048.bat\"");
			if (vMsg[2] == "4k")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes_4K.bat\"");
			if (vMsg[2] == "24")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes_24.bat\"");
			if (vMsg[2] == "22m")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes2266m.bat\"");
			if (vMsg[2] == "17m")int a = system("\"C:\\Windows\\System32\\cmd.exe /c D:\\data\\utilApps\\QRes\\QRes1700m.bat\"");
		}
		else
		{
			queSystem(dataLists[TYPE_CMD], vMsg[1], "");
		}
	}
	else if (vMsg[0] == "/WI")
	{
		bool down = m.getArgAsBool(0);
		vector<string> cvStr;
		cvStr.push_back("");
		cvStr.push_back("W");
		cvStr.push_back("i");
		applyMessage(cvStr, INPUT_KEYBOARD, true);
		ofSleepMillis(10);
		applyMessage(cvStr, INPUT_KEYBOARD, false);
		ofSleepMillis(300);
		for (int i = 0; i < vMsg[1].length(); i++)
		{
			cvStr[1] = "";
			cvStr[2] = vMsg[1].at(i);
			applyMessage(cvStr, INPUT_KEYBOARD, true);
			ofSleepMillis(10);
			applyMessage(cvStr, INPUT_KEYBOARD, false);
		}
		ofSleepMillis(400);
		ofLog(OF_LOG_VERBOSE) << "================================== " << vMsg[0] << " : " << vMsg[1];
		cvStr[2] = ("down");
		applyMessage(cvStr, INPUT_KEYBOARD, true);
		ofSleepMillis(10);
		applyMessage(cvStr, INPUT_KEYBOARD, false);
	}
	else if (vMsg[0] == "/WR")
	{
		bool down = m.getArgAsBool(0);
		vector<string> cvStr;
		cvStr.push_back("");
		cvStr.push_back("W");
		cvStr.push_back("r");
		applyMessage(cvStr, INPUT_KEYBOARD, true);
		ofSleepMillis(50);
		applyMessage(cvStr, INPUT_KEYBOARD, false);
		setClipboardText(vMsg[1].c_str());
		cvStr[1] = "C";
		cvStr[2] = "v";
		ofSleepMillis(50);
		applyMessage(cvStr, INPUT_KEYBOARD, true);
		ofSleepMillis(10);
		applyMessage(cvStr, INPUT_KEYBOARD, false);
		ofLog(OF_LOG_VERBOSE) << "================================== " << vMsg[0] << " : " << vMsg[1];
		cvStr[1] = "";
		cvStr[2] = "return";
		applyMessage(cvStr, INPUT_KEYBOARD, true);
		ofSleepMillis(10);
		applyMessage(cvStr, INPUT_KEYBOARD, false);
	}
	else if (vMsg[0] == "/key") {
		bool down = m.getArgAsBool(0);
		ofLog(OF_LOG_VERBOSE) << "key : " << mAdr;
		applyMessage(vMsg, INPUT_KEYBOARD, down);
	}
	else if (vMsg[0] == "/ms") {
		bool down = m.getArgAsBool(0);
		ofLog(OF_LOG_VERBOSE) << "key : " << mAdr;
		applyMessage(vMsg, INPUT_MOUSE, down);
	}
	else if (vMsg[0] == "/hou")
	{
		vector<string> cvStr;
		cvStr.push_back("");
		cvStr.push_back("");
		cvStr.push_back("tab");
		applyMessage(cvStr, INPUT_KEYBOARD, true);
		ofSleepMillis(10);
		applyMessage(cvStr, INPUT_KEYBOARD, false);
		ofSleepMillis(100);
		for (int i = 0; i < vMsg[1].length(); i++)
		{
			cvStr[2] = vMsg[1].at(i);
			applyMessage(cvStr, INPUT_KEYBOARD, true);
			ofSleepMillis(10);
			applyMessage(cvStr, INPUT_KEYBOARD, false);
		}
		ofLog(OF_LOG_VERBOSE) << "================================== Hou : " << vMsg[1];
		cvStr[2] = ("return");
		applyMessage(cvStr, INPUT_KEYBOARD, true);
		ofSleepMillis(10);
		applyMessage(cvStr, INPUT_KEYBOARD, false);
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
		applyMessage(cvStr, INPUT_KEYBOARD, true);
		ofSleepMillis(10);
		applyMessage(cvStr, INPUT_KEYBOARD, false);
	}
	else
	{

	}

}


void DataExcutor::excuteData(ofPtr<DataList> _data)
{
	////////////////////////////////////////////////////////////////////
	ofLogVerbose() << _data->type << " :: excuteKey( " << " )";
}

void DataExcutor::queSystem(vector< ofPtr<DataList> > &_lists, string _key, string _cmd)
{
	ofLog(OF_LOG_VERBOSE) << "_key " << _key;
	ofPtr<DataList> appData;
	int num = findAppList(_lists, _key, appData);

	if (num > 0)
	{
		stringstream cmd;
		if (_cmd == "start")
		{
			cmd << _cmd << " " << appData->data[0];
		}
		else
		{
			cmd << _cmd << " \"" << appData->data[0] << "\"" << " ";
		}
		int a = system(cmd.str().c_str());
		ofLog(OF_LOG_VERBOSE) << "app name : " << appData->keys[0] << " : " << cmd.str().c_str();
	}
}

void DataExcutor::stringPast(vector< ofPtr<DataList> > &_lists, string _key)
{
	ofLog(OF_LOG_VERBOSE) << "string::_key " << _key;
	ofPtr<DataList> appData;
	int num = findAppList(_lists, _key, appData);

	if (num > 0)
	{
		vector<string> cvStr;
		cvStr.resize(3, "");
		setClipboardText(appData->data[0].c_str());
		cvStr[1] = "C";
		cvStr[2] = "v";
		applyMessage(cvStr, INPUT_KEYBOARD, true);
		ofSleepMillis(5);
		applyMessage(cvStr, INPUT_KEYBOARD, false);
	}
}

void DataExcutor::setClipboardText(const char *text) {
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

void DataExcutor::applyMouse(string _btn, bool _down)
{
	DWORD type;
	INPUT  Input = { 0 };
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;

	if (_btn == "lc")
	{
		if (_down)Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		else	Input.mi.dwFlags = (MOUSEEVENTF_LEFTUP);
	}
	else if (_btn == "rc")
	{
		if (_down)Input.mi.dwFlags = (MOUSEEVENTF_RIGHTDOWN);
		else	Input.mi.dwFlags = (MOUSEEVENTF_RIGHTUP);
	}
	else if (_btn == "mc")
	{
		if (_down)Input.mi.dwFlags = (MOUSEEVENTF_MIDDLEDOWN);
		else	Input.mi.dwFlags = (MOUSEEVENTF_MIDDLEUP);
	}
	else if (_btn == "x1c")
	{
		if (_down)
		{
			Input.mi.dwFlags = MOUSEEVENTF_XDOWN;
			Input.mi.mouseData = XBUTTON1;
		}
		else
		{
			Input.mi.dwFlags = MOUSEEVENTF_XUP;
			Input.mi.mouseData = XBUTTON1;
		}
	}
	else if (_btn == "x2c")
	{
		if (_down)
		{
			Input.mi.dwFlags = MOUSEEVENTF_XDOWN;
			Input.mi.mouseData = XBUTTON2;
		}
		else
		{
			Input.mi.dwFlags = MOUSEEVENTF_XUP;
			Input.mi.mouseData = XBUTTON2;
		}
	}
	else if (_btn == "wheelf")
	{
		Input.mi.dwFlags = MOUSEEVENTF_WHEEL;
		Input.mi.mouseData = WHEEL_DELTA;
		//Input.mi.mouseData = 60;
	}
	else if (_btn == "wheelb")
	{
		Input.mi.dwFlags = MOUSEEVENTF_WHEEL;
		Input.mi.mouseData = -WHEEL_DELTA;
		//Input.mi.mouseData = -60;
	}
	else if (_btn == "wheelhf")
	{
		Input.mi.dwFlags = MOUSEEVENTF_HWHEEL;
		Input.mi.mouseData = WHEEL_DELTA;
		//Input.mi.mouseData = 60;
	}
	else if (_btn == "wheelhb")
	{
		Input.mi.dwFlags = MOUSEEVENTF_HWHEEL;
		Input.mi.mouseData = -WHEEL_DELTA;
		//Input.mi.mouseData = -60;
	}

	::SendInput(1, &Input, sizeof(INPUT));
}

void DataExcutor::applyMessage(vector<string> &_msg, DWORD _type, bool _down)
{
	unsigned int numKey = _msg[1].length();// (_msg.size() - 1 + _msg[1].length()) * 2;

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
	}

	if (_down && bExtended)
	{
		applyModKeys(_msg[1], _down);
		ofLog(OF_LOG_VERBOSE) << "APPLY extender";
	}

	bool forceModUp = false;
	if (_type == INPUT_KEYBOARD)
	{
		applyKey(_msg[2], bExtended, _down);
	}
	else
	{
		applyMouse(_msg[2], _down);
		if (_msg[2] == "wheelf" || _msg[2] == "wheelb" || _msg[2] == "wheelhf" || _msg[2] == "wheelhb")
		{
			forceModUp = true;
		}
	}

	if (!_down && bExtended)
	{
		applyModKeys(_msg[1], _down);
	}

	if (forceModUp && bExtended)
	{
		applyModKeys(_msg[1], false);
	}
}


void DataExcutor::applyModKeys(string _msg, bool _down)
{
	string cMsg = _msg;
	if (_down)
	{
		ofLog(OF_LOG_VERBOSE) << "DOWN " << cMsg;
	}
	else
	{
		ofLog(OF_LOG_VERBOSE) << "UP " << cMsg;
	}

	string sid[4];
	if (!_down)
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
		if (p != -1)
		{
			_msg.erase(p, 1);
			ofLog(OF_LOG_VERBOSE) << i << " : p = " << p;
			modWork(sid[i], _down);
		}
		else
		{
			ofLog(OF_LOG_VERBOSE) << i << " : p = " << p << " " << sid[i];
		}
	}
}

void DataExcutor::modWork(string _str, bool _down)
{
	INPUT  Input = { 0 };
	::ZeroMemory(&Input, sizeof(INPUT));
	DWORD modType;
	if (_str == "W") { modType = VK_LWIN; }
	else if (_str == "C") { modType = VK_CONTROL; }
	else if (_str == "A") { modType = VK_MENU; }
	else if (_str == "S") { modType = VK_SHIFT; }

	ofLog(OF_LOG_VERBOSE) << "<mod> modType = 0x" << std::hex << modType << " DOWN " << _down;

	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = modType;
	if (!_down)
	{
		Input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
	}
	else
	{
		Input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
	}

	::SendInput(1, &Input, sizeof(INPUT));
}

void DataExcutor::applyKey(string str, bool extended, bool _down)
{
	INPUT  Input = { 0 };
	::ZeroMemory(&Input, sizeof(INPUT));
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

	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = key;
	if (extended)
	{
		if (_down)
		{
			Input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
		}
		else
		{
			Input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		}
	}
	else
	{
		if (!_down)
			Input.ki.dwFlags = KEYEVENTF_KEYUP;
	}
	::SendInput(1, &Input, sizeof(INPUT));

}

