/*
    GameEngine header - ParticleSystem.h
    Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "SDL_opengl.h"
#include "LuaFuncs.h"
#include "LuaInterface.h"
#include "Subject.h"
#include "Rect.h"
#include "Color.h"
#include "RenderState.h"
#include <vector>

class Image;

typedef enum 
{
    ADDITIVE = 0,
    NORMAL,
    SUBTRACTIVE,
} particleBlendType;

class ParticleSystem
{
    friend class ResourceLoader;

    //Drawing helper arrays
    float* m_vertexPtr;
    float* m_colorPtr;
    float* m_texCoordPtr;

    //Should not directly set or modify these
    //Arrays of particle fields (structure-of-array format rather than array-of structure for speed)
    Rect*     m_imgRect;            //Rectangle of the image to draw
    Vec2*     m_pos;                //Position, in 2D space, of particle
    Vec2*     m_sizeStart;        //Size at start of particle's life
    Vec2*     m_sizeEnd;            //Size at end of particle's life
    Vec2*     m_vel;                //Linear velocity
    Vec2*     m_accel;            //Linear acceleration
    float*     m_rot;                //Angle to draw image at
    float*     m_rotVel;            //Rotational velocity (spinning speed of image)
    float*     m_rotAccel;            //Rotational acceleration (if spinning speed increases or decreases)
    Color*     m_colStart;            //Color at the start of life
    Color*     m_colEnd;            //Color at end of life
    float*    m_tangentialAccel;    //Tangential acceleration (acceleration at an angle perpendicular to the emission point)
    float*    m_normalAccel;        //Normal acceleration (acceleration away from the emission point)
    float*    m_lifetime;            //How long this particle is alive for
    float*    m_lifePreFade;        //How long the particle is alive before it starts fading colors
    float*    m_created;            //When this particle was created/spawned
    Vec3*    m_rotAxis;            //What axis this particle rotates around when it rotates

    unsigned m_num;                //How many actual particles there are active (i.e. current size of above arrays)
    unsigned m_totalAmt;        //max times particle factor (i.e. true total max)
    void _deleteAll();            //Delete all memory associated with particles
    void _newParticle();        //Create a new particle
    void _rmParticle(const unsigned idx);    //Delete an expired particle (i.e. copy particle at end of the list to where this one was)
    void _initValues();            //Initialize particle system variables

    float curTime;
    float spawnCounter;
    float startedFiring;            //When we started firing (to keep track of decay)

    std::string m_sXMLFrom;    //So we know what XML file we should reload from

    Subject* m_subject;

public:

    enum { TYPE = OT_PARTICLESYSTEM };
    LuaObjGlue *glue;
    LuaInterface* lua;
    
    ParticleSystem();
    ~ParticleSystem();
    
    //Variables used to determine starting values for each particle
    Vec2    sizeStart;            //Drawing size on particle spawn
    Vec2    sizeEnd;            //Drawing size at end of particle life
    float    sizeVar;            //Random variation in particle size
    float    speed;                //Speed at which particles are initially launched along emissionAngle
    float    speedVar;
    Vec2    accel;                //Linear acceleration
    Vec2    accelVar;
    float    rotStart;            //Starting angle
    float    rotStartVar;
    float     rotVel;                //Starting rotational velocity
    float     rotVelVar;            
    float     rotAccel;            //Rotational acceleration
    float     rotAccelVar;
    Color     colStart;            //Color at start of particle's life
    Color     colEnd;                //Color at end of particle's life
    Color    colVar;                //Variation in color (applied to both start and end colors)
    float     tangentialAccel;    //Tangential acceleration
    float     tangentialAccelVar;
    float     normalAccel;        //Normal acceleration (away from emission point)
    float     normalAccelVar;
    float     lifetime;            //How many seconds the particles stay alive
    float     lifetimeVar;
    float    lifetimePreFade;    //How long the particle stays alive before changing colors
    float    lifetimePreFadeVar;
    float    decay;                //How many seconds after firing to stop firing
    Vec3    rotAxis;            //What axis these particles rotate around
    Vec3    rotAxisVar;
    
    //Particle system variables
    Image*                img;                //Image to use for all of these particles    //TODO: Use quad instead
    std::vector<Rect>    imgRect;            //Possible positions in this image to use for each particle
    unsigned             max;                //Max # of particles
    float                 rate;                //How many particles to emit per second
    float                curRate;            //[0,1.0] Factor for how many particles we spawn per second
    Rect                 emitFrom;            //Where particles are created
    particleBlendType     blend;                //Blend type for how particles are drawn
    float                 emissionAngle;        //Angle particles are emitted at (degrees)
    float                 emissionAngleVar;    //Variation in emission angle
    bool                firing;                //If we're creating new particles or not
    bool                velRotate;            //If each particle should face the direction it's moving instead of rotating normally
    std::vector<std::string>        spawnOnDeath;        //Spawn a new particle system whenever one of these dies
    Vec2                emissionVel;        //Move the emission point every frame
    bool                velAdd;                //True to add emitter vel to new particles
    
    void update(float dt);
    void draw(const RenderState& renderState);
    void init();
    unsigned count()        {return m_num;};                //How many particles are currently alive (read-only because reasons)
    void killParticles()    {m_num=0;};                        //Kill all active particles
    bool done()                {return !(m_num || firing);};    //Test and see if effect is done

    void setSubject(Subject* subject) { m_subject = subject; };
};








































