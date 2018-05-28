#pragma once
#include "Rect.h"
#include <vulkan/vulkan.h>

class Texture
{
public:
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;

    unsigned int width;
    unsigned int height;
};

class Image
{
public:
    Texture tex;
    float uv[8];
};

class RenderState;

namespace Draw
{
    void drawQuad(Image* img, const Vec3& pos1, const Vec3& pos2, const Vec3& pos3, const Vec3& pos4);
}
