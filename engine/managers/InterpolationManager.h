#pragma once
#include <vector>

typedef enum
{
    LINEAR,
    BEZIER
} InterpType;

typedef enum
{
    REPEAT,
    PINGPONG
} InterpRepeat;

#define REPEAT_FOREVER -1

class Interpolation;

class InterpolationManager
{
    typedef struct
    {
        Interpolation* interp;
        InterpRepeat repeat;
        int count;
    } InterpHolder;

    std::vector<InterpHolder> interpolations;

public:
    InterpolationManager();
    ~InterpolationManager();

    void update(float dt);

    void interpolate(float* val, float end, float time, InterpType type = BEZIER, int repeatCount = 0, InterpRepeat repeatType = REPEAT);
    void clear();
};
