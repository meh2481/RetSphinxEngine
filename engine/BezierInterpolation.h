#pragma once
#include "Interpolation.h"
#include "Rect.h"

class BezierInterpolation : public Interpolation
{

	BezierInterpolation() {};
public:
	Vec2 p1, p2;
	BezierInterpolation(float* v, float d, float t);

	bool update(float dt);
};
