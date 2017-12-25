/*
    RetSphinxEngine source - ObjSegment.cpp
    Copyright (c) 2017 Mark Hutcheson
*/
#include "ObjSegment.h"
#include "opengl-api.h"
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

    if(body == NULL)
    {
        renderState.model = glm::rotate(renderState.model, glm::degrees(rot), glm::vec3(0.0f, 0.0f, 1.0f));
        renderState.model = glm::translate(renderState.model, glm::vec3(pos.x, pos.y, depth));
        if(obj3D)
        {
            renderState.model = glm::scale(renderState.model, glm::vec3(size.x, size.y, size.x)); //No Z axis to scale on, hmm
            renderState.apply();

            glEnable(GL_CULL_FACE);
            obj3D->render(renderState);
            glDisable(GL_CULL_FACE);
        }
        else if(img != NULL)
        {
            renderState.apply();

            //TODO: This needs to be constant, only updating when size/tex/tile changes
            Quad q;
            q.tex = *img;
            float sizex = size.x / tile.x;
            float sizey = size.y / tile.y;
            for(float y = 0; y < tile.y; y++)
            {
                for(float x = 0; x < tile.x; x++)    //TODO: Partial quad at end
                {
                    q.pos[0] = -size.x / 2.0f + sizex * x;
                    q.pos[1] = -size.y / 2.0f + sizey * y; // upper left

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
    else
    {
        b2Vec2 objpos = body->GetWorldCenter();
        float objrot = body->GetAngle();
        renderState.model = glm::translate(renderState.model, glm::vec3(objpos.x, objpos.y, 0.0f));
        renderState.model = glm::rotate(renderState.model, objrot, glm::vec3(0.0f, 0.0f, 1.0f));
        renderState.model = glm::translate(renderState.model, glm::vec3(pos.x, pos.y, depth));
        renderState.model = glm::rotate(renderState.model, glm::degrees(rot), glm::vec3(0.0f, 0.0f, 1.0f));
        if(obj3D)
        {
            renderState.model = glm::scale(renderState.model, glm::vec3(size.x, size.y, size.x)); //No Z axis to scale on, hmm
            renderState.apply();

            glEnable(GL_CULL_FACE);
            obj3D->render(renderState);
            glDisable(GL_CULL_FACE);
        }
        else if(img != NULL)
        {
            renderState.apply();

            //TODO: This needs to be constant, only updating when size/tex/tile changes
            Quad q;
            q.tex = *img;
            float sizex = size.x / tile.x;
            float sizey = size.y / tile.y;
            for(float y = 0; y < tile.y; y++)
            {
                for(float x = 0; x < tile.x; x++)    //TODO: Partial quad at end
                {
                    q.pos[0] = -size.x / 2.0f + sizex * x;
                    q.pos[1] = -size.y / 2.0f + sizey * y; // upper left

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
}
