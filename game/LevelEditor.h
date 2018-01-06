#pragma once

class GameEngine;

class LevelEditor
{
    GameEngine* ge;

public:
    LevelEditor(GameEngine* _g);

    bool open;
    void draw(int windowFlags, bool focus);
};
