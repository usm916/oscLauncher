#include "SysExcutor.h"

void SysExcuter::setup()
{
	// for sound state
	vMuteState.push_back(false);
	vMuteState.push_back(false);
	volCache.push_back(0.0);
	volCache.push_back(0.0);

	setDeviceVol(0.0, eCapture, MUTE_GET);

	initializeTouch();
	ofLogVerbose() << "keystate numlock" << GetKeyState(VK_NUMLOCK);
}

int SysExcuter::update()
{
	int out = -999;
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
			bool bYoutube = windowName.find("youtube") < 1000000 ? true : false;
			bool bChrome = windowName.find("Chrome") < 1000000 ? true : false;
			if (bChrome)
			{
				chromeBack = 2.5;
			}
			else
			{
				chromeBack = 1.0;
			}

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
			else if (windowName.find("DaVinci") < 1000000 && !bYoutube && !bChrome)
			{
				ofxOscMessage m;
				m.setAddress("/pager");
				m.addInt32Arg(1);
				sendOSC(m);
				currentPage = 1;
				ofLog(OF_LOG_VERBOSE) << "change 2 DaVinci";
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

	return out;
}
wstring SysExcuter::multi_to_wide_winapi(std::string const& src)
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

void SysExcuter::setDeviceVol(float _vol, EDataFlow _type, MUTETYPE _mute)
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
	int deviceType = (_type == eRender) ? 0 : 1;

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


void SysExcuter::touchinject(int _x, int _y, int _bTouch, int _tID)
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
	if (_bTouch == 1)
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


void SysExcuter::initializeTouch()
{
	int touchNum = 2;
	vContact.clear();
	vContact.reserve(touchNum);

	for (int i = 0; i < touchNum; i++)
	{
		vbTouchDown.push_back(false);
		vbTouchUp.push_back(false);
		vtPos.push_back(ofVec2f(0, 0));
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

void SysExcuter::touchUpdate(int touchID, float x, float y, bool bPush)
{
	ofLog(OF_LOG_VERBOSE) << "touch ID :::: " << touchID;

	touchScale = 700;

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

void SysExcuter::pinchUpdate(float x, bool bPush)
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


