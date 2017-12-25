/*
	RetSphinxEngine source - DebugDraw.cpp
	Copyright (c) 2017 Mark Hutcheson
*/

#include "Engine.h"
#include "opengl-api.h"

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
	float* data = new float[vertexCount*2];
	const float col[] = {
		color.r * 0.5,
		color.g * 0.5,
		color.b * 0.5,
		color.a * 0.5
	};
	for(int i = 0; i < vertexCount; i++)
	{
		data[i * 2] = vertices[i].x;
		data[i * 2 + 1] = vertices[i].y;
	}
	glUniform4fv(uniformId, 1, col);
	glVertexPointer(2, GL_FLOAT, 0, data);
	glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

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
	float data[NUM_SEGMENTS * 2];
	const float col[] = {
		color.r * 0.5,
		color.g * 0.5,
		color.b * 0.5,
		color.a * 0.5
	};
	float angle = 0.0f;
	for(int i = 0; i < NUM_SEGMENTS; i++)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(angle), sinf(angle));
		data[i * 2] = v.x;
		data[i * 2 + 1] = v.y;
		angle += ANGLE_INCREMENT;
	}
	glUniform4fv(uniformId, 1, col);
	glVertexPointer(2, GL_FLOAT, 0, data);
	glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SEGMENTS);

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
		color.a
	};
	glUniform4fv(uniformId, 1, col);
	glVertexPointer(2, GL_FLOAT, 0, data);
	glDrawArrays(GL_LINES, 0, 2);
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
	//Don't know what this is, don't really care
}

void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
	//For some reason this is the wrong size
	DrawCircle(p, size * 0.025, color);
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
