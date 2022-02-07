#pragma once

#include "ofMain.h"

enum MUTETYPE
{
	MUTE_SET_FALSE,
	MUTE_SET_TRUE,
	MUTE_TOGGLE,
	MUTE_IGNORE,
	MUTE_GET
};

class SysExcuter
{
public:
	SysExcuter() {}
	~SysExcuter() {}
	/*
	void setDeviceVol(float _vol, EDataFlow _type, MUTETYPE mute = MUTE_IGNORE);
	void touchUpdate(int touchID, float x, float y, bool bPush);
	void pinchUpdate(float x, bool bPush);

	string windowName, pWindowName;
	wstring multi_to_wide_winapi(std::string const& src);

	int updateTouch = 0;
	float touchScale = 750;
	float chromeBack = 0.0;
	POINT cMSPos;
	vector<ofVec2f> vtPos;
	vector<ofVec2f> vpPos;
	vector<int> vSetCounter;
	vector<bool> vCtouch;
	vector<bool> vPTouch;
	vector<bool> vbTouchDown;
	vector<bool> vbTouchUp;
	vector<POINTER_TOUCH_INFO > vContact;

	// win
	RECT activeRect;
	*/
};

