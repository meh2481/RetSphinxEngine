#pragma once

class GameEngine;

class LevelEditor
{
    GameEngine* ge;
    const char* WINDOW_TITLE = "Level Editor";

public:
    LevelEditor(GameEngine* _g);

    bool open;
    void draw(int windowFlags, bool focus);
};
