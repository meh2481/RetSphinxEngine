#include "DebugUI.h"
#include "imgui/imgui.h"
#include "GameEngine.h"
#include "easylogging++.h"
#include "SteelSeriesEditor.h"
#include "ParticleEditor.h"
#include "LevelEditor.h"
#include "InputDevice.h"
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
	levelEditor = new LevelEditor(ge);
}

DebugUI::~DebugUI()
{
	delete steelSeriesEditor;
	delete particleEditor;
	delete levelEditor;
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

			ImGui::MenuItem("Level Editor", NULL, &levelEditor->open);

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

	if(levelEditor->open)
	{
		_ge->pausePhysics();
		levelEditor->draw(windowFlags, hasFocus());
	}
	else
		_ge->playPhysics();

	if(rumbleMenu)
	{
		if(ImGui::Begin("Controller Rumble", &rumbleMenu, windowFlags))
		{
			//Controller rumble testing
			if(ImGui::Button("Test controller rumble"))
			{
				InputDevice* controller = _ge->getCurController();
				if(controller != NULL)
					controller->rumbleLR(motorDuration, largeMotorStrength, smallMotorStrength, _ge->getSeconds());
			}
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
