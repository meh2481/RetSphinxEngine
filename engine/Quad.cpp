#include "Quad.h"
#include "RenderState.h"
#include "glm/glm.hpp"
#include <vector>
#include "VulkanInterface.h"

//HACK
extern VulkanInterface* g_vulkan;

namespace Draw
{
    void drawQuad(Quad* q)
    {
        //CCW winding
        Vertex v = {};

        //Upper left
        v.pos.x = q->pos[0];
        v.pos.y = q->pos[1];
        v.pos.z = 0.0f;
        v.color.r = 1.0f;
        v.color.g = 1.0f;
        v.color.b = 1.0f;
        v.color.a = 1.0f;
        v.texCoord.x = q->tex.uv[0];
        v.texCoord.y = q->tex.uv[1];
        g_vulkan->quadIndices.push_back(g_vulkan->quadVertices.size());
        g_vulkan->quadVertices.push_back(v);

        //Lower left
        v.pos.x = q->pos[4];
        v.pos.y = q->pos[5];
        v.texCoord.x = q->tex.uv[4];
        v.texCoord.y = q->tex.uv[5];
        g_vulkan->quadIndices.push_back(g_vulkan->quadVertices.size());
        g_vulkan->quadVertices.push_back(v);

        //Upper right
        v.pos.x = q->pos[2];
        v.pos.y = q->pos[3];
        v.texCoord.x = q->tex.uv[2];
        v.texCoord.y = q->tex.uv[3];
        g_vulkan->quadIndices.push_back(g_vulkan->quadVertices.size());
        g_vulkan->quadVertices.push_back(v);

        //Second triangle
        //Lower right
        v.pos.x = q->pos[6];
        v.pos.y = q->pos[7];
        v.texCoord.x = q->tex.uv[6];
        v.texCoord.y = q->tex.uv[7];
        g_vulkan->quadIndices.push_back(g_vulkan->quadVertices.size());
        g_vulkan->quadVertices.push_back(v);

        //Upper right
        g_vulkan->quadIndices.push_back(g_vulkan->quadVertices.size() - 2);

        //Lower left
        g_vulkan->quadIndices.push_back(g_vulkan->quadVertices.size() - 3);
    }
}
