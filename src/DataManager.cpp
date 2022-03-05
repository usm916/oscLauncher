#include "DataManager.h"

void DataManager::setup()
{
	// read config --------------------------------------------------------
	configJson = ofLoadJson("../config.json");
	string cjs = configJson.dump();
	ofLog(OF_LOG_VERBOSE) << "dump : " << cjs;




	for (int s = 0; s < configJson["keyTypes"].size(); s++)
	{
		vKeyTypes.push_back(configJson["keyTypes"][s]["type"].get<string>());
		vKeyDisplayTypeName.push_back(configJson["keyTypes"][s]["display"].get<string>());
		vKeyOpened.push_back(true);
		ofLog(OF_LOG_VERBOSE) << "key : ID" << s << " :: " << vKeyTypes[s] << " / " << vKeyDisplayTypeName.back();
	}

	// read -------------------------------------------------------- Link data
	appListJson = ofLoadJson("../appList2.json");
	//appListJson = ofLoadJson("../testAppList.json");
	cjs = appListJson.dump();
	ofLog(OF_LOG_VERBOSE) << "KeyType : " << vKeyTypes.size();
	ofLog(OF_LOG_VERBOSE) << "dump apps settings : " << cjs;
	int searchCnt = 0;
	for (int s = 0; s < vKeyTypes.size(); s++)
	{
		dataLists.push_back(vector< ofPtr<DataList> >());
		if (s > 0)
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

				// Check data OLD / NEW
				if (appListJson[vKeyTypes[s]][i]["option"].is_null())
				{
					// NEW appList.json reading
					for (int k = 0; k < appListJson[vKeyTypes[s]][i]["data"].size(); k++)
					{
						ofLog(OF_LOG_VERBOSE) << "Multiple Data : ";
						dataLists.back().back()->data.push_back(appListJson[vKeyTypes[s]][i]["data"][k].get<string>());
						keySearchIndex[dataLists.back().back()->keys.back()] = ofToString(s) + "_" + ofToString(i);
					}
					dataLists.back().back()->title = appListJson[vKeyTypes[s]][i]["title"].get<string>();
				}
				else
				{
					// OLD appList.json reading
					dataLists.back().back()->data.push_back("");
					dataLists.back().back()->data.push_back("");
					dataLists.back().back()->data[0] = appListJson[vKeyTypes[s]][i]["data"].get<string>();
					dataLists.back().back()->data[1] = appListJson[vKeyTypes[s]][i]["option"].get<string>();
					dataLists.back().back()->title = "";
				}
			}
		}
		else if (s == 0)
		{
			ofLog() << "CONFIG JSON :: Num of targets - " << configJson["targets"].size();
			for (int i = 0; i < configJson["targets"].size(); i++)
			{
				dataLists.back().push_back(ofPtr<DataList>(new DataList));
				dataLists.back().back()->bEdit = false;
				dataLists.back().back()->type = "network";
				dataLists.back().back()->keys.push_back(configJson["targets"][i]["name"].get<string>());
				dataLists.back().back()->data.push_back("");
				dataLists.back().back()->data.push_back("");
				dataLists.back().back()->data[0] = configJson["targets"][i]["address"].get<string>();
				dataLists.back().back()->data[1] = ofToString(configJson["targets"][i]["port"].get<string>());
				dataLists.back().back()->bEnable = configJson["targets"][i]["enable"].get<bool>();
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
}

int DataManager::findAppList(vector< ofPtr<DataList>> &_lists, string _key, ofPtr<DataList> &_app)
{
	ofLog(OF_LOG_VERBOSE) << "LIST SIZE " << _lists.size();
	for (int i = 0; i < _lists.size(); i++)
	{
		ofPtr<DataList> &list = _lists[i];
		ofLog(OF_LOG_VERBOSE) << "list name = " << list->keys[0] << "   _key = " << _key;
		if (list->keys[0] == _key)
		{
			ofLog(OF_LOG_VERBOSE) << "find key " << list->keys[0];
			_app = list;
			return 1;
		}
	}

	return -1;
}

int DataManager::getDataList(string _key, map<string, string> &_search, vector< vector< ofPtr<DataList> > > &_dataLists, ofPtr<DataList> *_out)
{
	string sID;
	//keySearchIndex
	decltype(_search)::iterator it = _search.find(_key);
	if (it != _search.end())
	{
		sID = it->second;
		ofLogVerbose() << "Key Exists! " << _key << " " << sID;
		vector<string> &vID = ofSplitString(sID, "_");
		_out = &_dataLists[ofToInt(vID[0])][ofToInt(vID[1])];
		return 1;
	}
	else
	{
		ofLogVerbose() << "Dose not Exists! " << _key << " " << sID;
		return 0;
	}
}

void DataManager::queCombination(string _comb)
{
	ofLogVerbose() << "que combination " << _comb;
	ofPtr<DataList> *cDL;
	getDataList(_comb, cDL);
	for (int i = 0; i < cDL->data.size(); i++)
	{
		ofLogVerbose() << "combi -> " << i << " / " << cDL->data[i];
		ofPtr<DataList> tempDL;
		if (getDataList(cDL->data[i], tempDL))
		{
			excuteData(tempDL);
		}
	}
}

size_t DataManager::getSenderSize()
{
	return dataLists[0].size();
}
