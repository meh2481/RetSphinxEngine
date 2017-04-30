#pragma once
#include "MovementBind.h"
#include "SDL_gamecontroller.h"

class DpadMovement : public MovementBind
{
private:
	int up, down, left, right;
	SDL_GameController* controller;

	DpadMovement() {};

public:
	DpadMovement(SDL_GameController* controller, int left, int right, int up, int down);

	Vec2 getMovement();

};