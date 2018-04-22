#ifdef _DEBUG
#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include "Box2D/Box2D.h"
#include "Rect.h"
#include "VulkanInterface.h"
#include <vector>

//Debug draw class for drawing Box2D stuff
class DebugDraw : public b2Draw
{
    VulkanInterface* m_vulkan;
    std::vector<DbgVertex> m_vertices;
    std::vector<DbgVertex> m_lineVertices;
    std::vector<DbgVertex> m_pointVertices;
    std::vector<uint16_t> m_indices;

    DebugDraw() {};

public:

    DebugDraw(VulkanInterface* vulkan);
    void flush();   //Finish rendering

    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
    void DrawTransform(const b2Transform& xf);
    void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
    void DrawString(int x, int y, const char* string, ...);
    void DrawString(const b2Vec2& p, const char* string, ...);
    void DrawAABB(b2AABB* aabb, const b2Color& color);
    void Draw3DPolygon(const Vec3* vertices, int32 vertexCount, const b2Color& color);
    void Draw3DSegment(const Vec3& p1, const Vec3& p2, const b2Color& color);

    float fillAlpha;
    float fillMul;
    float outlineAlpha;
};


#endif //DEBUG_DRAW_H
#endif //_DEBUG
