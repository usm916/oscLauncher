#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include <map>

enum EXECTYPE
{
	TYPE_SETTING,
	TYPE_APP,
	TYPE_URL,
	TYPE_FOLDER,
	TYPE_CMD,
	TYPE_TXT,
	TYPE_END
};

class DataList
{
public:
	DataList() {}
	~DataList() {}

	virtual void setup() {}
	virtual void guiPopUp() {}
	virtual void guiDraw() {}

	bool bEdit;
	bool bEnable;
	string title;
	string type;
	vector<string> keys;
	vector<string> data;
	int targetPort;
	int ID;
	int group;
};

class DataManager
{
public:
	DataManager() {}
	~DataManager() {}
	void setup();
	size_t getSenderSize();

	bool bLogNotice;
	int listeningPort;
	int defaultPage;
	int defaultBrowserLocation;

	ofJson appListJson;
	ofJson configJson, keyList;
	vector< vector< ofPtr<DataList> > > dataLists;
	map<string, string> keySearchIndex;
	vector<string> vKeyTypes;
	vector<string> vKeyDisplayTypeName;

	int findAppList(vector<ofPtr<DataList> > &_lists, string _key, ofPtr<DataList> &_app);
	int getDataList(string _key, map<string, string> &_search, vector< vector< ofPtr<DataList> > > &_dataLists, ofPtr<DataList> *_out);
	void queCombination(string _comb);
	vector < bool> vKeyOpened;

};
