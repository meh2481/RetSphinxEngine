#pragma once

class Texture
{
public:
    unsigned int tex;
    unsigned int width;
    unsigned int height;
};

class Image
{
public:
    Texture tex;
    float uv[8];
};

class Quad
{
public:
    Image tex;
    float pos[8];
};

class RenderState;

namespace Draw
{
    void init(int programId);
    void shutdown();
    void drawQuad(Quad* q, RenderState* state);    //Yeeeeaaaaaaah

    void drawHelper(const float* data, unsigned int len, int numPer, int count, int type);
}
