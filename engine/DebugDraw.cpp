/*
    RetSphinxEngine source - DebugDraw.cpp
    Copyright (c) 2017 Mark Hutcheson
*/

#include "DebugDraw.h"
#include "opengl-api.h"
#include "Box2D/Box2D.h"
#include "Quad.h"

DebugDraw::DebugDraw(RenderState renderState)
{
    m_renderState = renderState;
    m_colorUniformId = glGetUniformLocation(m_renderState.programId, "col");
    m_posAttribId = glGetAttribLocation(m_renderState.programId, "position");
    assert(m_colorUniformId >= 0);
    assert(m_posAttribId >= 0);
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
    unsigned int numTriangles = vertexCount - 2;
    float* data = new float[numTriangles * 6];
    const float col[] = {
        color.r * fillMul,
        color.g * fillMul,
        color.b * fillMul,
        color.a * fillAlpha
    };
    //Fill out triangles
    for(unsigned int i = 0; i < numTriangles; i++)
    {
        data[i * 6] = vertices[0].x;
        data[i * 6 + 1] = vertices[0].y;
        data[i * 6 + 2] = vertices[i + 1].x;
        data[i * 6 + 3] = vertices[i + 1].y;
        data[i * 6 + 4] = vertices[i + 2].x;
        data[i * 6 + 5] = vertices[i + 2].y;
    }
    //for(int i = 0; i < vertexCount; i++)
    //{
    //    data[i * 2] = vertices[i].x;
    //    data[i * 2 + 1] = vertices[i].y;
    //}
    glUniform4fv(m_colorUniformId, 1, col);
    Draw::drawHelper(data, sizeof(float) * numTriangles * 6, 2, numTriangles*3, GL_TRIANGLES, m_posAttribId);

    //Fill in outside
    DrawPolygon(vertices, vertexCount, color);

    delete[] data;
}

const int NUM_SEGMENTS = 16;
const float ANGLE_INCREMENT = 2.0f * b2_pi / (float)NUM_SEGMENTS;
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
    //Draw filled circle in center
    float data[NUM_SEGMENTS * 6];
    const float col[] = {
        color.r * fillMul,
        color.g * fillMul,
        color.b * fillMul,
        color.a * fillAlpha
    };
    float angle = 0.0f;
    for(int i = 0; i < NUM_SEGMENTS; i++)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(angle), sinf(angle));
        data[i * 6] = v.x;
        data[i * 6 + 1] = v.y;
        angle += ANGLE_INCREMENT;
        v = center + radius * b2Vec2(cosf(angle), sinf(angle));
        data[i * 6 + 2] = v.x;
        data[i * 6 + 3] = v.y;
        data[i * 6 + 4] = center.x;
        data[i * 6 + 5] = center.y;
    }
    glUniform4fv(m_colorUniformId, 1, col);
    Draw::drawHelper(data, sizeof(float) * NUM_SEGMENTS * 6, 2, NUM_SEGMENTS * 3, GL_TRIANGLES, m_posAttribId);

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
    glUniform4fv(m_colorUniformId, 1, col);
    Draw::drawHelper(data, sizeof(float) * 4, 2, 2, GL_LINES, m_posAttribId);
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
    glPointSize(size);
    glUniform4fv(m_colorUniformId, 1, col);
    Draw::drawHelper(data, sizeof(float) * 2, 2, 1, GL_POINTS, m_posAttribId);
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

#ifdef _DEBUG
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
    glUniform4fv(m_colorUniformId, 1, col);
    Draw::drawHelper(data, sizeof(float) * 6, 3, 2, GL_LINES, m_posAttribId);
}

void DebugDraw::Draw3DPolygon(const Vec3* vertices, int32 vertexCount, const b2Color& color)
{
    //Draw filled center
    float* data = new float[vertexCount * 3];
    const float col[] = {
        color.r * fillMul,
        color.g * fillMul,
        color.b * fillMul,
        color.a * fillAlpha
    };
    for(int i = 0; i < vertexCount; i++)
    {
        data[i * 3] = vertices[i].x;
        data[i * 3 + 1] = vertices[i].y;
        data[i * 3 + 2] = vertices[i].z;
    }
    glUniform4fv(m_colorUniformId, 1, col);
    Draw::drawHelper(data, sizeof(float) * vertexCount * 3, 3, vertexCount, GL_TRIANGLE_FAN, m_posAttribId);

    //Fill in outside
    //Draw sides
    for(int i = 0; i < vertexCount - 1; i++)
        Draw3DSegment(vertices[i], vertices[i + 1], color);
    //Draw last side
    Draw3DSegment(vertices[0], vertices[vertexCount - 1], color);

    delete[] data;
}
#endif
