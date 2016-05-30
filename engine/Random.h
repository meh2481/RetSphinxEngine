#pragma once
class Random
{
	//Don't allow instantiations
	Random() {};
	~Random() {};
public:
	static int random();
	static int random(int min, int max);
	static int random(int max);					//Between 0 and max
	static float randomFloat();										//Between 0 and 1
	static float randomFloat(float min, float max);
	static float randomFloat(float max);							//Between 0 and max
	static void seed(unsigned long seed);
};

