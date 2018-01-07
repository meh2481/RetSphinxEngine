#include "Random.h"
#include "mtrand.h"

static MTRand_int32 irand;
static MTRand_closed drand;

int Random::random()
{
    return irand();
}

int Random::random(int min, int max)
{
    if(min == max) return min;
    if(min > max)
    {
        int temp = min;
        min = max;
        max = temp;
    }
    int diff = max - min + 1;
    return(irand() % diff + min);
}

int Random::random(int max)
{
    return random(0, max);
}

float Random::randomFloat()
{
    return (float)drand();
}

float Random::randomFloat(float min, float max)
{
    if(min == max) return min;
    if(min > max)
    {
        float temp = min;
        min = max;
        max = temp;
    }
    return (float)(drand() * (max - min) + min);
}

float Random::randomFloat(float max)
{
    return randomFloat(0.0f, max);
}

void Random::seed(unsigned long seed)
{
    irand.seed(seed);
    drand.seed(seed);
}
