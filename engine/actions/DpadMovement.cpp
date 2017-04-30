#include "DpadMovement.h"

DpadMovement::DpadMovement(SDL_GameController * c, int l, int r, int u, int d)
{
	up = u;
	down = d;
	left = l;
	right = r;
	controller = c;
}

Vec2 DpadMovement::getMovement()
{
	Vec2 v(0.0f, 0.0f);

	if(SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)up) > 0)
		v.y = v.y + 1.0f;
	if(SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)down) > 0)
		v.y = v.y - 1.0f;
	if(SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)right) > 0)
		v.x = v.x + 1.0f;
	if(SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)left) > 0)
		v.x = v.x - 1.0f;

	//TODO Normalize
	return v;
}
