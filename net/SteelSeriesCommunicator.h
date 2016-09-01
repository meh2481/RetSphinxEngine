#pragma once
#include "rapidjson/document.h"
#include <string>

class SteelSeriesCommunicator
{
	bool valid;
	float heartbeatTimer;
	std::string url;
	std::string appId;

	std::string getSSURL();	//Get the URL for communicating with SteelSeries devices
	bool registerApp(std::string ID, std::string displayName);		//Registers the application
	std::string normalize(std::string input);						//Normalizes this string to all uppercase, no punctuation
	bool sendJSON(const rapidjson::Document& doc, const char* endpoint);	//Sends this JSON to the specified endpoint

	std::string stringify(const rapidjson::Document& doc);			//Stringifies the passed-in JSON doc
	void heartbeat();												//Call this every so often so that SteelSeries drivers don't drop us

	//TEST
	int lub;
	void bindTestEvent();								//Create lub-dub effect

public:
	SteelSeriesCommunicator();
	~SteelSeriesCommunicator();

	bool init(std::string appName);

	void update(float dt);

	//TEST
	void sendTestEvent();								//Send lub-dub update

	void bindTactileEvent(std::string eventType, std::string eventId, float rumbleFreq, int rumbleCount, int rumbleLen = 100);
	void bindScreenEvent(std::string eventId, int iconId, int ms = 0, std::string prefixText = "", std::string suffixText = "");
	void sendEvent(std::string eventId, int value);
};