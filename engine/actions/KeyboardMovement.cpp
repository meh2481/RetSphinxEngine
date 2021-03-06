#include "KeyboardMovement.h"
#include "SDL_keyboard.h"

KeyboardMovement::KeyboardMovement(int l, int r, int u, int d)
{
    left = l;
    right = r;
    up = u;
    down = d;
    keystates = SDL_GetKeyboardState(NULL);
}

Vec2 KeyboardMovement::getMovement(InputDevice* d)
{
    Vec2 v(0.0f, 0.0f);

    if(keystates[up])
        v.y += 1.0f;
    if(keystates[down])
        v.y -= 1.0f;
    if(keystates[right])
        v.x += 1.0f;
    if(keystates[left])
        v.x -= 1.0f;

    return v;
}
