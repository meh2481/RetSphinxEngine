#include "SteelSeriesCommunicator.h"
#include "rapidjson/prettywriter.h"
#include "easylogging++.h"
#include "NetworkThread.h"
#include <sstream>
#include <fstream>
#include <algorithm>
using namespace std;

#define SS_FILEPATH "/SteelSeries/SteelSeries Engine 3/coreProps.json"	//Location SteelSeries ini file is
#define START_HREF_HTTP "http://"

//URLs for POSTing to SteelSeries methods
#define URL_REGISTER_APP "/game_metadata"
#define URL_HEARTBEAT "/game_heartbeat"

//JSON keys
#define JSON_KEY_ADDRESS "address"
#define JSON_KEY_ENCRYPTED_ADDRESS "encrypted_address"	//Use mebbe at some point? Prolly not
#define JSON_KEY_GAME "game"
#define JSON_KEY_GAME_DISPLAY_NAME "game_display_name"
#define JSON_KEY_ICON_COLOR_ID "icon_color_id"
#define JSON_KEY_TIMEOUT "deinitialize_timer_length_ms"

#define MAX_TIMEOUT_LEN 60000
const float HEARTBEAT_FREQUENCY = (((float)MAX_TIMEOUT_LEN) / (1000.0) - 1.0);
#define ICON_COLOR_BLUE 5

SteelSeriesCommunicator::SteelSeriesCommunicator()
{
	url = appId = "";
	heartbeatTimer = 0;
	valid = false;
}

SteelSeriesCommunicator::~SteelSeriesCommunicator()
{
}

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>//for knownFolder
#include <winerror.h> //for HRESULT
#include <atlstr.h>	//for CW2A

//Defined by SteelSeries API to be %PROGRAMDATA%/SteelSeries/SteelSeries Engine 3/coreProps.json
string SteelSeriesCommunicator::getSSURL()
{
	LPWSTR wszPath = NULL;
	HRESULT hr;

	hr = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &wszPath);

	if(SUCCEEDED(hr))
	{
		//Get the actual path to the file
		string strPath = CW2A(wszPath);
		ostringstream oss;
		oss << strPath << SS_FILEPATH;

		//Read the contents of the file
		ifstream jsonFile(oss.str());

		if(!jsonFile.fail())
		{
			ostringstream buffer;
			buffer << jsonFile.rdbuf();

			//Parse the JSON
			rapidjson::Document document;
			if(!document.Parse(buffer.str().c_str()).HasParseError())
			{
				if(document.IsObject())    // Document is a JSON value represents the root of DOM. Root can be either an object or array.
				{
					if(document.HasMember(JSON_KEY_ADDRESS))
					{
						if(document[JSON_KEY_ADDRESS].IsString())
						{
							ostringstream SSURL;
							SSURL << START_HREF_HTTP << document[JSON_KEY_ADDRESS].GetString();
							return SSURL.str();
						}
					}
				}
			}
			else
			{
				LOG(ERROR) << "Unable to parse JSON " << oss.str();
			}
		}
		else
		{
			LOG(ERROR) << "Unable to open file " << oss.str();
		}
	}
	else
	{
		LOG(ERROR) << "SHGetKnownFolderPath() failed; unable to search for SteelSeries JSON file.";
	}
	return string();
}

#else
#error TODO Support other OSs for SteelSeries stuff...
#endif	//_WIN32

bool SteelSeriesCommunicator::init(std::string appName)
{
	url = getSSURL();
	if(url.empty())
	{
		valid = false;
		return false;
	}

	if(!registerApp(normalize(appName), appName)) 
		return false;

	valid = true;
	return true;
}

void SteelSeriesCommunicator::update(float dt)
{
	if(!valid) return;

	heartbeatTimer += dt;
	if(heartbeatTimer >= HEARTBEAT_FREQUENCY)
	{
		heartbeatTimer = 0;	//Reset

		//Send a new heartbeat message
		heartbeat();
	}
}

bool SteelSeriesCommunicator::registerApp(std::string ID, std::string displayName)
{
	appId = ID;

	rapidjson::Document doc;
	doc.SetObject();
	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(ID.c_str()), doc.GetAllocator());
	doc.AddMember(JSON_KEY_GAME_DISPLAY_NAME, rapidjson::StringRef(displayName.c_str()), doc.GetAllocator());
	doc.AddMember(JSON_KEY_TIMEOUT, MAX_TIMEOUT_LEN, doc.GetAllocator());
	doc.AddMember(JSON_KEY_ICON_COLOR_ID, ICON_COLOR_BLUE, doc.GetAllocator());

	std::string stringifiedJSON = stringify(doc);

	LOG(INFO) << "Game init json: " << endl << stringifiedJSON;

	NetworkThread::NetworkMessage msg;
	msg.data = stringifiedJSON;
	ostringstream ssURL;
	ssURL << url << URL_REGISTER_APP;
	msg.url = ssURL.str();
	NetworkThread::send(msg);

	return true;
}

string SteelSeriesCommunicator::normalize(std::string input)
{
	transform(input.begin(), input.end(), input.begin(), ::toupper);
	input.erase(remove_if(input.begin(), input.end(), [](char c) { return !isalpha(c); }), input.end());
	return input;
}

string SteelSeriesCommunicator::stringify(const rapidjson::Document& doc)
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	doc.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
	return sb.GetString();
}

void SteelSeriesCommunicator::heartbeat()
{
	rapidjson::Document doc;
	doc.SetObject();
	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), doc.GetAllocator());
	string heartbeatJSON = stringify(doc);

	LOG(TRACE) << "Sending heartbeat" << endl << heartbeatJSON;

	NetworkThread::NetworkMessage msg;
	msg.data = heartbeatJSON;
	ostringstream ssURL;
	ssURL << url << URL_HEARTBEAT;
	msg.url = ssURL.str();
	NetworkThread::send(msg);
}


