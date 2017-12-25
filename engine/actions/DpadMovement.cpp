#include "DpadMovement.h"
#include "InputDevice.h"

DpadMovement::DpadMovement(int l, int r, int u, int d)
{
    up = u;
    down = d;
    left = l;
    right = r;
}

Vec2 DpadMovement::getMovement(InputDevice* d)
{
    Vec2 v(0.0f, 0.0f);

    if(d->getButton(up))
        v.y += 1.0f;
    if(d->getButton(down))
        v.y -= 1.0f;
    if(d->getButton(right))
        v.x += 1.0f;
    if(d->getButton(left))
        v.x -= 1.0f;

    return v;
}
