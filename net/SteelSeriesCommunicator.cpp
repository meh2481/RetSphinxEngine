#include "SteelSeriesCommunicator.h"
#include "rapidjson/document.h"
#include "easylogging++.h"
#include <sstream>
#include <fstream>
using namespace std;

#define SS_FILEPATH "/SteelSeries/SteelSeries Engine 3/coreProps.json"
#define JSON_KEY_ADDRESS "address"
#define JSON_KEY_ENCRYPTED_ADDRESS "encrypted_address"	//Use mebbe at some point? Prolly not
#define START_HREF_HTTP "http://"

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

SteelSeriesCommunicator::SteelSeriesCommunicator()
{
	url = "";
	valid = false;
}

SteelSeriesCommunicator::~SteelSeriesCommunicator()
{
}

bool SteelSeriesCommunicator::init(std::string appName)
{
	url = getSSURL();
	if(url.empty())
	{
		valid = false;
		return false;
	}
	return true;
}

void SteelSeriesCommunicator::update(float dt)
{
	if(!valid) return;


}
