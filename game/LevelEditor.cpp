#include "LevelEditor.h"
#include "GameEngine.h"
#include "imgui/imgui.h"

LevelEditor::LevelEditor(GameEngine * _g)
{
	open = true;// false;
	ge = _g;
}

void LevelEditor::draw(int windowFlags, bool focus)
{
	if(ImGui::Begin(WINDOW_TITLE, &open, windowFlags))
	{
		ImGuiIO& io = ImGui::GetIO();
		ge->CameraPos.z += io.MouseWheel * 4.0f; //+ = zoom in, - = zoom out

		//Pan view with rmb+drag
		if(!focus && io.MouseDown[1])
		{
			Vec2 movement = ge->worldMovement(Vec2(io.MouseDelta.x, io.MouseDelta.y), ge->CameraPos);
			movement.y = -movement.y;
			ge->CameraPos.x += movement.x;
			ge->CameraPos.y += movement.y;
		}
	}
	else
		ge->playPhysics();
	ImGui::End();
}
