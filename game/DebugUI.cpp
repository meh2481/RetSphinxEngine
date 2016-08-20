#include "DebugUI.h"
#include "imgui/imgui.h"
#include "GameEngine.h"
#include <climits>

DebugUI::DebugUI(GameEngine *ge)
: visible(false), hadFocus(false), _ge(ge), showTestWindow(false), rumbleMenu(false)
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
		if(ImGui::Begin("Gamepad Rumble Test", &rumbleMenu))
		{
			ImGui::SliderInt("large motor", &g_largeMotorStrength, 0, USHRT_MAX);
			ImGui::SliderInt("small motor", &g_smallMotorStrength, 0, USHRT_MAX);
			ImGui::SliderInt("duration (msec)", &g_motorDuration, 0, 5000);
		}
		ImGui::End();
	}
}

bool DebugUI::hasFocus()
{
	return visible && (hadFocus || ImGui::IsMouseHoveringAnyWindow());
}

