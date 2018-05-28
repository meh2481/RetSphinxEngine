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
    if(!active) //TODO: Separate out into active/non-active lists?
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
        obj3D->render(renderState);
    else if(img != NULL)    //TODO: Why do we have non-image segments?
    {
        //assert(img != NULL);
        float sizex = size.x / tile.x;
        float sizey = size.y / tile.y;
        for(float y = 0; y < tile.y; y++)
        {
            for(float x = 0; x < tile.x; x++)    //TODO: Partial quad at end
            {
                //TODO: Rotating a tiled image now breaks tiling
                Vec3 pos1(-size.x / 2.0f + sizex * x + objpos.x,   //TODO: Take rotation into account
                    -size.y / 2.0f + sizey * y + objpos.y,
                    objpos.z); // upper left

                Vec3 pos2(pos1.x + sizex,
                    pos1.y,
                    pos1.z); // upper right

                Vec3 pos3(pos1.x,
                    pos1.y + sizey,
                    pos1.z); // lower left

                Vec3 pos4(pos2.x,
                    pos3.y,
                    pos1.z); // lower right

                Draw::drawQuad(img, pos1, pos2, pos3, pos4);
            }
        }
    }
}
