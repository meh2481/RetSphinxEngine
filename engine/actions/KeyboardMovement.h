#pragma once
#include "MovementBind.h"
#include "SDL_scancode.h"

class KeyboardMovement : public MovementBind
{
private:
	int left, right, up, down;
	const Uint8* keystates;

public:
	KeyboardMovement(int left, int right, int up, int down);

	Vec2 getMovement();
};
