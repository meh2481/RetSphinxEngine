#include "Quad.h"
#include "SDL_opengl.h"
#include "opengl-api.h"
#include "RenderState.h"
#include "glm/glm.hpp"
#include <vector>

namespace Draw
{
    static const int MAX_COUNT = 1024;
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
        glBindTexture(GL_TEXTURE_2D, q->tex.tex.tex);
        glVertexPointer(2, GL_FLOAT, 0, q->pos);
        glTexCoordPointer(2, GL_FLOAT, 0, q->tex.uv);



        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
#ifdef _DEBUG
    static unsigned int vertBuf;
    static unsigned int vertArray;
    static int curNumPer = 2;
    void drawHelper(const float* data, unsigned int len, int numPer, int count, int type)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
        glBufferData(GL_ARRAY_BUFFER, len, data, GL_STREAM_DRAW);

        glBindVertexArray(vertArray);
        if(numPer != curNumPer) //Flip-flop this. Kinda mad hacky but whatevs
        {
            glVertexAttribPointer(0, numPer, GL_FLOAT, GL_FALSE, 0, (void*)0);   //Assumes position is at attribute location 0 in the debug shader
            curNumPer = numPer;
        }
        glDrawArrays(type, 0, count);
        glBindVertexArray(0);
    }
#endif

    void init(int programId)
    {
        program = programId;
        model.reserve(MAX_COUNT);
        view.reserve(MAX_COUNT);
        projection.reserve(MAX_COUNT);

        modelId = glGetUniformLocation(programId, "model");
        viewId = glGetUniformLocation(programId, "view");
        projectionId = glGetUniformLocation(programId, "projection");
#ifdef _DEBUG
        glGenVertexArrays(1, &vertArray);
        glBindVertexArray(vertArray);
        glGenBuffers(1, &vertBuf);

        glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
        glBufferData(GL_ARRAY_BUFFER, MAX_COUNT * sizeof(float), NULL, GL_STREAM_DRAW);
        glEnableVertexAttribArray(0);   //Assumes position is at attribute location 0 in the debug shader
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);   //Assumes position is at attribute location 0 in the debug shader
        glBindVertexArray(0);
#endif
    }

    void shutdown()
    {
#ifdef _DEBUG
        glDeleteVertexArrays(1, &vertArray);
        glDeleteBuffers(1, &vertBuf);
#endif

    }
}
