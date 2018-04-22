/*
    RetSphinxEngine source - DebugDraw.cpp
    Copyright (c) 2017 Mark Hutcheson
*/
#ifdef _DEBUG   //Only debug draw in debug mode

#include "DebugDraw.h"
#include "Box2D/Box2D.h"
#include "VulkanInterface.h"
#include "Logger.h"

#define START_VERT_DEPTH 0.0f;
#define VERT_DEPTH_INCR  0.0006f;

static const int NUM_SEGMENTS = 16;    //Must be greater than 2
static const float ANGLE_INCREMENT = 2.0f * b2_pi / (float)NUM_SEGMENTS;
static float vertexDepth = START_VERT_DEPTH;

DebugDraw::DebugDraw(VulkanInterface* vulkan)
{
    m_vulkan = vulkan;
}

void DebugDraw::flush()
{
    //Push to vulkan
    m_vulkan->dbgPolyVertices.clear();
    m_vulkan->dbgPolyIndices.clear();
    for(auto i : m_vertices)
        m_vulkan->dbgPolyVertices.push_back(i);
    for(auto i : m_indices)
        m_vulkan->dbgPolyIndices.push_back(i);
    m_vulkan->polyLineIdx = (uint32_t)m_vulkan->dbgPolyVertices.size();
    for(auto i : m_lineVertices)
    {
        if(i.pos.z == FLT_MIN)
            i.pos.z = vertexDepth + VERT_DEPTH_INCR;
        m_vulkan->dbgPolyVertices.push_back(i);
    }
    m_vulkan->polyPointIdx = (uint32_t)m_vulkan->dbgPolyVertices.size();
    for(auto i : m_pointVertices)
    {
        i.pos.z = vertexDepth + VERT_DEPTH_INCR;
        m_vulkan->dbgPolyVertices.push_back(i);
    }

    //Clear for next pass
    m_vertices.clear();
    m_indices.clear();
    m_lineVertices.clear();
    m_pointVertices.clear();
    vertexDepth = START_VERT_DEPTH;
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
    v.pos.z = vertexDepth;
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
    vertexDepth += VERT_DEPTH_INCR;
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
    v.pos.z = vertexDepth;
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

    vertexDepth += VERT_DEPTH_INCR;
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    Vec3 pt1;
    Vec3 pt2;
    pt1.x = p1.x;
    pt1.y = p1.y;
    pt2.x = p2.x;
    pt2.y = p2.y;
    pt1.z = pt2.z = FLT_MIN;
    Draw3DSegment(pt1, pt2, color);
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
    //Don't know what this is, don't really care
}

void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
    DbgVertex v = {};
    v.pos.x = p.x;
    v.pos.y = p.y;
    v.color.r = color.r;
    v.color.g = color.g;
    v.color.b = color.b;
    v.color.a = color.a * outlineAlpha;
    m_pointVertices.push_back(v);
    //glPointSize(size);    //Setting this to 4.0 in the shader, since it seems that it stays constant
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
    DbgVertex v = {};
    v.pos.x = p1.x;
    v.pos.y = p1.y;
    v.pos.z = p1.z;
    v.color.r = color.r;
    v.color.g = color.g;
    v.color.b = color.b;
    v.color.a = color.a * outlineAlpha;
    m_lineVertices.push_back(v);
    v.pos.x = p2.x;
    v.pos.y = p2.y;
    v.pos.z = p2.z;
    m_lineVertices.push_back(v);
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
