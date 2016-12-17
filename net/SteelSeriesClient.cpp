#include "SteelSeriesClient.h"
#include "easylogging++.h"
#include "NetworkThread.h"
#include "StringUtils.h"
#include <sstream>
#include <fstream>

#ifdef _WIN32
	#include <windows.h>
	#include <shlobj.h>		//for knownFolder
	#include <winerror.h>	//for HRESULT
	#include <atlstr.h>		//for CW2A
#endif //_WIN32

using namespace std;

#define SS_FILEPATH "/SteelSeries/SteelSeries Engine 3/coreProps.json"	//Location SteelSeries ini file is relative to %PROGRAMDATA%
#define START_HREF_HTTP "http://"

//URLs for POSTing to SteelSeries methods
#define URL_BIND_EVENT "/bind_game_event"
#define URL_GAME_EVENT "/game_event"
#define URL_HEARTBEAT "/game_heartbeat"
#define URL_REGISTER_APP "/game_metadata"

//JSON keys
#define JSON_KEY_ADDRESS "address"
#define JSON_KEY_DATA "data"
#define JSON_KEY_ENCRYPTED_ADDRESS "encrypted_address"	//Use mebbe at some point? Prolly not
#define JSON_KEY_EVENT "event"
#define JSON_KEY_GAME "game"
#define JSON_KEY_GAME_DISPLAY_NAME "game_display_name"
#define JSON_KEY_ICON_COLOR_ID "icon_color_id"
#define JSON_KEY_TIMEOUT "deinitialize_timer_length_ms"
#define JSON_KEY_VALUE "value"

#define MAX_TIMEOUT_LEN 60000
const float HEARTBEAT_FREQUENCY = (((float)MAX_TIMEOUT_LEN) / (1000.0) - 1.0);
#define ICON_COLOR_BLUE 5

SteelSeriesClient::SteelSeriesClient()
{
	valid = true;
	url = getSSURL();
	if(url.empty())
		valid = false;
	heartbeatTimer = 0;
}

SteelSeriesClient::~SteelSeriesClient()
{
}

string SteelSeriesClient::getSSURL()
{
#ifdef _WIN32
	//Path on Windows is defined by SteelSeries API to be %PROGRAMDATA%/SteelSeries/SteelSeries Engine 3/coreProps.json
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
#else
	#warning TODO Support other OSs for SteelSeries stuff...
        return string();
	//On OSX it'll be at /Library/Application Support/SteelSeries Engine 3/coreProps.json
	//SS has no Linux support yet
#endif	//_WIN32
}

bool SteelSeriesClient::init(std::string appName)
{
	if(!registerApp(StringUtils::normalize(appName), appName))
	{
		valid = false;
		return false;
	}
	return true;
}

void SteelSeriesClient::update(float dt)
{
	if(!valid) 
		return;

	heartbeatTimer += dt;
	if(heartbeatTimer >= HEARTBEAT_FREQUENCY)
	{
		heartbeatTimer = 0;	//Reset

		//Send a new heartbeat message
		heartbeat();
	}
}

bool SteelSeriesClient::registerApp(std::string ID, std::string displayName)
{
	appId = ID;

	rapidjson::Document doc(rapidjson::kObjectType);
	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), doc.GetAllocator());
	doc.AddMember(JSON_KEY_GAME_DISPLAY_NAME, rapidjson::StringRef(displayName.c_str()), doc.GetAllocator());
	doc.AddMember(JSON_KEY_TIMEOUT, MAX_TIMEOUT_LEN, doc.GetAllocator());
	doc.AddMember(JSON_KEY_ICON_COLOR_ID, ICON_COLOR_BLUE, doc.GetAllocator());

	return sendJSON(StringUtils::stringify(doc), URL_REGISTER_APP);
}

bool SteelSeriesClient::sendJSON(std::string stringifiedJSON, const char * endpoint)
{
	if(!valid) 
		return false;

	//Send message to SS
	NetworkThread::NetworkMessage msg;
	msg.data = stringifiedJSON;
	ostringstream ssURL;
	ssURL << url << endpoint;
	msg.url = ssURL.str();

	std::cout << "Sending json to " << ssURL.str() << " : " << endl << stringifiedJSON;

	return NetworkThread::send(msg);
}

void SteelSeriesClient::heartbeat()
{
	rapidjson::Document doc(rapidjson::kObjectType);
	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), doc.GetAllocator());
	string heartbeatJSON = StringUtils::stringify(doc);

	sendJSON(StringUtils::stringify(doc), URL_HEARTBEAT);
}

void SteelSeriesClient::bindEvent(std::string eventJSON)
{
	sendJSON(eventJSON, URL_BIND_EVENT);
}

void SteelSeriesClient::sendEvent(std::string eventId, int value)
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, rapidjson::StringRef(eventId.c_str()), allocator);

	rapidjson::Value data(rapidjson::kObjectType);
	data.AddMember(JSON_KEY_VALUE, value, allocator);
	doc.AddMember(JSON_KEY_DATA, data, allocator);

	sendJSON(StringUtils::stringify(doc), URL_GAME_EVENT);
}


