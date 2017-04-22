#include "SteelSeriesHaptic.h"
#include "SteelSeriesClient.h"
#include "SteelSeriesEvents.h"
#include "StringUtils.h"

#define TEST_EVNT "MOUSE_RUMBLE"
#define CLICK_10MS_100 "MOUSE_CLICK_10MS_100"	//1
#define CLICK_10MS_60 "MOUSE_CLICK_10MS_60"		//2
#define CLICK_10MS_30 "MOUSE_CLICK_10MS_30"		//3
#define TICK_10MS_100 "MOUSE_TICK_10MS_100"		//24
#define TICK_10MS_60 "MOUSE_TICK_10MS_60"		//25
#define TICK_10MS_30 "MOUSE_TICK_10MS_30"		//26
#define BUMP_20MS_100 "MOUSE_BUMP_20MS_100"		//7
#define BUMP_20MS_60 "MOUSE_BUMP_20MS_60"		//8
#define BUMP_20MS_30 "MOUSE_BUMP_20MS_30"		//9
#define CLICK_20MS_100 "MOUSE_CLICK_20MS_100"	//21
#define CLICK_20MS_80 "MOUSE_CLICK_20MS_80"		//22
#define CLICK_20MS_60 "MOUSE_CLICK_20MS_60"		//23


SteelSeriesHaptic::SteelSeriesHaptic(SteelSeriesClient * client)
{
	ssClient = client;
}

SteelSeriesHaptic::~SteelSeriesHaptic()
{
}

void SteelSeriesHaptic::init()
{
}

bool SteelSeriesHaptic::isValid()
 { 
	 return ssClient != NULL && ssClient->isValid();
 }

void SteelSeriesHaptic::rumble(float strength, uint32_t duration, float curTime)
{
	static int eventVal = 0;
	static float fLastRumble = 0.0f;

	float sec = (float)duration / 1000.0;

	//Last rumble is still going or 0-msec duration
	if(curTime < fLastRumble || duration < 1)
		return;

	fLastRumble = curTime + sec;

	bindEvent(TEST_EVNT, EVENT_TYPE_CUSTOM, duration);
	ssClient->sendEvent(TEST_EVNT, ++eventVal);
}

void SteelSeriesHaptic::bindEvent(std::string eventId, std::string eventType, uint32_t rumbleLen)
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	std::string appId = ssClient->getAppId();
	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appId.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, rapidjson::StringRef(eventId.c_str()), allocator);

	rapidjson::Value handlers(rapidjson::kArrayType);
	rapidjson::Value handler(rapidjson::kObjectType);
	handler.AddMember(JSON_KEY_DEVICE_TYPE, TYPE_TACTILE, allocator);
	handler.AddMember(JSON_KEY_ZONE, ZONE_ONE, allocator);
	handler.AddMember(JSON_KEY_MODE, MODE_VIBRATE, allocator);

	rapidjson::Value rate(rapidjson::kObjectType);
	rate.AddMember(JSON_KEY_REPEAT_LIMIT, 1, allocator);	//Repeat once (as in, only play effect once)
	handler.AddMember(JSON_KEY_RATE, rate, allocator);

	rapidjson::Value patterns(rapidjson::kArrayType);
	rapidjson::Value pattern(rapidjson::kObjectType);
	pattern.AddMember(JSON_KEY_TYPE, rapidjson::StringRef(eventType.c_str()), allocator);
	if(eventType == EVENT_TYPE_CUSTOM)
		pattern.AddMember(JSON_KEY_LENGTH_MS, rumbleLen, allocator);
	patterns.PushBack(pattern, allocator);
	handler.AddMember(JSON_KEY_PATTERN, patterns, allocator);

	handlers.PushBack(handler, allocator);
	doc.AddMember(JSON_KEY_HANDLERS, handlers, allocator);

	ssClient->bindEvent(StringUtils::stringify(doc));
}
