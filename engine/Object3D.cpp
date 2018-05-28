/*
 RetSphinxEngine source - Object3D.cpp
 Copyright (c) 2013 Mark Hutcheson
*/
#include "Object3D.h"
#include <assert.h>
#include "ResourceTypes.h"
#include "Quad.h"
#include "RenderState.h"

Object3D::Object3D(unsigned char* data, Image* tex, RenderState* renderState)
{
    num = 0;
    //m_tex = tex->tex.tex;
    m_renderState = renderState;

    //modelId = glGetUniformLocation(renderState->programId, "model");
    //viewId = glGetUniformLocation(renderState->programId, "view");
    //projectionId = glGetUniformLocation(renderState->programId, "proj");

    _fromData(data, tex);
}

Object3D::~Object3D()
{
    //glDeleteBuffers(1, &vertBuf);
}

void Object3D::_fromData(unsigned char* data, Image* tex)
{
    MeshHeader* header = (MeshHeader*)data;
    num = header->numVertices;

    //Copy all data, since we'll be modifying UVs on the fly
    unsigned int len = sizeof(float) * 2 * num + sizeof(float) * 3 * num + sizeof(float) * 3 * num;
    float* bufferData = (float*)malloc(len);
    memcpy(bufferData, (void*)((size_t)data + sizeof(MeshHeader)), len);

    //Assign pointers to vertex/texture/normal data
    unsigned long vertexPtr = 0;
    unsigned long texCoordPtr = vertexPtr + sizeof(float) * 3 * num;
    unsigned long normalPtr = texCoordPtr + sizeof(float) * 2 * num;

    //Offset UV coordinates by tex->uv
    float* uv = (float*)(texCoordPtr + (unsigned long)bufferData);
    float left = tex->uv[0];
    float right = tex->uv[2];
    float top = tex->uv[5];
    float bottom = tex->uv[1];
    float width = right - left;
    float height = bottom - top;
    for(unsigned int i = 0; i < num; i++)
    {
        *uv++ = (*uv * width + left);
        *uv++ = (*uv * height + top);
    }

    //Gen vertex buffer
    //GLint position = glGetAttribLocation(m_renderState->programId, "position");
    //GLint texcoord = glGetAttribLocation(m_renderState->programId, "texcoord");
    //GLint normal = glGetAttribLocation(m_renderState->programId, "normal");

    //glGenVertexArrays(1, &vertArray);
    //glBindVertexArray(vertArray);
    //glGenBuffers(1, &vertBuf);
    //glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
    //glBufferData(GL_ARRAY_BUFFER, len, bufferData, GL_STATIC_DRAW);

    //if(position >= 0)
    //{
    //    glEnableVertexAttribArray(position);
    //    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, (void*)vertexPtr);
    //}

    //if(texcoord >= 0)
    //{
    //    glEnableVertexAttribArray(texcoord);
    //    glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 0, (void*)texCoordPtr);
    //}

    //if(normal >= 0) //This can be -1, since for now the variable is compiled out
    //{
    //    glEnableVertexAttribArray(normal);
    //    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)normalPtr);
    //}

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);

    //Free buffer data
    free(bufferData);
}

void Object3D::render(RenderState renderState)
{
    //assert(m_tex);
    //assert(num > 0);

    //m_renderState->model = renderState.model;
    //m_renderState->proj = renderState.proj;
    //m_renderState->view = renderState.view;

    //glUseProgram(m_renderState->programId);

    //glUniformMatrix4fv(modelId, 1, false, &m_renderState->model[0][0]);
    //glUniformMatrix4fv(viewId, 1, false, &m_renderState->view[0][0]);
    //glUniformMatrix4fv(projectionId, 1, false, &m_renderState->proj[0][0]);

    //glBindTexture(GL_TEXTURE_2D, m_tex);    //Bind texture

    ////Set pointers
    //glBindVertexArray(vertArray);
    //glDrawArrays(GL_TRIANGLES, 0, num);    //Render
    //glBindVertexArray(0);

    //glUseProgram(renderState.programId);
}
