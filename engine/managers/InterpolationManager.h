#pragma once
#include <vector>

typedef enum
{
	LINEAR,
	BEZIER
} InterpType;

class Interpolation;

class InterpolationManager
{
	std::vector<Interpolation*> interpolations;

public:
	InterpolationManager();
	~InterpolationManager();

	void update(float dt);

	void interpolate(float* val, float end, float time, InterpType type = BEZIER);
	void clear();
};
