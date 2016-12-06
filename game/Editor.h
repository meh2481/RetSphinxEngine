#pragma once

class GameEngine;

class Editor
{
	GameEngine* ge;
	const char* WINDOW_TITLE = "Level Editor";

public:
	Editor(GameEngine* _g);

	bool open;
	void draw(int windowFlags);
};
