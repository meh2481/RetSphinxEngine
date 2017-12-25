#include "LevelEditor.h"
#include "GameEngine.h"
#include "imgui/imgui.h"
#include "Node.h"
#include "EntityManager.h"
#include "Box2D/Box2D.h"

LevelEditor::LevelEditor(GameEngine * _g)
{
	open = false;
	ge = _g;
}

void LevelEditor::draw(int windowFlags, bool focus)
{
	if(ImGui::Begin(WINDOW_TITLE, &open, windowFlags))
	{
		ImGuiIO& io = ImGui::GetIO();
		ge->cameraPos.z += io.MouseWheel * 4.0f; //+ = zoom in, - = zoom out

		Vec2 cursorPos = ge->worldPosFromCursor(Vec2(io.MousePos.x, io.MousePos.y), ge->cameraPos);
		Vec2 cursorMove = ge->worldMovement(Vec2(io.MouseDelta.x, -io.MouseDelta.y), ge->cameraPos);
		static Node* draggingNode = NULL;
		//Pan view with rmb+drag
		if(!focus && io.MouseDown[1])
		{
			ge->cameraPos.x += cursorMove.x;
			ge->cameraPos.y += cursorMove.y;
		}
		else if(!focus && io.MouseDown[0])	//Move objects with LMB
		{
			Node* curOverNode = ge->getEntityManager()->getNodeUnder(cursorPos);
			if(draggingNode == NULL && curOverNode != NULL)
				draggingNode = curOverNode;
		}
		else if(!io.MouseDown[0])
		{
			if(draggingNode)
			{
				draggingNode->lua->callMethod(draggingNode, "init");	//Re-init lua to recalc position
				draggingNode = NULL;
			}
		}

		if(draggingNode != NULL)
		{
			draggingNode->pos += cursorMove;
			draggingNode->body->SetTransform(b2Vec2(draggingNode->pos.x, draggingNode->pos.y), draggingNode->body->GetAngle());
		}
	}
	else
		ge->playPhysics();
	ImGui::End();
}
