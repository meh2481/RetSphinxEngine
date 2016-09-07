#include "DebugUI.h"
#include "imgui/imgui.h"
#include "GameEngine.h"
#include "SteelSeriesEvents.h"
#include "SteelSeriesClient.h"
#include "StringUtils.h"
#include "ParticleSystem.h"
#include <climits>

DebugUI::DebugUI(GameEngine *ge)
{
	visible = false;
	hadFocus = false;
	_ge = ge;
	showTestWindow = false;
	rumbleMenu = true;
	windowFlags = ImGuiWindowFlags_ShowBorders;
	appName = StringUtils::normalize(_ge->getAppName());

	largeMotorStrength = USHRT_MAX;
	smallMotorStrength = USHRT_MAX;
	motorDuration = 100;
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

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.9f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.9f);

	particleSystemEdit = true;
	particles = new ParticleSystem();
}

DebugUI::~DebugUI()
{
}

void DebugUI::draw()
{
	_draw();
	hadFocus = ImGui::IsMouseHoveringAnyWindow();
}

void DebugUI::_draw()
{
	if(!visible)
		return;


	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("Windows"))
		{
#ifdef _DEBUG
			ImGui::MenuItem("ImGui Demo", NULL, &showTestWindow);
#endif
			ImGui::MenuItem("Memory Editor", NULL, &memEdit.Open);

			ImGui::MenuItem("SteelSeries & Rumble", NULL, &rumbleMenu);

			ImGui::MenuItem("Particle System Editor", NULL, &particleSystemEdit);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if(memEdit.Open)
		memEdit.Draw("Memory Editor", (unsigned char*)_ge, sizeof(*_ge), 0, windowFlags);

#ifdef _DEBUG
	if(showTestWindow)
		ImGui::ShowTestWindow(&showTestWindow);
#endif

	if(rumbleMenu)
	{
		if(ImGui::Begin("SteelSeries & Rumble", &rumbleMenu, windowFlags))
		{
			//Controller rumble testing
			if(ImGui::CollapsingHeader("Game controller rumble"))
			{
				if(ImGui::Button("Test controller rumble"))
					_ge->rumbleLR(motorDuration, largeMotorStrength, smallMotorStrength);
				ImGui::SliderInt("Large motor", &largeMotorStrength, 0, USHRT_MAX);
				ImGui::SliderInt("Small motor", &smallMotorStrength, 0, USHRT_MAX);
				ImGui::SliderInt("Rumble duration (ms)", &motorDuration, 10, 5000);
			}

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

	if(particleSystemEdit)
	{
		if(ImGui::Begin("Particle System Editor", &particleSystemEdit, windowFlags))
		{
			if(ImGui::CollapsingHeader("Size"))
			{
				static float startSz[2] = { 1.0f, 1.0f };
				if(ImGui::SliderFloat2("Starting Size (x, y)", startSz, 0.0f, 5.0f))
				{
					particles->sizeStart.x = startSz[0];
					particles->sizeStart.y = startSz[1];
				}
				static float endSz[2] = { 1.0f, 1.0f };
				if(ImGui::SliderFloat2("Ending Size (x, y)", endSz, 0.0f, 5.0f))
				{
					particles->sizeEnd.x = endSz[0];
					particles->sizeEnd.y = endSz[1];
				}
				ImGui::SliderFloat("Size var", &particles->sizeVar, 0.0f, 10.0f);
			}
			if(ImGui::CollapsingHeader("Speed"))
			{
				ImGui::SliderFloat("Speed", &particles->speed, 0.0f, 50.0f);
				ImGui::SliderFloat("Speed var", &particles->speedVar, 0.0f, 50.0f);
				static float accel[2] = { 0.0f, 0.0f };
				if(ImGui::SliderFloat2("Acceleration (x, y)", accel, 0.0f, 5.0f))
				{
					particles->accel.x = accel[0];
					particles->accel.y = accel[1];
				}
				static float accelVar[2] = { 0.0f, 0.0f };
				if(ImGui::SliderFloat2("Acceleration var (x, y)", accelVar, 0.0f, 5.0f))
				{
					particles->accelVar.x = accelVar[0];
					particles->accelVar.y = accelVar[1];
				}
				ImGui::SliderFloat("Tangential Accel", &particles->tangentialAccel, -100.0f, 100.0f);
				ImGui::SliderFloat("Tangential Accel var", &particles->tangentialAccelVar, -100.0f, 100.0f);
				ImGui::SliderFloat("Normal Accel", &particles->normalAccel, -100.0f, 100.0f);
				ImGui::SliderFloat("Normal Accel var", &particles->normalAccelVar, -100.0f, 100.0f);
			}
			//TODO: Check if these are radians or degrees
			if(ImGui::CollapsingHeader("Rotation"))
			{
				ImGui::SliderFloat("Starting Rotation", &particles->rotStart, -180.0f, 180.0f);
				ImGui::SliderFloat("Starting Rotation var", &particles->rotStartVar, -180.0f, 180.0f);
				ImGui::SliderFloat("Rotational velocity", &particles->rotVel, -180.0f, 180.0f);
				ImGui::SliderFloat("Rotational velocity var", &particles->rotVelVar, -180.0f, 180.0f);
				ImGui::SliderFloat("Rotational Acceleration", &particles->rotAccel, -180.0f, 180.0f);
				ImGui::SliderFloat("Rotational Acceleration var", &particles->rotAccelVar, -180.0f, 180.0f);
				static float rotAxis[3] = { 0.0f, 0.0f, 1.0f };
				if(ImGui::SliderFloat3("Rotation Axis", rotAxis, -1.0f, 1.0f))
				{
					particles->rotAxis.x = rotAxis[0];
					particles->rotAxis.y = rotAxis[1];
					particles->rotAxis.z = rotAxis[2];
				}
				static float rotAxisVar[3] = { 0.0f, 0.0f, 0.0f };
				if(ImGui::SliderFloat3("Rotation Axis var", rotAxisVar, -1.0f, 1.0f))
				{
					particles->rotAxisVar.x = rotAxisVar[0];
					particles->rotAxisVar.y = rotAxisVar[1];
					particles->rotAxisVar.z = rotAxisVar[2];
				}
			}
			if(ImGui::CollapsingHeader("Color"))
			{
				static float startCol[4] = {1.0f, 1.0f, 1.0f, 1.0f};
				if(ImGui::ColorEdit4("Start col", startCol))
					particles->colStart.set(startCol[0], startCol[1], startCol[2], startCol[3]);
				static float endCol[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				if(ImGui::ColorEdit4("End col", endCol))
					particles->colEnd.set(endCol[0], endCol[1], endCol[2], endCol[3]);
				static float colVar[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				if(ImGui::ColorEdit4("Color var", colVar))
					particles->colVar.set(colVar[0], colVar[1], colVar[2], colVar[3]);
			}
			if(ImGui::CollapsingHeader("Life"))
			{
				ImGui::SliderFloat("Lifetime (sec)", &particles->lifetime, 0.0f, 20.0f);
				ImGui::SliderFloat("Lifetime var", &particles->lifetimeVar, -10.0f, 10.0f);
				ImGui::SliderFloat("Lifetime Pre-Fade", &particles->lifetimePreFade, 0.0f, 20.0f);
				ImGui::SliderFloat("Lifetime Pre-Fade var", &particles->lifetimePreFadeVar, -10.0f, 10.0f);
				static bool psysDecay = false;
				if(ImGui::Checkbox("Particle System Decay", &psysDecay))
					particles->decay = 5.0f;	//On first tick, set to 5 secs (because it'll be FLT_MAX, which isn't pretty to look at)
				if(psysDecay)
					ImGui::SliderFloat("decay", &particles->decay, 0.0f, 120.0f);
				else
					particles->decay = FLT_MAX;
			}
		}
		ImGui::End();
	}
}

bool DebugUI::hasFocus()
{
	return visible && (hadFocus || ImGui::IsMouseHoveringAnyWindow());
}

void DebugUI::bindTactileEvent(std::string eventId)
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
	if(eventType == steelSeriesTactileEvents[0])
		pattern.AddMember(JSON_KEY_LENGTH_MS, rumbleLen, allocator);
	patterns.PushBack(pattern, allocator);
	handler.AddMember(JSON_KEY_PATTERN, patterns, allocator);

	handlers.PushBack(handler, allocator);
	doc.AddMember(JSON_KEY_HANDLERS, handlers, allocator);

	_ge->getSteelSeriesClient()->bindEvent(StringUtils::stringify(doc));
}

void DebugUI::bindScreenEvent(std::string eventId)
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

void DebugUI::bindColorEvent(std::string eventId)
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
