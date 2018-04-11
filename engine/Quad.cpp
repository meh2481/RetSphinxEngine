#include "Quad.h"
#include "RenderState.h"
#include "glm/glm.hpp"
#include <vector>

namespace Draw
{
    static const int MAX_COUNT = 4096;
    static std::vector<glm::mat4> model;
    static std::vector<glm::mat4> view;
    static std::vector<glm::mat4> projection;
    static int program;
    static int modelId;
    static int viewId;
    static int projectionId;

    void drawQuad(Quad* q, RenderState* state)
    {
        //glm::mat4 mvp = state->projection * state->view * state->model;

        //glUseProgram(state->programId);
        //glUniformMatrix4fv(modelId, 1, false, &state->model[0][0]);
        //glUniformMatrix4fv(viewId, 1, false, &state->view[0][0]);
        //glUniformMatrix4fv(projectionId, 1, false, &state->projection[0][0]);
        // tell opengl to use the generated texture



        //glBindTexture(GL_TEXTURE_2D, q->tex.tex.tex);
        //glVertexPointer(2, GL_FLOAT, 0, q->pos);
        //glTexCoordPointer(2, GL_FLOAT, 0, q->tex.uv);



        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
#ifdef _DEBUG
    #define NUM_BUFS 5
    static unsigned int vertBuf[NUM_BUFS];
    static unsigned int vertArray[NUM_BUFS];
    static std::vector<float> triangles2d;
    static std::vector<float> lines2d;
    static std::vector<float> points2d;
    static std::vector<float> triangles3d;
    static std::vector<float> lines3d;
    void drawHelper(const float* data, unsigned int len, int numPer, int count, int type)
    {
        assert(len % sizeof(float) == 0);
        int num = len / sizeof(float);
        if(numPer == 2)
        {
            if(type == 1)//TODO
            {
                for(int i = 0; i < num; i++)
                    lines2d.push_back(data[i]);
            }
            else if(type == 2)//TODO
            {
                for(int i = 0; i < num; i++)
                    triangles2d.push_back(data[i]);
            }
            else if(type == 3)//TODO
            {
                for(int i = 0; i < num; i++)
                    points2d.push_back(data[i]);
            }
            else
                assert(false);
        }
        else if(numPer == 3)//TODO
        {
            if(type == 1)
            {
                for(int i = 0; i < num; i++)
                    lines3d.push_back(data[i]);
            }
            else if(type == 2)//TODO
            {
                for(int i = 0; i < num; i++)
                    triangles3d.push_back(data[i]);
            }
            else
                assert(false);
        }
        else
            assert(false);
    }
#endif

    void init(int programId)
    {
        program = programId;

        //modelId = glGetUniformLocation(programId, "model");
        //viewId = glGetUniformLocation(programId, "view");
        //projectionId = glGetUniformLocation(programId, "projection");

#ifdef _DEBUG
        //model.reserve(MAX_COUNT);
        //view.reserve(MAX_COUNT);
        //projection.reserve(MAX_COUNT);
        triangles2d.reserve(MAX_COUNT);
        lines2d.reserve(MAX_COUNT);
        points2d.reserve(MAX_COUNT);
        triangles3d.reserve(MAX_COUNT);
        lines3d.reserve(MAX_COUNT);

        //glGenVertexArrays(NUM_BUFS, vertArray);
        //for(int i = 0; i < NUM_BUFS; i++)
        //{
        //    glBindVertexArray(vertArray[i]);
        //    glGenBuffers(1, &vertBuf[i]);

        //    glBindBuffer(GL_ARRAY_BUFFER, vertBuf[i]);
        //    glBufferData(GL_ARRAY_BUFFER, MAX_COUNT * sizeof(float), NULL, GL_STREAM_DRAW);
        //    glEnableVertexAttribArray(0);   //Assumes position is at attribute location 0 in the debug shader
        //    //First three bufs are 2, last two are 3
        //    glVertexAttribPointer(0, (i < 3)?(2):(3), GL_FLOAT, GL_FALSE, 0, (void*)0);   //Assumes position is at attribute location 0 in the debug shader
        //}
        //glBindVertexArray(0);
#endif
    }

    void shutdown()
    {
#ifdef _DEBUG
        //glDeleteVertexArrays(NUM_BUFS, vertArray);
        //glDeleteBuffers(NUM_BUFS, vertBuf);
#endif

    }

    void flush()
    {
#ifdef _DEBUG
        //Make sure we don't go over bounds
        assert(triangles2d.size() <= MAX_COUNT);
        assert(lines2d.size() <= MAX_COUNT);
        assert(points2d.size() <= MAX_COUNT);
        assert(triangles3d.size() <= MAX_COUNT);
        assert(lines3d.size() <= MAX_COUNT);

        //Draw 2d triangles
        //glBindBuffer(GL_ARRAY_BUFFER, vertBuf[0]);
        //glBufferData(GL_ARRAY_BUFFER, triangles2d.size() * sizeof(float), triangles2d.data(), GL_STREAM_DRAW);
        //glBindVertexArray(vertArray[0]);
        //assert(triangles2d.size() % 2 == 0);
        //glDrawArrays(GL_TRIANGLES, 0, triangles2d.size() / 2);

        ////Draw 2d lines
        //glBindBuffer(GL_ARRAY_BUFFER, vertBuf[1]);
        //glBufferData(GL_ARRAY_BUFFER, lines2d.size() * sizeof(float), lines2d.data(), GL_STREAM_DRAW);
        //glBindVertexArray(vertArray[1]);
        //assert(lines2d.size() % 2 == 0);
        //glDrawArrays(GL_LINES, 0, lines2d.size() / 2);

        ////Draw 2d points
        //glBindBuffer(GL_ARRAY_BUFFER, vertBuf[2]);
        //glBufferData(GL_ARRAY_BUFFER, points2d.size() * sizeof(float), points2d.data(), GL_STREAM_DRAW);
        //glBindVertexArray(vertArray[2]);
        //assert(points2d.size() % 2 == 0);
        //glDrawArrays(GL_POINTS, 0, points2d.size() / 2);

        ////Draw 3d triangles
        //glBindBuffer(GL_ARRAY_BUFFER, vertBuf[3]);
        //glBufferData(GL_ARRAY_BUFFER, triangles3d.size() * sizeof(float), triangles3d.data(), GL_STREAM_DRAW);
        //glBindVertexArray(vertArray[3]);
        //assert(triangles3d.size() % 3 == 0);
        //glDrawArrays(GL_TRIANGLES, 0, triangles3d.size() / 3);

        ////Draw 3d lines
        //glBindBuffer(GL_ARRAY_BUFFER, vertBuf[4]);
        //glBufferData(GL_ARRAY_BUFFER, lines3d.size() * sizeof(float), lines3d.data(), GL_STREAM_DRAW);
        //glBindVertexArray(vertArray[4]);
        //assert(lines3d.size() % 3 == 0);
        //glDrawArrays(GL_LINES, 0, lines3d.size() / 3);

        ////Done
        //glBindVertexArray(0);

        //Clear
        triangles2d.clear();
        lines2d.clear();
        points2d.clear();
        triangles3d.clear();
        lines3d.clear();
#endif
    }
}
