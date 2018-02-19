#include "Quad.h"
#include "SDL_opengl.h"
#include "opengl-api.h"

namespace Draw
{

    static int program;

    void drawQuad(Quad* q)
    {
        //// tell opengl to use the generated texture
        //glBindTexture(GL_TEXTURE_2D, q->tex.tex.tex);
        //glVertexPointer(2, GL_FLOAT, 0, q->pos);
        //glTexCoordPointer(2, GL_FLOAT, 0, q->tex.uv);
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void drawHelper(const float* data, unsigned int len, int numPer, int count, int type, int posAttribId)
    {
        //unsigned int vertBuf;
        //glGenBuffers(1, &vertBuf);
        //glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
        //glBufferData(GL_ARRAY_BUFFER, len, data, GL_STATIC_DRAW);
        //glEnableVertexAttribArray(posAttribId);
        //glVertexAttribPointer(posAttribId, numPer, GL_FLOAT, GL_FALSE, 0, (void*)0);

        //glDrawArrays(type, 0, count);
        //glDisableVertexAttribArray(posAttribId);
        //glDeleteBuffers(1, &vertBuf);
    }

    void init(int programId)
    {
        program = programId;
        //glGenBuffers(1, &vertBuf);
    }

    void shutdown()
    {

    }
}
