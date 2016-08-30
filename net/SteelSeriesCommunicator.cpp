#include "SteelSeriesCommunicator.h"
#include "rapidjson/prettywriter.h"
#include "easylogging++.h"
#include "NetworkThread.h"
#include <sstream>
#include <fstream>
#include <algorithm>

//TODO don't have these as global variables; pass them around as needed
extern bool g_fireSSTest;
extern std::string g_eventType;
extern int g_rumbleCount;
extern float g_rumbleFreq;
extern int g_rumbleLen;

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
#define JSON_KEY_DELAY_MS "delay-ms"
#define JSON_KEY_DEVICE_TYPE "device-type"
#define JSON_KEY_ENCRYPTED_ADDRESS "encrypted_address"	//Use mebbe at some point? Prolly not
#define JSON_KEY_EVENT "event"
#define JSON_KEY_FREQUENCY "frequency"
#define JSON_KEY_GAME "game"
#define JSON_KEY_GAME_DISPLAY_NAME "game_display_name"
#define JSON_KEY_HANDLERS "handlers"
#define JSON_KEY_ICON_COLOR_ID "icon_color_id"
#define JSON_KEY_LENGTH_MS "length-ms"
#define JSON_KEY_MODE "mode"
#define JSON_KEY_PATTERN "pattern"
#define JSON_KEY_RATE "rate"
#define JSON_KEY_REPEAT_LIMIT "repeat_limit"
#define JSON_KEY_TIMEOUT "deinitialize_timer_length_ms"
#define JSON_KEY_TYPE "type"
#define JSON_KEY_VALUE "value"
#define JSON_KEY_ZONE "zone"

//String constants for JSON values
#define TYPE_TACTILE "tactile"
#define ZONE_ONE "one"
#define MODE_VIBRATE "vibrate"
#define TYPE_SOFTBUMP_100 "ti_predefined_softbump_100"
#define TYPE_SOFTBUMP_30 "ti_predefined_softbump_30"

#define TEST_EVENT_NAME "TEST_EVENT"

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

string SteelSeriesCommunicator::getSSURL()
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
	#error TODO Support other OSs for SteelSeries stuff...
	//On OSX it'll be at /Library/Application Support/SteelSeries Engine 3/coreProps.json
	//SS has no Linux support yet
#endif	//_WIN32
}

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


	//TEST
	bindTestEvent();
	//sendTestEvent();


	return true;
}

void SteelSeriesCommunicator::update(float dt)
{
	if(!valid) return;

	//TEST
	if(g_fireSSTest)
	{
		static int tmp = 0;
		const char* TEST_EVNT = "TEST_EVNT";
		g_fireSSTest = false;
		bindEvent(g_eventType, TEST_EVNT);
		sendEvent(TEST_EVNT, ++tmp);
	}

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

	rapidjson::Document doc(rapidjson::kObjectType);
	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), doc.GetAllocator());
	doc.AddMember(JSON_KEY_GAME_DISPLAY_NAME, rapidjson::StringRef(displayName.c_str()), doc.GetAllocator());
	doc.AddMember(JSON_KEY_TIMEOUT, MAX_TIMEOUT_LEN, doc.GetAllocator());
	doc.AddMember(JSON_KEY_ICON_COLOR_ID, ICON_COLOR_BLUE, doc.GetAllocator());

	return sendJSON(doc, URL_REGISTER_APP);
}

string SteelSeriesCommunicator::normalize(std::string input)
{
	transform(input.begin(), input.end(), input.begin(), ::toupper);
	input.erase(remove_if(input.begin(), input.end(), [](char c) { return (!isalnum(c) && c != '_'); }), input.end());
	return input;
}

bool SteelSeriesCommunicator::sendJSON(const rapidjson::Document & doc, const char * endpoint)
{
	std::string stringifiedJSON = stringify(doc);

	//Send message to SS
	NetworkThread::NetworkMessage msg;
	msg.data = stringifiedJSON;
	ostringstream ssURL;
	ssURL << url << endpoint;
	msg.url = ssURL.str();

	LOG(INFO) << "Sending json to " << ssURL.str() << " : " << endl << stringifiedJSON;

	return NetworkThread::send(msg);
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
	rapidjson::Document doc(rapidjson::kObjectType);
	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), doc.GetAllocator());
	string heartbeatJSON = stringify(doc);

	sendJSON(doc, URL_HEARTBEAT);
}

void SteelSeriesCommunicator::bindTestEvent()
{
	lub = 0;

	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, TEST_EVENT_NAME, allocator);

	rapidjson::Value handlers(rapidjson::kArrayType);
	rapidjson::Value handler(rapidjson::kObjectType);
	handler.AddMember(JSON_KEY_DEVICE_TYPE, TYPE_TACTILE, allocator);
	handler.AddMember(JSON_KEY_ZONE, ZONE_ONE, allocator);
	handler.AddMember(JSON_KEY_MODE, MODE_VIBRATE, allocator);
	
	//rapidjson::Value rate(rapidjson::kObjectType);
	//rate.AddMember(JSON_KEY_FREQUENCY, 0.65, allocator);
	//rate.AddMember(JSON_KEY_REPEAT_LIMIT, 10, allocator);
	//handler.AddMember(JSON_KEY_RATE, rate, allocator);

	rapidjson::Value patterns(rapidjson::kArrayType);
	rapidjson::Value patternLub(rapidjson::kObjectType);
	patternLub.AddMember(JSON_KEY_TYPE, TYPE_SOFTBUMP_100, allocator);
	patternLub.AddMember(JSON_KEY_DELAY_MS, 250, allocator);
	patterns.PushBack(patternLub, allocator);

	rapidjson::Value patternDub(rapidjson::kObjectType);
	patternDub.AddMember(JSON_KEY_TYPE, TYPE_SOFTBUMP_30, allocator);
	patterns.PushBack(patternDub, allocator);
	handler.AddMember(JSON_KEY_PATTERN, patterns, allocator);

	handlers.PushBack(handler, allocator);
	doc.AddMember(JSON_KEY_HANDLERS, handlers, allocator);

	sendJSON(doc, URL_BIND_EVENT);
}

void SteelSeriesCommunicator::sendTestEvent()
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, TEST_EVENT_NAME, allocator);

	rapidjson::Value data(rapidjson::kObjectType);
	data.AddMember(JSON_KEY_VALUE, lub++, allocator);
	doc.AddMember(JSON_KEY_DATA, data, allocator);

	sendJSON(doc, URL_GAME_EVENT);
}

void SteelSeriesCommunicator::bindEvent(std::string eventType, std::string eventId)
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, rapidjson::StringRef(eventId.c_str()), allocator);

	rapidjson::Value handlers(rapidjson::kArrayType);
	rapidjson::Value handler(rapidjson::kObjectType);
	handler.AddMember(JSON_KEY_DEVICE_TYPE, TYPE_TACTILE, allocator);
	handler.AddMember(JSON_KEY_ZONE, ZONE_ONE, allocator);
	handler.AddMember(JSON_KEY_MODE, MODE_VIBRATE, allocator);

	rapidjson::Value rate(rapidjson::kObjectType);
	rate.AddMember(JSON_KEY_FREQUENCY, g_rumbleFreq, allocator);
	rate.AddMember(JSON_KEY_REPEAT_LIMIT, g_rumbleCount, allocator);
	handler.AddMember(JSON_KEY_RATE, rate, allocator);

	rapidjson::Value patterns(rapidjson::kArrayType);
	rapidjson::Value patternLub(rapidjson::kObjectType);
	patternLub.AddMember(JSON_KEY_TYPE, rapidjson::StringRef(eventType.c_str()), allocator);
	if(eventType == "custom")
		patternLub.AddMember(JSON_KEY_LENGTH_MS, g_rumbleLen, allocator);
	patterns.PushBack(patternLub, allocator);
	handler.AddMember(JSON_KEY_PATTERN, patterns, allocator);

	handlers.PushBack(handler, allocator);
	doc.AddMember(JSON_KEY_HANDLERS, handlers, allocator);

	sendJSON(doc, URL_BIND_EVENT);
}

void SteelSeriesCommunicator::sendEvent(std::string eventId, int value)
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, rapidjson::StringRef(eventId.c_str()), allocator);

	rapidjson::Value data(rapidjson::kObjectType);
	data.AddMember(JSON_KEY_VALUE, value, allocator);
	doc.AddMember(JSON_KEY_DATA, data, allocator);

	sendJSON(doc, URL_GAME_EVENT);
}


