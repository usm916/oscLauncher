#pragma once

#include "ofMain.h"

enum EXECTYPE
{
	TYPE_APP,
	TYPE_URL,
	TYPE_FOLDER,
	TYPE_CMD,
	TYPE_TXT,
	TYPE_END
};

struct DataList
{
	string type;
	string name;
	string data;
	string option;
};

class KeyManager
{
public:
	KeyManager() {}
	~KeyManager() {}
	/*
	int findAppList(vector<DataList> &_lists, string _key, DataList &_app);
	void queSystem(vector<DataList> &_lists, string _key, string _cmd = "explorer");
	void stringPast(vector<DataList> &_lists, string _key);
	
	// Key things
	void applyMessage(vector<string> &_msg);
	void applyKeys(INPUT *_inputs, string _ms, int &_step, bool extended = false);
	void applyModKeys(INPUT *_inputs, string _ms, int &_step, bool _rise = false);
	void modWork(INPUT *_input, string _str, bool _rise = false);

	ofJson appListJson, keyList;
	vector < vector<DataList> > dataLists;
	vector<string> vKeyTypes;
	map<string, WORD> keyPairs;
	*/
};
