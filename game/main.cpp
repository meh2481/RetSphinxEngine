/*
    GameEngine source - main.cpp
    Copyright (c) 2014 Mark Hutcheson
*/

#include "GameEngine.h"
#include "Logger.h"
#include "LuaInterface.h"

#ifdef _WIN32
#define ICONNAME "res/icons/icon_32.img"    //For some reason, Windoze (Or SDL2, or something) doesn't like large (256x256) icons for windows. Using a 32x32 icon instead.
#else
#define ICONNAME "res/icons/icon_256.img"
#endif

#ifdef _WIN32
int SDL_main(int argc, char *argv[])
#else
int main(int argc, char** argv)
#endif
{
    GameEngine* eng = new GameEngine(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Game Engine", "RetiredSphinxGameDev", "GameEngine", ICONNAME, true); //Create our engine

    std::vector<std::string> lCommandLine;

    for(int i = 1; i < argc; i++)
        lCommandLine.push_back(argv[i]);

    eng->commandline(lCommandLine);
    eng->start(); //Get the engine rolling

    //Done main loop; exit program
    LOG_info("Deleting engine");
    delete eng;
    LOG_info("Ending program happily");
    logger_quit();
    return 0;
}
