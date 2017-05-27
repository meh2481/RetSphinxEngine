#pragma once
#include "Interpolation.h"
#include "Rect.h"

class BezierInterpolation : public Interpolation
{
	float p2;
	BezierInterpolation() {};
public:
	BezierInterpolation(float* v, float d, float t);

	bool update(float dt);
};
