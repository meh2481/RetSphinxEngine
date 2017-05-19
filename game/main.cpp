/*
	GameEngine source - main.cpp
	Copyright (c) 2014 Mark Hutcheson
*/

#include "GameEngine.h"
#include "easylogging++.h"

//Define constants for logger
INITIALIZE_EASYLOGGINGPP

#ifdef _WIN32
#define ICONNAME "res/icons/icon_32.png"	//For some reason, Windoze (Or SDL2, or something) doesn't like large (256x256) icons for windows. Using a 32x32 icon instead.
#else
#define ICONNAME "res/icons/icon_256.png"
#endif

#ifdef _WIN32
int SDL_main(int argc, char *argv[])
#else
int main(int argc, char** argv)
#endif
{
	//Give logger our commandline parameters (logger isn't actually started until Engine::Engine())
	START_EASYLOGGINGPP(argc, argv);
	
	GameEngine* eng = new GameEngine(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Game Engine", "RetiredSphinxGameDev", "GameEngine", ICONNAME, true); //Create our engine

	//Start Lua
	LuaInterface Lua("res/lua/init.lua", argc, argv);
	if (!Lua.Init()) return 1;
	eng->setLua(&Lua);

	std::list<std::string> lCommandLine;
	
	for(int i = 1; i < argc; i++)
		lCommandLine.push_back(argv[i]);

	eng->commandline(lCommandLine);
	eng->start(); //Get the engine rolling
	
	//Done main loop; exit program	
	LOG(INFO) << "Deleting engine";
	delete eng;
	LOG(INFO) << "Ending program happily";
	return 0;
}
