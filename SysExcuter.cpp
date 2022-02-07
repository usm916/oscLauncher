#include "SysExcuter.h"
/*
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


void SysExcuter::touchUpdate(int touchID, float x, float y, bool bPush)
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

void SysExcuter::initializeTouch()
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
*/