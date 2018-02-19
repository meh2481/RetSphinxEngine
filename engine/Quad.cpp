#include "Quad.h"
#include "SDL_opengl.h"
#include "opengl-api.h"
#include "RenderState.h"
#include "glm/glm.hpp"
#include <vector>

namespace Draw
{
    std::vector<glm::mat4> mvps;
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

    void drawHelper(const float* data, unsigned int len, int numPer, int count, int type, int posAttribId)
    {
        unsigned int vertBuf;
        glGenBuffers(1, &vertBuf);
        glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
        glBufferData(GL_ARRAY_BUFFER, len, data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(posAttribId);
        glVertexAttribPointer(posAttribId, numPer, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(type, 0, count);
        glDisableVertexAttribArray(posAttribId);
        glDeleteBuffers(1, &vertBuf);
    }

    void init(int programId)
    {
        program = programId;
        mvps.reserve(1024);

        modelId = glGetUniformLocation(programId, "model");
        viewId = glGetUniformLocation(programId, "view");
        projectionId = glGetUniformLocation(programId, "projection");
    }

    void shutdown()
    {

    }
}
