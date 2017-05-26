#pragma once

class Interpolation
{
protected:
	float* val;
	float start;
	float dest;
	float time;
	float curTime;

	Interpolation() {};
public:
	Interpolation(float* v, float d, float t) : val(v), dest(d), time(t), curTime(0), start(*v) {};

	virtual bool update(float dt) = 0;
};