#include "Editor.h"
#include "Editor.h"
#include "GameEngine.h"
#include "imgui/imgui.h"

Editor::Editor(GameEngine * _g)
{
	open = false;
	ge = _g;
}

void Editor::draw(int windowFlags)
{
	if(ImGui::Begin(WINDOW_TITLE, &open, windowFlags))
	{
		
	}
	else
		ge->playPhysics();
	ImGui::End();
}
