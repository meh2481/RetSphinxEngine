#include "DebugUI.h"
#include "imgui/imgui.h"
#include "GameEngine.h"
#include "SteelSeriesEvents.h"
#include "SteelSeriesCommunicator.h"
#include <climits>

DebugUI::DebugUI(GameEngine *ge)
: visible(false), hadFocus(false), _ge(ge), showTestWindow(false), rumbleMenu(true)
{
	largeMotorStrength = USHRT_MAX;
	smallMotorStrength = USHRT_MAX;
	motorDuration = 1000;
	eventType = "ti_predefined_strongclick_100";
	selectedSSMouseRumble = 1;
	rumbleCount = 5;
	rumbleFreq = 0.65;
	rumbleLen = 100;
}

DebugUI::~DebugUI()
{
}

void DebugUI::draw()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
	_draw();
	hadFocus = ImGui::IsMouseHoveringAnyWindow();
	ImGui::PopStyleColor(2);
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
			ImGui::MenuItem("ImGui Test", NULL, &showTestWindow);
#endif
			ImGui::MenuItem("Memory debugger", NULL, &memEdit.Open);

			ImGui::MenuItem("Gamepad rumble", NULL, &rumbleMenu);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if(memEdit.Open)
		memEdit.Draw("GameEngine memory", (unsigned char*)_ge, sizeof(*_ge));

#ifdef _DEBUG
	if(showTestWindow)
		ImGui::ShowTestWindow(&showTestWindow);
#endif

	if(rumbleMenu)
	{
		if(ImGui::Begin("Rumble Test", &rumbleMenu))
		{
			ImGui::LabelText("", "Game controller rumble");
			if(ImGui::Button("Test Controller Rumble"))
				_ge->rumbleLR(motorDuration, largeMotorStrength, smallMotorStrength);
			ImGui::SliderInt("large motor", &largeMotorStrength, 0, USHRT_MAX);
			ImGui::SliderInt("small motor", &smallMotorStrength, 0, USHRT_MAX);
			ImGui::SliderInt("duration (msec)", &motorDuration, 0, 5000);

			ImGui::LabelText("", "SteelSeries mouse rumble");
			if(ImGui::Button("Test Mouse Rumble"))
			{
					static int eventVal = 0;
					const char* TEST_EVNT = "TEST_EVNT2";
					_ge->getSSCommunicator()->bindEvent(eventType, TEST_EVNT, rumbleFreq, rumbleCount, rumbleLen);
					_ge->getSSCommunicator()->sendEvent(TEST_EVNT, ++eventVal);
			}
			ImGui::SliderInt("repeat", &rumbleCount, 1, 10);
			ImGui::SliderFloat("frequency (Hz)", &rumbleFreq, 0.25, 5.0);
			if(ImGui::Combo("rumble type", &selectedSSMouseRumble, steelSeriesTactileEvents, 124))
				eventType = steelSeriesTactileEvents[selectedSSMouseRumble];
			if(eventType == "custom")
				ImGui::SliderInt("length (msec)", &rumbleLen, 1, 2559);
		}
		ImGui::End();
	}
}

bool DebugUI::hasFocus()
{
	return visible && (hadFocus || ImGui::IsMouseHoveringAnyWindow());
}

