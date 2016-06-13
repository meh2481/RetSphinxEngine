#include "DebugUI.h"
#include "imgui/imgui.h"
#include "GameEngine.h"

DebugUI::DebugUI(GameEngine *ge)
: visible(false), hadFocus(false), _ge(ge), showTestWindow(false)
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
}

bool DebugUI::hasFocus()
{
	return visible && (hadFocus || ImGui::IsMouseHoveringAnyWindow());
}

