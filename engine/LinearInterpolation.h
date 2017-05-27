#pragma once
#include "Interpolation.h"

class LinearInterpolation : public Interpolation
{

	LinearInterpolation() {};
public:
	LinearInterpolation(float* v, float d, float t);

	bool update(float dt);
};
