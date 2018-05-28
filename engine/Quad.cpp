#include "Quad.h"
#include "RenderState.h"
#include "glm/glm.hpp"
#include <vector>
#include "VulkanInterface.h"

//HACK
extern VulkanInterface* g_vulkan;

namespace Draw
{
    void drawQuad(Image* img, const Vec3& pos1, const Vec3& pos2, const Vec3& pos3, const Vec3& pos4)
    {
        //CCW winding
        Vertex v = {};

        //Upper left
        v.pos = pos1;
        v.color.r = 1.0f;
        v.color.g = 1.0f;
        v.color.b = 1.0f;
        v.color.a = 1.0f;
        v.texCoord.x = img->uv[0];
        v.texCoord.y = img->uv[1];
        g_vulkan->quadIndices.push_back((uint16_t)g_vulkan->quadVertices.size());
        g_vulkan->quadVertices.push_back(v);

        //Lower left
        v.pos = pos3;
        v.texCoord.x = img->uv[4];
        v.texCoord.y = img->uv[5];
        g_vulkan->quadIndices.push_back((uint16_t)g_vulkan->quadVertices.size());
        g_vulkan->quadVertices.push_back(v);

        //Upper right
        v.pos = pos2;
        v.texCoord.x = img->uv[2];
        v.texCoord.y = img->uv[3];
        g_vulkan->quadIndices.push_back((uint16_t)g_vulkan->quadVertices.size());
        g_vulkan->quadVertices.push_back(v);

        //Second triangle
        //Lower right
        v.pos = pos4;
        v.texCoord.x = img->uv[6];
        v.texCoord.y = img->uv[7];
        g_vulkan->quadIndices.push_back((uint16_t)g_vulkan->quadVertices.size());
        g_vulkan->quadVertices.push_back(v);

        //Upper right
        g_vulkan->quadIndices.push_back((uint16_t)g_vulkan->quadVertices.size() - 2);

        //Lower left
        g_vulkan->quadIndices.push_back((uint16_t)g_vulkan->quadVertices.size() - 3);
    }
}
