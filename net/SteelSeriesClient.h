#pragma once
#include "rapidjson/document.h"
#include <string>

class SteelSeriesClient
{
	bool valid;
	float heartbeatTimer;
	std::string url;
	std::string appId;

	std::string getSSURL();	//Get the URL for communicating with SteelSeries devices
	bool registerApp(std::string ID, std::string displayName);		//Registers the application
	bool sendJSON(std::string stringifiedJSON, const char* endpoint);	//Sends this JSON to the specified endpoint
	void heartbeat();												//Call this every so often so that SteelSeries drivers don't drop us

public:
	SteelSeriesClient();
	~SteelSeriesClient();

	bool isValid() { return valid; };	//Call after constructor to determine if the SS engine exists or not

	bool init(std::string appName);	//Register ourselves with SteelSeries with the given app name
	void update(float dt);			//Update (Call every frame)

	void bindEvent(std::string eventJSON);					//Create an event with this (stringified) JSON
	void sendEvent(std::string eventId, int value);			//Send an update to this event

	std::string getAppId() { return appId; }
};
