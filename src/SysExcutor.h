#pragma once

#include "ofMain.h"
#include "ofxImGui.h"

#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <stdio.h>
#include <locale.h>
#include <tchar.h>

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
	void setup();
	int update();

	float mouseXf = 0;
	float mouseYf = 0;
	float px = 0;
	float py = 0;
	int mouseButtonInt = 0;
	string mouseButtonState = "";

	void initializeTouch();
	void touchinject(int x, int y, int _bTouch, int _tID);
	void setDeviceVol(float _vol, EDataFlow _type, MUTETYPE mute = MUTE_IGNORE);
	void touchUpdate(int touchID, float x, float y, bool bPush);
	void pinchUpdate(float x, bool bPush);
	wstring multi_to_wide_winapi(std::string const& src);

	bool pTouch;
	vector <bool> vMuteState;
	bool bLogNotice = false;
	ofImage receivedImage;

	string defaultBrowserLocation;
	char buf[1000];
	HWND hWnd;
	ofVec2f startPos;
	vector<float> volCache;

	float pEncoder;
	float sum = 0.0;
	float pEnTime = 0.0;
	int mk1TouchStaet = 0;

	string windowName, pWindowName;

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

};

