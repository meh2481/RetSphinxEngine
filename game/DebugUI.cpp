#include "DebugUI.h"
#include "imgui/imgui.h"
#include "GameEngine.h"
#include <climits>

const char* event_types[] = {
	"ti_predefined_strongclick_100",
	"ti_predefined_strongclick_60",
	"ti_predefined_strongclick_30",
	"ti_predefined_sharpclick_100",
	"ti_predefined_sharpclick_60",
	"ti_predefined_sharpclick_30",
	"ti_predefined_softbump_100",
	"ti_predefined_softbump_60",
	"ti_predefined_softbump_30",
	"ti_predefined_doubleclick_100",
	"ti_predefined_doubleclick_60",
	"ti_predefined_tripleclick_100",
	"ti_predefined_softfuzz_60",
	"ti_predefined_strongbuzz_100",
	"ti_predefined_buzzalert750ms",
	"ti_predefined_buzzalert1000ms",
	"ti_predefined_strongclick1_100",
	"ti_predefined_strongclick2_80",
	"ti_predefined_strongclick3_60",
	"ti_predefined_strongclick4_30",
	"ti_predefined_mediumclick1_100",
	"ti_predefined_mediumclick2_80",
	"ti_predefined_mediumclick3_60",
	"ti_predefined_sharptick1_100",
	"ti_predefined_sharptick2_80",
	"ti_predefined_sharptick3_60",
	"ti_predefined_shortdoubleclickstrong1_100",
	"ti_predefined_shortdoubleclickstrong2_80",
	"ti_predefined_shortdoubleclickstrong3_60",
	"ti_predefined_shortdoubleclickstrong4_30",
	"ti_predefined_shortdoubleclickmedium1_100",
	"ti_predefined_shortdoubleclickmedium2_80",
	"ti_predefined_shortdoubleclickmedium3_60",
	"ti_predefined_shortdoublesharptick1_100",
	"ti_predefined_shortdoublesharptick2_80",
	"ti_predefined_shortdoublesharptick3_60",
	"ti_predefined_longdoublesharpclickstrong1_100",
	"ti_predefined_longdoublesharpclickstrong2_80",
	"ti_predefined_longdoublesharpclickstrong3_60",
	"ti_predefined_longdoublesharpclickstrong4_30",
	"ti_predefined_longdoublesharpclickmedium1_100",
	"ti_predefined_longdoublesharpclickmedium2_80",
	"ti_predefined_longdoublesharpclickmedium3_60",
	"ti_predefined_longdoublesharptick1_100",
	"ti_predefined_longdoublesharptick2_80",
	"ti_predefined_longdoublesharptick3_60",
	"ti_predefined_buzz1_100",
	"ti_predefined_buzz2_80",
	"ti_predefined_buzz3_60",
	"ti_predefined_buzz4_40",
	"ti_predefined_buzz5_20",
	"ti_predefined_pulsingstrong1_100",
	"ti_predefined_pulsingstrong2_60",
	"ti_predefined_pulsingmedium1_100",
	"ti_predefined_pulsingmedium2_60",
	"ti_predefined_pulsingsharp1_100",
	"ti_predefined_pulsingsharp2_60",
	"ti_predefined_transitionclick1_100",
	"ti_predefined_transitionclick2_80",
	"ti_predefined_transitionclick3_60",
	"ti_predefined_transitionclick4_40",
	"ti_predefined_transitionclick5_20",
	"ti_predefined_transitionclick6_10",
	"ti_predefined_transitionhum1_100",
	"ti_predefined_transitionhum2_80",
	"ti_predefined_transitionhum3_60",
	"ti_predefined_transitionhum4_40",
	"ti_predefined_transitionhum5_20",
	"ti_predefined_transitionhum6_10",
	"ti_predefined_transitionrampdownlongsmooth1_100to0",
	"ti_predefined_transitionrampdownlongsmooth2_100to0",
	"ti_predefined_transitionrampdownmediumsmooth1_100to0",
	"ti_predefined_transitionrampdownmediumsmooth2_100to0",
	"ti_predefined_transitionrampdownshortsmooth1_100to0",
	"ti_predefined_transitionrampdownshortsmooth2_100to0",
	"ti_predefined_transitionrampdownlongsharp1_100to0",
	"ti_predefined_transitionrampdownlongsharp2_100to0",
	"ti_predefined_transitionrampdownmediumsharp1_100to0",
	"ti_predefined_transitionrampdownmediumsharp2_100to0",
	"ti_predefined_transitionrampdownshortsharp1_100to0",
	"ti_predefined_transitionrampdownshortsharp2_100to0",
	"ti_predefined_transitionrampuplongsmooth1_0to100",
	"ti_predefined_transitionrampuplongsmooth2_0to100",
	"ti_predefined_transitionrampupmediumsmooth1_0to100",
	"ti_predefined_transitionrampupmediumsmooth2_0to100",
	"ti_predefined_transitionrampupshortsmooth1_0to100",
	"ti_predefined_transitionrampupshortsmooth2_0to100",
	"ti_predefined_transitionrampuplongsharp1_0to100",
	"ti_predefined_transitionrampuplongsharp2_0to100",
	"ti_predefined_transitionrampupmediumsharp1_0to100",
	"ti_predefined_transitionrampupmediumsharp2_0to100",
	"ti_predefined_transitionrampupshortsharp1_0to100",
	"ti_predefined_transitionrampupshortsharp2_0to100",
	"ti_predefined_transitionrampdownlongsmooth1_50to0",
	"ti_predefined_transitionrampdownlongsmooth2_50to0",
	"ti_predefined_transitionrampdownmediumsmooth1_50to0",
	"ti_predefined_transitionrampdownmediumsmooth2_50to0",
	"ti_predefined_transitionrampdownshortsmooth1_50to0",
	"ti_predefined_transitionrampdownshortsmooth2_50to0",
	"ti_predefined_transitionrampdownlongsharp1_50to0",
	"ti_predefined_transitionrampdownlongsharp2_50to0",
	"ti_predefined_transitionrampdownmediumsharp1_50to0",
	"ti_predefined_transitionrampdownmediumsharp2_50to0",
	"ti_predefined_transitionrampdownshortsharp1_50to0",
	"ti_predefined_transitionrampdownshortsharp2_50to0",
	"ti_predefined_transitionrampuplongsmooth1_0to50",
	"ti_predefined_transitionrampuplongsmooth2_0to50",
	"ti_predefined_transitionrampupmediumsmooth1_0to50",
	"ti_predefined_transitionrampupmediumsmooth2_0to50",
	"ti_predefined_transitionrampupshortsmooth1_0to50",
	"ti_predefined_transitionrampupshortsmooth2_0to50",
	"ti_predefined_transitionrampuplongsharp1_0to50",
	"ti_predefined_transitionrampuplongsharp2_0to50",
	"ti_predefined_transitionrampupmediumsharp1_0to50",
	"ti_predefined_transitionrampupmediumsharp2_0to50",
	"ti_predefined_transitionrampupshortsharp1_0to50",
	"ti_predefined_transitionrampupshortsharp2_0to50",
	"ti_predefined_longbuzzforprogrammaticstopping_100",
	"ti_predefined_smoothhum1nokickorbrakepulse_50",
	"ti_predefined_smoothhum2nokickorbrakepulse_40",
	"ti_predefined_smoothhum3nokickorbrakepulse_30",
	"ti_predefined_smoothhum4nokickorbrakepulse_20",
	"ti_predefined_smoothhum5nokickorbrakepulse_10"
};


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

int g_largeMotorStrength = USHRT_MAX;
int g_smallMotorStrength = USHRT_MAX;
int g_motorDuration = 1000;
bool g_fireSSTest = false;
std::string g_eventType = "ti_predefined_strongclick_100";

int selectedSSMouseRumble = 0;

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
			//TODO Repeat count slider, delay between slider
			ImGui::LabelText("", "SteelSeries mouse rumble");
			if(ImGui::Button("TEST"))
				g_fireSSTest = true;
			ImGui::SameLine();
			if(ImGui::Combo("rumble type", &selectedSSMouseRumble, event_types, 123))
				g_eventType = event_types[selectedSSMouseRumble];
		}
		ImGui::End();
	}
}

bool DebugUI::hasFocus()
{
	return visible && (hadFocus || ImGui::IsMouseHoveringAnyWindow());
}

