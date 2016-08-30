#include "DebugUI.h"
#include "imgui/imgui.h"
#include "GameEngine.h"
#include "SteelSeriesEvents.h"
#include <climits>

//TODO don't have these as global variables; pass them around as needed
int g_largeMotorStrength = USHRT_MAX;
int g_smallMotorStrength = USHRT_MAX;
int g_motorDuration = 1000;
bool g_fireSSTest = false;
std::string g_eventType = "ti_predefined_strongclick_100";
int selectedSSMouseRumble = 1;
int g_rumbleCount = 5;
float g_rumbleFreq = 0.65;
int g_rumbleLen = 100;

DebugUI::DebugUI(GameEngine *ge)
: visible(false), hadFocus(false), _ge(ge), showTestWindow(false), rumbleMenu(true)
{
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
			ImGui::LabelText("", "Game controller rumble (Press A to test)");
			ImGui::SliderInt("large motor", &g_largeMotorStrength, 0, USHRT_MAX);
			ImGui::SliderInt("small motor", &g_smallMotorStrength, 0, USHRT_MAX);
			ImGui::SliderInt("duration (msec)", &g_motorDuration, 0, 5000);

			//TEST
			ImGui::LabelText("", "SteelSeries mouse rumble");
			if(ImGui::Button("TEST"))
				g_fireSSTest = true;
			ImGui::SliderInt("repeat", &g_rumbleCount, 1, 10);
			ImGui::SliderFloat("frequency (Hz)", &g_rumbleFreq, 0.25, 5.0);
			if(ImGui::Combo("rumble type", &selectedSSMouseRumble, steelSeriesTactileEvents, 124))
				g_eventType = steelSeriesTactileEvents[selectedSSMouseRumble];
			if(g_eventType == "custom")
				ImGui::SliderInt("length (msec)", &g_rumbleLen, 1, 2559);
		}
		ImGui::End();
	}
}

bool DebugUI::hasFocus()
{
	return visible && (hadFocus || ImGui::IsMouseHoveringAnyWindow());
}

