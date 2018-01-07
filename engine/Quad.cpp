#include "Quad.h"
#include "SDL_opengl.h"

namespace Draw
{
    void drawQuad(Quad* q)
    {
        // tell opengl to use the generated texture
        glBindTexture(GL_TEXTURE_2D, q->tex.tex.tex);
        glVertexPointer(2, GL_FLOAT, 0, q->pos);
        glTexCoordPointer(2, GL_FLOAT, 0, q->tex.uv);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}