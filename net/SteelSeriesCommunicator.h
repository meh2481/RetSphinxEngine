#pragma once
#include <string>


class SteelSeriesCommunicator
{
	bool valid;
	std::string url;

	std::string getSSURL();	//Get the URL for communicating with SteelSeries devices

public:
	SteelSeriesCommunicator();
	~SteelSeriesCommunicator();

	bool init(std::string appName);

	void update(float dt);
};