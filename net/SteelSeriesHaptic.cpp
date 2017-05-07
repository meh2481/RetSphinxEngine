#include "SteelSeriesHaptic.h"
#include "SteelSeriesClient.h"
#include "SteelSeriesEvents.h"
#include "StringUtils.h"

#define CUSTOM_RUMBLE_EVENT "MOUSE_RUMBLE"		//Custom length (strength 100)
#define CLICK_10MS_100 "MOUSE_CLICK_10MS_100"	//4
#define CLICK_10MS_60 "MOUSE_CLICK_10MS_60"		//5
#define CLICK_10MS_30 "MOUSE_CLICK_10MS_30"		//6
#define TICK_10MS_100 "MOUSE_TICK_10MS_100"		//24
#define TICK_10MS_60 "MOUSE_TICK_10MS_60"		//25
#define TICK_10MS_30 "MOUSE_TICK_10MS_30"		//26
#define BUMP_20MS_100 "MOUSE_BUMP_20MS_100"		//7
#define BUMP_20MS_60 "MOUSE_BUMP_20MS_60"		//8
#define BUMP_20MS_30 "MOUSE_BUMP_20MS_30"		//9
#define CLICK_20MS_100 "MOUSE_CLICK_20MS_100"	//21
#define CLICK_20MS_80 "MOUSE_CLICK_20MS_80"		//22
#define CLICK_20MS_60 "MOUSE_CLICK_20MS_60"		//23
#define CLICK_60MS_100 "MOUSE_CLICK_60MS_100"	//1
#define CLICK_60MS_60 "MOUSE_CLICK_60MS_60"		//2
#define CLICK_60MS_30 "MOUSE_CLICK_60MS_30"		//3
#define CLICK_75MS_100 "MOUSE_CLICK_75MS_100"	//17
#define CLICK_75MS_80 "MOUSE_CLICK_75MS_80"		//18
#define CLICK_75MS_60 "MOUSE_CLICK_75MS_60"		//19
#define CLICK_75MS_30 "MOUSE_CLICK_75MS_30"		//20
#define FUZZ_100MS_60 "MOUSE_FUZZ_100MS_60"		//13
#define BUZZ_100MS_100 "MOUSE_BUZZ_100MS_100"	//14
#define BUZZ_200MS_100 "MOUSE_BUZZ_200MS_100"	//47
#define BUZZ_200MS_80 "MOUSE_BUZZ_200MS_80"		//48
#define BUZZ_200MS_60 "MOUSE_BUZZ_200MS_60"		//49
#define BUZZ_200MS_40 "MOUSE_BUZZ_200MS_40"		//50
#define BUZZ_200MS_20 "MOUSE_BUZZ_200MS_20"		//51
#define CLICK_500MS_100 "MOUSE_CLICK_500MS_100"	//58
#define CLICK_500MS_80 "MOUSE_CLICK_500MS_80"	//59
#define CLICK_500MS_60 "MOUSE_CLICK_500MS_60"	//60
#define CLICK_500MS_40 "MOUSE_CLICK_500MS_40"	//61
#define CLICK_500MS_20 "MOUSE_CLICK_500MS_20"	//62
#define CLICK_500MS_10 "MOUSE_CLICK_500MS_10"	//63
#define HUM_500MS_100 "MOUSE_HUM_500MS_100"		//64
#define HUM_500MS_80 "MOUSE_HUM_500MS_80"		//65
#define HUM_500MS_60 "MOUSE_HUM_500MS_60"		//66
#define HUM_500MS_40 "MOUSE_HUM_500MS_40"		//67
#define HUM_500MS_20 "MOUSE_HUM_500MS_20"		//68
#define HUM_500MS_10 "MOUSE_HUM_500MS_10"		//69
#define BUZZ_750MS_100 "MOUSE_BUZZ_750MS_100"	//15
#define BUZZ_1S_100 "MOUSE_BUZZ_1S_100"			//16

SteelSeriesHaptic::SteelSeriesHaptic(SteelSeriesClient * client)
{
	ssClient = client;
}

SteelSeriesHaptic::~SteelSeriesHaptic()
{
}

void SteelSeriesHaptic::init()
{
	//Bind all our predefined events
	bindEvent(TICK_10MS_100, steelSeriesTactileEvents[24], 0);
	bindEvent(TICK_10MS_60, steelSeriesTactileEvents[25], 0);
	bindEvent(TICK_10MS_30, steelSeriesTactileEvents[26], 0);
	bindEvent(CLICK_10MS_100, steelSeriesTactileEvents[4], 0);
	bindEvent(CLICK_10MS_60, steelSeriesTactileEvents[5], 0);
	bindEvent(CLICK_10MS_30, steelSeriesTactileEvents[6], 0);
	bindEvent(CLICK_20MS_100, steelSeriesTactileEvents[21], 0);
	bindEvent(CLICK_20MS_80, steelSeriesTactileEvents[22], 0);
	bindEvent(CLICK_20MS_60, steelSeriesTactileEvents[23], 0);
	bindEvent(CLICK_60MS_100, steelSeriesTactileEvents[1], 0);
	bindEvent(CLICK_60MS_60, steelSeriesTactileEvents[2], 0);
	bindEvent(CLICK_60MS_30, steelSeriesTactileEvents[3], 0);
	bindEvent(CLICK_75MS_100, steelSeriesTactileEvents[17], 0);
	bindEvent(CLICK_75MS_80, steelSeriesTactileEvents[18], 0);
	bindEvent(CLICK_75MS_60, steelSeriesTactileEvents[19], 0);
	bindEvent(CLICK_75MS_30, steelSeriesTactileEvents[20], 0);
	bindEvent(BUZZ_100MS_100, steelSeriesTactileEvents[14], 0);
	bindEvent(BUZZ_200MS_100, steelSeriesTactileEvents[47], 0);
	bindEvent(BUZZ_200MS_80, steelSeriesTactileEvents[48], 0);
	bindEvent(BUZZ_200MS_60, steelSeriesTactileEvents[49], 0);
	bindEvent(BUZZ_200MS_40, steelSeriesTactileEvents[50], 0);
	bindEvent(BUZZ_200MS_20, steelSeriesTactileEvents[51], 0);
	bindEvent(HUM_500MS_100, steelSeriesTactileEvents[64], 0);
	bindEvent(HUM_500MS_80, steelSeriesTactileEvents[65], 0);
	bindEvent(HUM_500MS_60, steelSeriesTactileEvents[66], 0);
	bindEvent(HUM_500MS_40, steelSeriesTactileEvents[67], 0);
	bindEvent(HUM_500MS_20, steelSeriesTactileEvents[68], 0);
	bindEvent(HUM_500MS_10, steelSeriesTactileEvents[69], 0);
	bindEvent(BUZZ_750MS_100, steelSeriesTactileEvents[15], 0);
	bindEvent(BUZZ_1S_100, steelSeriesTactileEvents[16], 0);
}

bool SteelSeriesHaptic::isValid()
 { 
	 return ssClient != NULL && ssClient->isValid();
 }

void SteelSeriesHaptic::rumble(float strength, uint32_t duration, float curTime)
{
	static int eventVal = 0;	//Need some sort of value for this event, but we don't actually care about value, so increment each time
	static float fLastRumble = 0.0f;

	float sec = (float)duration / 1000.0;

	//Last rumble is still going or 0-msec duration
	if(curTime < fLastRumble || duration < 1)
		return;

	fLastRumble = curTime + sec;

	std::string evnt = CUSTOM_RUMBLE_EVENT;	//Event to send
	//Map relative strengths/lengths to the events SS has available (yuck)
	if(duration < 8)
	{
		if(strength < 0.55f)
			evnt = TICK_10MS_30;
		else if(strength < 0.80f)
			evnt = TICK_10MS_60;
		else
			evnt = TICK_10MS_100;
	}
	else if(duration < 15)
	{
		if(strength < 0.55f)
			evnt = CLICK_10MS_30;
		else if(strength < 0.80f)
			evnt = CLICK_10MS_60;
		else
			evnt = CLICK_10MS_100;
	}
	else if(duration < 40)
	{
		if(strength < 0.7f)
			evnt = CLICK_20MS_60;
		else if(strength < 0.9f)
			evnt = CLICK_20MS_80;
		else
			evnt = CLICK_20MS_100;
	}
	else if(duration < 70)
	{
		if(strength < 0.55f)
			evnt = CLICK_60MS_30;
		else if(strength < 0.80f)
			evnt = CLICK_60MS_60;
		else
			evnt = CLICK_60MS_100;
	}
	else if(duration < 88)
	{
		if(strength < 0.55f)
			evnt = CLICK_75MS_30;
		else if(strength < 0.7f)
			evnt = CLICK_75MS_60;
		else if(strength < 0.9f)
			evnt = CLICK_75MS_80;
		else
			evnt = CLICK_75MS_100;
	}
	else if(duration < 150)
	{
		evnt = BUZZ_100MS_100;
	}
	else if(duration < 350)
	{
		if(strength < 0.3f)
			evnt = BUZZ_200MS_20;
		else if(strength < 0.55f)
			evnt = BUZZ_200MS_40;
		else if(strength < 0.7f)
			evnt = BUZZ_200MS_60;
		else if(strength < 0.9f)
			evnt = BUZZ_200MS_80;
		else
			evnt = BUZZ_200MS_100;
	}
	else if(duration < 625)
	{
		if(strength < 0.15f)
			evnt = HUM_500MS_10;
		else if(strength < 0.3f)
			evnt = HUM_500MS_20;
		else if(strength < 0.55f)
			evnt = HUM_500MS_40;
		else if(strength < 0.7f)
			evnt = HUM_500MS_60;
		else if(strength < 0.9f)
			evnt = HUM_500MS_80;
		else
			evnt = HUM_500MS_100;
	}
	else if(duration < 875)
	{
		evnt = BUZZ_750MS_100;
	}
	else if(duration < 1500)
	{
		evnt = BUZZ_1S_100;
	}
	else	//Create and send custom event for larger than 1.5s buzzes
	{
		bindEvent(CUSTOM_RUMBLE_EVENT, EVENT_TYPE_CUSTOM, duration);
	}
	ssClient->sendEvent(evnt, ++eventVal);
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
