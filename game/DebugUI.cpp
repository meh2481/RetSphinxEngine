#include "DebugUI.h"
#include "imgui/imgui.h"
#include "GameEngine.h"
#include "SteelSeriesEvents.h"
#include "SteelSeriesCommunicator.h"
#include <climits>

DebugUI::DebugUI(GameEngine *ge)
{
	visible = false;
	hadFocus = false;
	_ge = ge;
	showTestWindow = false;
	rumbleMenu = true;

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
}

DebugUI::~DebugUI()
{
}

void DebugUI::draw()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.9f));
	//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.9f));
	_draw();
	hadFocus = ImGui::IsMouseHoveringAnyWindow();
	ImGui::PopStyleColor(1);
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

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if(memEdit.Open)
		memEdit.Draw("Memory Editor", (unsigned char*)_ge, sizeof(*_ge));

#ifdef _DEBUG
	if(showTestWindow)
		ImGui::ShowTestWindow(&showTestWindow);
#endif

	if(rumbleMenu)
	{
		if(ImGui::Begin("SteelSeries & Rumble", &rumbleMenu))
		{
			//Controller rumble testing
			if(ImGui::CollapsingHeader("Game controller rumble"))
			{
				//ImGui::LabelText("", "Game controller rumble");
				if(ImGui::Button("Test controller rumble"))
					_ge->rumbleLR(motorDuration, largeMotorStrength, smallMotorStrength);
				ImGui::SliderInt("Large motor", &largeMotorStrength, 0, USHRT_MAX);
				ImGui::SliderInt("Small motor", &smallMotorStrength, 0, USHRT_MAX);
				ImGui::SliderInt("Rumble duration (ms)", &motorDuration, 10, 5000);
			}

			//Mouse rumble testing
			if(ImGui::CollapsingHeader("SteelSeries mouse rumble"))
			{
				//ImGui::LabelText("", "SteelSeries mouse rumble");
				if(ImGui::Button("Test mouse rumble"))
				{
					static int eventVal = 0;
					const char* TEST_EVNT = "TEST_EVENT_RUMBLE";
					_ge->getSSCommunicator()->bindTactileEvent(eventType, TEST_EVNT, rumbleFreq, rumbleCount, rumbleLen);
					_ge->getSSCommunicator()->sendEvent(TEST_EVNT, ++eventVal);
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
					_ge->getSSCommunicator()->bindColorEvent(TEST_EVNT, colorZone, mouse0Color, mouse100Color, colorFlash, colorFlashFreq, colorFlashCount);
					_ge->getSSCommunicator()->sendEvent(TEST_EVNT, colorValue);
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
					_ge->getSSCommunicator()->bindScreenEvent(TEST_EVNT, selectedEventIcon, screenMs, prefixBuf, suffixBuf);
					_ge->getSSCommunicator()->sendEvent(TEST_EVNT, percentHealth);
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
}

bool DebugUI::hasFocus()
{
	return visible && (hadFocus || ImGui::IsMouseHoveringAnyWindow());
}

