/*
    RetSphinxEngine source - DebugDraw.cpp
    Copyright (c) 2017 Mark Hutcheson
*/
#ifdef _DEBUG   //Only debug draw in debug mode

#include "DebugDraw.h"
#include "Box2D/Box2D.h"
#include "VulkanInterface.h"
#include "Logger.h"

static const int NUM_SEGMENTS = 16;    //Must be greater than 2
static const float ANGLE_INCREMENT = 2.0f * b2_pi / (float)NUM_SEGMENTS;

DebugDraw::DebugDraw(VulkanInterface* vulkan)
{
    m_vulkan = vulkan;
}

void DebugDraw::flush()
{
    //Push to vulkan
    m_vulkan->vertices.clear();
    m_vulkan->indices.clear();
    for(auto i : m_vertices)
        m_vulkan->vertices.push_back(i);
    for(auto i : m_indices)
        m_vulkan->indices.push_back(i);

    //Clear for next pass
    m_vertices.clear();
    m_indices.clear();
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    //Draw sides
    for(int i = 0; i < vertexCount - 1; i++)
        DrawSegment(vertices[i], vertices[i + 1], color);
    //Draw last side
    DrawSegment(vertices[0], vertices[vertexCount - 1], color);
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    //Draw filled center
    assert(vertexCount >= 3);

    //Fill out triangles
    uint16_t idx0 = (uint16_t)m_vertices.size();  //Index of starting vertex
    uint16_t curIdx = idx0;

    //Push first triangle
    DbgVertex v = {};
    v.pos.x = vertices[0].x;
    v.pos.y = vertices[0].y;
    v.color.r = color.r * fillMul;
    v.color.g = color.g * fillMul;
    v.color.b = color.b * fillMul;
    v.color.a = color.a * fillAlpha;
    m_vertices.push_back(v);
    m_indices.push_back(idx0);
    v.pos.x = vertices[1].x;
    v.pos.y = vertices[1].y;
    m_vertices.push_back(v);
    m_indices.push_back(++curIdx);
    v.pos.x = vertices[2].x;
    v.pos.y = vertices[2].y;
    m_vertices.push_back(v);
    m_indices.push_back(++curIdx);

    //Fill out the rest
    for(int32 i = 3; i < vertexCount; i++)
    {
        //Add one new vertex
        v.pos.x = vertices[i].x;
        v.pos.y = vertices[i].y;
        m_vertices.push_back(v);
        //This triangle is formed by the first vertex, the previous vertex, and this vertex
        m_indices.push_back(idx0);
        m_indices.push_back(curIdx);
        m_indices.push_back(++curIdx);
    }

    //Fill in outside
    DrawPolygon(vertices, vertexCount, color);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
    float angle = 0.0f;
    for(int i = 0; i < NUM_SEGMENTS; ++i)
    {
        b2Vec2 v1 = center + radius * b2Vec2(cosf(angle), sinf(angle));
        b2Vec2 v2 = center + radius * b2Vec2(cosf(angle + ANGLE_INCREMENT), sinf(angle + ANGLE_INCREMENT));
        DrawSegment(v1, v2, color);
        angle += ANGLE_INCREMENT;
    }
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
    //Fill out triangles
    uint16_t idx0 = (uint16_t)m_vertices.size();  //Index of starting vertex

    //Push center vertex
    DbgVertex v = {};
    v.pos.x = center.x;
    v.pos.y = center.y;
    v.color.r = color.r * fillMul;
    v.color.g = color.g * fillMul;
    v.color.b = color.b * fillMul;
    v.color.a = color.a * fillAlpha;
    m_vertices.push_back(v);

    //Push second vertex
    float angle = 0.0f;
    b2Vec2 pt = center + radius * b2Vec2(cosf(angle), sinf(angle));
    angle += ANGLE_INCREMENT;
    v.pos.x = pt.x;
    v.pos.y = pt.y;
    m_vertices.push_back(v);
    uint16_t idx1 = idx0 + 1;	//Index of second vertex
    uint16_t curIdx = idx1;

    for(int i = 1; i < NUM_SEGMENTS; i++)
    {
        pt = center + radius * b2Vec2(cosf(angle), sinf(angle));
        v.pos.x = pt.x;
        v.pos.y = pt.y;
        m_vertices.push_back(v);
        angle += ANGLE_INCREMENT;
        m_indices.push_back(idx0);
        m_indices.push_back(curIdx);
        m_indices.push_back(++curIdx);
    }

    //Push last triangle
    m_indices.push_back(idx0);
    m_indices.push_back(idx1);
    m_indices.push_back(curIdx);

    //Draw circle
    DrawCircle(center, radius, color);

    //Draw axis
    b2Vec2 p = center + radius * axis;
    DrawSegment(center, p, color);
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    const float data[] = {
        p1.x,
        p1.y,
        p2.x,
        p2.y
    };
    const float col[] = {
        color.r,
        color.g,
        color.b,
        color.a * outlineAlpha
    };
    //glUniform4fv(m_colorUniformId, 1, col);
    //Draw::drawHelper(data, sizeof(float) * 4, 2, 2, GL_LINES);
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
    //Don't know what this is, don't really care
}

void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
    const float data[] = {
        p.x,
        p.y
    };
    const float col[] = {
        color.r,
        color.g,
        color.b,
        color.a * outlineAlpha
    };
    //glPointSize(size);
    //glUniform4fv(m_colorUniformId, 1, col);
    //Draw::drawHelper(data, sizeof(float) * 2, 2, 1, GL_POINTS);
}

void DebugDraw::DrawString(int x, int y, const char *string, ...)
{
    //Don't care about drawing strings
}

void DebugDraw::DrawString(const b2Vec2& p, const char *string, ...)
{
    //Don't care about drawing strings
}

void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
    //Don't care about drawing AABBs, as they're generally useless to see
}

void DebugDraw::Draw3DSegment(const Vec3& p1, const Vec3& p2, const b2Color& color)
{
    const float data[] = {
        p1.x,
        p1.y,
        p1.z,
        p2.x,
        p2.y,
        p2.z
    };
    const float col[] = {
        color.r,
        color.g,
        color.b,
        color.a * outlineAlpha
    };
    //glUniform4fv(m_colorUniformId, 1, col);
    //Draw::drawHelper(data, sizeof(float) * 6, 3, 2, GL_LINES);
}

void DebugDraw::Draw3DPolygon(const Vec3* vertices, int32 vertexCount, const b2Color& color)
{
    //Draw filled center
    assert(vertexCount >= 3);
    unsigned int numTriangles = vertexCount - 2;
    float* data = new float[numTriangles * 9];
    const float col[] = {
        color.r * fillMul,
        color.g * fillMul,
        color.b * fillMul,
        color.a * fillAlpha
    };
    //Fill out triangles
    for(unsigned int i = 0; i < numTriangles; i++)
    {
        data[i * 9] = vertices[0].x;
        data[i * 9 + 1] = vertices[0].y;
        data[i * 9 + 2] = vertices[0].z;

        data[i * 9 + 3] = vertices[i + 1].x;
        data[i * 9 + 4] = vertices[i + 1].y;
        data[i * 9 + 5] = vertices[i + 1].z;

        data[i * 9 + 6] = vertices[i + 2].x;
        data[i * 9 + 7] = vertices[i + 2].y;
        data[i * 9 + 8] = vertices[i + 2].z;
    }
    //for(int i = 0; i < vertexCount; i++)
    //{
    //    data[i * 3] = vertices[i].x;
    //    data[i * 3 + 1] = vertices[i].y;
    //    data[i * 3 + 2] = vertices[i].z;
    //}
    //glUniform4fv(m_colorUniformId, 1, col);
    //Draw::drawHelper(data, sizeof(float) * numTriangles * 9, 3, numTriangles * 3, GL_TRIANGLES);

    //Fill in outside
    //Draw sides
    for(int i = 0; i < vertexCount - 1; i++)
        Draw3DSegment(vertices[i], vertices[i + 1], color);
    //Draw last side
    Draw3DSegment(vertices[0], vertices[vertexCount - 1], color);

    delete[] data;
}
#endif //_DEBUG
