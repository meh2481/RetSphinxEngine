#include "SteelSeriesEditor.h"
#include "imgui/imgui.h"
#include "GameEngine.h"
#include "SteelSeriesEvents.h"
#include "SteelSeriesClient.h"
#include "StringUtils.h"
#include "GameEngine.h"

#define SS_BUF_SZ 33

SteelSeriesEditor::SteelSeriesEditor(GameEngine* ge)
{
	open = false;
	_ge = ge;
	appName = StringUtils::normalize(_ge->getAppName());
	selectedSSMouseRumble = 1;
	eventType = steelSeriesTactileEvents[selectedSSMouseRumble];
	selectedEventIcon = 0;
	rumbleCount = 5;
	rumbleFreq = 0.65;
	rumbleLen = 100;
	percentHealth = 100;
	mouse0Color[0] = 1.0f;
	mouse0Color[1] = 0.0f;
	mouse0Color[2] = 0.0f;
	mouse100Color[0] = 0.0f;
	mouse100Color[1] = 1.0f;
	mouse100Color[2] = 0.0f;
	prefixBuf[0] = suffixBuf[0] = '\0';
	screenMs = 0;
	colorValue = 0;
	selectedSSMouseColorZone = 0;
	colorZone = steelSeriesColorZones[selectedSSMouseColorZone];
	colorFlash = false;
	colorFlashFreq = 5.0f;
	colorFlashCount = 0;
}

void SteelSeriesEditor::draw(int windowFlags)
{
	if(ImGui::Begin("SteelSeries", &open, windowFlags))
	{
		//Mouse rumble testing
		if(ImGui::CollapsingHeader("SteelSeries mouse rumble"))
		{
			if(ImGui::Button("Test mouse rumble"))
			{
				static int eventVal = 0;
				const char* TEST_EVNT = "TEST_EVENT_RUMBLE";
				bindTactileEvent(TEST_EVNT);
				_ge->getSteelSeriesClient()->sendEvent(TEST_EVNT, ++eventVal);
			}
			ImGui::SliderInt("Rumble repeat", &rumbleCount, 1, 10);
			ImGui::SliderFloat("Rumble repeat frequency (Hz)", &rumbleFreq, 0.25, 5.0);
			if(ImGui::Combo("Rumble type", &selectedSSMouseRumble, steelSeriesTactileEvents, 124))
				eventType = steelSeriesTactileEvents[selectedSSMouseRumble];
			if(eventType == "custom")
				ImGui::SliderInt("Rumble length (ms)", &rumbleLen, 1, 2559);
		}

		//Mouse color testing
		if(ImGui::CollapsingHeader("SteelSeries mouse color"))
		{
			if(ImGui::Button("Test mouse color"))
			{
				const char* TEST_EVNT = "TEST_EVENT_COLOR";
				bindColorEvent(TEST_EVNT);
				_ge->getSteelSeriesClient()->sendEvent(TEST_EVNT, colorValue);
			}
			if(ImGui::Combo("Color zone", &selectedSSMouseColorZone, steelSeriesColorZones, 3))
				colorZone = steelSeriesColorZones[selectedSSMouseColorZone];
			ImGui::ColorEdit3("0 color", mouse0Color);
			ImGui::ColorEdit3("100 color", mouse100Color);
			ImGui::SliderInt("Color value", &colorValue, 0, 100);
			ImGui::Checkbox("Color flash", &colorFlash);
			if(colorFlash)
			{
				ImGui::SliderFloat("Color flash frequency (Hz)", &colorFlashFreq, 0.0f, 30.0f);
				ImGui::SliderInt("Color flash repeat (0 = forever)", &colorFlashCount, 0, 100);
			}
		}

		//Mouse screen testing
		if(ImGui::CollapsingHeader("SteelSeries mouse screen"))
		{
			if(ImGui::Button("Test mouse screen"))
			{
				const char* TEST_EVNT = "TEST_EVENT_SCREEN";
				bindScreenEvent(TEST_EVNT);
				_ge->getSteelSeriesClient()->sendEvent(TEST_EVNT, percentHealth);
			}

			ImGui::Combo("Icon", &selectedEventIcon, steelSeriesEventIcons, 18);
			ImGui::SliderInt("Mouse event value", &percentHealth, 1, 100);
			ImGui::InputText("Prefix text", prefixBuf, SS_BUF_SZ);
			ImGui::InputText("Suffix text", suffixBuf, SS_BUF_SZ);
			ImGui::SliderInt("Display duration (ms)", &screenMs, 0, 2559);
		}
	}
	ImGui::End();
}

void SteelSeriesEditor::bindTactileEvent(const std::string& eventId)
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appName.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, rapidjson::StringRef(eventId.c_str()), allocator);

	rapidjson::Value handlers(rapidjson::kArrayType);
	rapidjson::Value handler(rapidjson::kObjectType);
	handler.AddMember(JSON_KEY_DEVICE_TYPE, TYPE_TACTILE, allocator);
	handler.AddMember(JSON_KEY_ZONE, ZONE_ONE, allocator);
	handler.AddMember(JSON_KEY_MODE, MODE_VIBRATE, allocator);

	rapidjson::Value rate(rapidjson::kObjectType);
	rate.AddMember(JSON_KEY_FREQUENCY, rumbleFreq, allocator);
	rate.AddMember(JSON_KEY_REPEAT_LIMIT, rumbleCount, allocator);
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

	_ge->getSteelSeriesClient()->bindEvent(StringUtils::stringify(doc));
}

void SteelSeriesEditor::bindScreenEvent(const std::string& eventId)
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appName.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, rapidjson::StringRef(eventId.c_str()), allocator);
	doc.AddMember(JSON_KEY_ICON_ID, selectedEventIcon, allocator);

	rapidjson::Value handlers(rapidjson::kArrayType);
	rapidjson::Value handler(rapidjson::kObjectType);
	handler.AddMember(JSON_KEY_DEVICE_TYPE, TYPE_SCREENED, allocator);
	handler.AddMember(JSON_KEY_ZONE, ZONE_ONE, allocator);
	handler.AddMember(JSON_KEY_MODE, MODE_SCREEN, allocator);

	rapidjson::Value datas(rapidjson::kArrayType);
	rapidjson::Value data1(rapidjson::kObjectType);
	data1.AddMember(JSON_KEY_HAS_TEXT, true, allocator);
	data1.AddMember(JSON_KEY_PREFIX, rapidjson::StringRef(prefixBuf), allocator);
	data1.AddMember(JSON_KEY_SUFFIX, rapidjson::StringRef(suffixBuf), allocator);
	data1.AddMember(JSON_KEY_ICON_ID, selectedEventIcon, allocator);
	data1.AddMember(JSON_KEY_LENGTH_MILLIS, screenMs, allocator);

	datas.PushBack(data1, allocator);
	handler.AddMember(JSON_KEY_DATAS, datas, allocator);

	handlers.PushBack(handler, allocator);
	doc.AddMember(JSON_KEY_HANDLERS, handlers, allocator);

	_ge->getSteelSeriesClient()->bindEvent(StringUtils::stringify(doc));
}

void SteelSeriesEditor::bindColorEvent(const std::string& eventId)
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appName.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, rapidjson::StringRef(eventId.c_str()), allocator);
	doc.AddMember(JSON_KEY_MIN_VALUE, 0, allocator);
	doc.AddMember(JSON_KEY_MAX_VALUE, 100, allocator);
	doc.AddMember(JSON_KEY_ICON_ID, 1, allocator);

	rapidjson::Value handlers(rapidjson::kArrayType);
	rapidjson::Value handler(rapidjson::kObjectType);
	handler.AddMember(JSON_KEY_DEVICE_TYPE, TYPE_MOUSE, allocator);
	handler.AddMember(JSON_KEY_ZONE, rapidjson::StringRef(colorZone.c_str()), allocator);
	handler.AddMember(JSON_KEY_MODE, MODE_COLOR, allocator);

	rapidjson::Value color(rapidjson::kObjectType);
	rapidjson::Value gradient(rapidjson::kObjectType);

	rapidjson::Value zero(rapidjson::kObjectType);
	zero.AddMember(JSON_KEY_RED, int(mouse0Color[0] * 255), allocator);
	zero.AddMember(JSON_KEY_GREEN, int(mouse0Color[1] * 255), allocator);
	zero.AddMember(JSON_KEY_BLUE, int(mouse0Color[2] * 255), allocator);
	gradient.AddMember(JSON_KEY_ZERO, zero, allocator);

	rapidjson::Value hundred(rapidjson::kObjectType);
	hundred.AddMember(JSON_KEY_RED, int(mouse100Color[0] * 255), allocator);
	hundred.AddMember(JSON_KEY_GREEN, int(mouse100Color[1] * 255), allocator);
	hundred.AddMember(JSON_KEY_BLUE, int(mouse100Color[2] * 255), allocator);
	gradient.AddMember(JSON_KEY_HUNDRED, hundred, allocator);

	color.AddMember(JSON_KEY_GRADIENT, gradient, allocator);
	handler.AddMember(JSON_KEY_COLOR, color, allocator);

	if(colorFlash)
	{
		rapidjson::Value rate(rapidjson::kObjectType);
		rate.AddMember(JSON_KEY_FREQUENCY, colorFlashFreq, allocator);
		if(colorFlashCount)
			rate.AddMember(JSON_KEY_REPEAT_LIMIT, colorFlashCount, allocator);

		handler.AddMember(JSON_KEY_RATE, rate, allocator);
	}

	handlers.PushBack(handler, allocator);
	doc.AddMember(JSON_KEY_HANDLERS, handlers, allocator);

	_ge->getSteelSeriesClient()->bindEvent(StringUtils::stringify(doc));
}
