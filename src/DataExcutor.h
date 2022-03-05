#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "DataManager.h"

class DataExcutor
{
public:
	DataExcutor() {}
	~DataExcutor() {}

	void excuteData(ofPtr<DataList>);
	void drawEnterGui();
	void drawListGui();

	// excute data
	void excuteMessage(vector<string> &_adrs);
	void queSystem(vector<ofPtr<DataList> > &_lists, string _key, string _cmd = "explorer");
	void stringPast(vector<ofPtr<DataList> > &_lists, string _key);
	void setClipboardText(const char *text);

	void applyMessage(vector<string> &_msg, DWORD _type, bool _down);
	void modWork(string _str, bool _rise);
	void applyKey(string _ms, bool extended, bool _down);
	void applyModKeys(string _ms, bool _down);
	void applyMouse(string _btn, bool _down);

	map<string, WORD> keyPairs;

};
