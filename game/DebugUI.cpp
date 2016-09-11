#include "DebugUI.h"
#include "imgui/imgui.h"
#include "GameEngine.h"
#include "easylogging++.h"
#include "SteelSeriesEditor.h"
#include "ParticleEditor.h"
#include <climits>

DebugUI::DebugUI(GameEngine *ge)
{
	visible = false;
	hadFocus = false;
	_ge = ge;
	rumbleMenu = false;
	windowFlags = ImGuiWindowFlags_ShowBorders;

	largeMotorStrength = USHRT_MAX;
	smallMotorStrength = USHRT_MAX;
	motorDuration = 100;
	
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.9f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.9f);
	
	steelSeriesEditor = new SteelSeriesEditor(ge);
	particleEditor = new ParticleEditor(ge);
}

DebugUI::~DebugUI()
{
	delete steelSeriesEditor;
	delete particleEditor;
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
			ImGui::MenuItem("Memory Editor", NULL, &memEdit.Open);

			ImGui::MenuItem("Controller Rumble", NULL, &rumbleMenu);

			ImGui::MenuItem("SteelSeries", NULL, &steelSeriesEditor->open);

			ImGui::MenuItem("Particle System Editor", NULL, &particleEditor->open);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if(memEdit.Open)
		memEdit.Draw("Memory Editor", (unsigned char*)_ge, sizeof(*_ge), 0, windowFlags);

	if(steelSeriesEditor->open)
		steelSeriesEditor->draw(windowFlags);

	if(particleEditor->open)
		particleEditor->draw(windowFlags);

	if(rumbleMenu)
	{
		if(ImGui::Begin("Controller Rumble", &rumbleMenu, windowFlags))
		{
			//Controller rumble testing
			if(ImGui::Button("Test controller rumble"))
				_ge->rumbleLR(motorDuration, largeMotorStrength, smallMotorStrength);
			ImGui::SliderInt("Large motor", &largeMotorStrength, 0, USHRT_MAX);
			ImGui::SliderInt("Small motor", &smallMotorStrength, 0, USHRT_MAX);
			ImGui::SliderInt("Rumble duration (ms)", &motorDuration, 10, 5000);
		}
		ImGui::End();
	}
}

bool DebugUI::hasFocus()
{
	return visible && (hadFocus || ImGui::IsMouseHoveringAnyWindow());
}
