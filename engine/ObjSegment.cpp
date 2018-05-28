/*
    RetSphinxEngine source - ObjSegment.cpp
    Copyright (c) 2017 Mark Hutcheson
*/
#include "ObjSegment.h"
#include "Quad.h"
#include "Object3D.h"
#include <Box2D/Box2D.h>

ObjSegment::ObjSegment()
{
    body = NULL;
    parent = NULL;
    obj3D = NULL;
    depth = 0;
    img = NULL;
    active = true;

    rot = 0.0f;
    size.x = size.y = tile.x = tile.y = 1.0f;
}

ObjSegment::~ObjSegment()
{
    //Free Box2D body
    if(body != NULL)
        body->GetWorld()->DestroyBody(body);
}

void ObjSegment::draw(RenderState renderState)
{
    if(!active)
        return;

    Vec3 objpos(pos.x, pos.y, depth);
    float objrot = rot;
    if(body != NULL)
    {
        b2Vec2 bodyPos = body->GetWorldCenter();
        objpos = Vec3(bodyPos.x, bodyPos.y, depth);
        objrot = body->GetAngle();
    }

    if(obj3D)
    {
        renderState.model = glm::scale(renderState.model, glm::vec3(size.x, size.y, size.x)); //No Z axis to scale on, hmm

        //glEnable(GL_CULL_FACE);
        obj3D->render(renderState);
        //glDisable(GL_CULL_FACE);
    }
    else if(img != NULL)
    {
        //glUseProgram(renderState.programId);
        //glUniformMatrix4fv(renderState.modelId, 1, false, &renderState.model[0][0]);
        //glUniformMatrix4fv(renderState.viewId, 1, false, &renderState.view[0][0]);
        //glUniformMatrix4fv(renderState.projectionId, 1, false, &renderState.proj[0][0]);

        //TODO: This needs to be constant, only updating when size/tex/tile changes
        Quad q;
        q.tex = *img;
        float sizex = size.x / tile.x;
        float sizey = size.y / tile.y;
        for(float y = 0; y < tile.y; y++)
        {
            for(float x = 0; x < tile.x; x++)    //TODO: Partial quad at end
            {
                q.pos[0] = -size.x / 2.0f + sizex * x + objpos.x;   //TODO: Take rotation into account
                q.pos[1] = -size.y / 2.0f + sizey * y + objpos.y; // upper left

                q.pos[2] = q.pos[0] + sizex;
                q.pos[3] = q.pos[1]; // upper right

                q.pos[4] = q.pos[0];
                q.pos[5] = q.pos[1] + sizey; // lower left

                q.pos[6] = q.pos[2];
                q.pos[7] = q.pos[5]; // lower right

                Draw::drawQuad(&q);
            }
        }
    }
}
