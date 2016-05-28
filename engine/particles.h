/*
    GameEngine header - particles.h
    Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "globaldefs.h"
#include "Image.h"
#include "Drawable.h"
#include "luafuncs.h"
#include "luainterface.h"

#ifndef PARTICLES_H
#define PARICLES_H

extern float g_fParticleFac;

class Image;

typedef enum 
{
	ADDITIVE,
	NORMAL,
	SUBTRACTIVE,
} particleBlendType;

class ParticleSystem : public Drawable
{
protected:
	//Arrays of particle fields (structure-of-array format rather than array-of structure for speed)
	Rect* 		m_imgRect;			//Rectangle of the image to draw
	Point* 		m_pos;				//Position, in 2D space, of particle
	Point* 		m_sizeStart;		//Size at start of particle's life
	Point* 		m_sizeEnd;			//Size at end of particle's life
	Point* 		m_vel;				//Linear velocity
	Point* 		m_accel;			//Linear acceleration
	float* 	m_rot;				//Angle to draw image at
	float* 	m_rotVel;			//Rotational velocity (spinning speed of image)
	float* 	m_rotAccel;			//Rotational acceleration (if spinning speed increases or decreases)
	Color* 		m_colStart;			//Color at the start of life
	Color* 		m_colEnd;			//Color at end of life
	float*	m_tangentialAccel;	//Tangential acceleration (acceleration at an angle perpendicular to the emission point)
	float*	m_normalAccel;		//Normal acceleration (acceleration away from the emission point)
	float*	m_lifetime;			//How long this particle is alive for
	float*	m_lifePreFade;		//How long the particle is alive before it starts fading colors
	float*	m_created;			//When this particle was created/spawned
	Vec3*		m_rotAxis;			//What axis this particle rotates around when it rotates
	
	unsigned m_num;					//How many actual particles there are active (i.e. current size of above arrays)
	unsigned m_totalAmt;			//max times particle factor (i.e. true total max)
	void _deleteAll();				//Delete all memory associated with particles
	void _newParticle();			//Create a new particle
	void _rmParticle(const unsigned idx);	//Delete an expired particle (i.e. copy particle at end of the list to where this one was)
	void _initValues();				//Initialize particle system variables
	
	float curTime;
	float spawnCounter;
	float startedFiring;			//When we started firing (to keep track of decay)
	
	string m_sXMLFrom;	//So we know what XML file we should reload from
	
public:
	enum { TYPE = OT_PARTICLESYSTEM };
	LuaObjGlue *glue;
	LuaInterface* lua;
	
	ParticleSystem();
	~ParticleSystem();
	
	//Variables used to determine starting values for each particle
	Point			sizeStart;			//Drawing size on particle spawn
	Point			sizeEnd;			//Drawing size at end of particle life
	float			sizeVar;			//Random variation in particle size
	float			speed;				//Speed at which particles are initially launched along emissionAngle
	float			speedVar;
	Point			accel;				//Linear acceleration
	Point			accelVar;
	float			rotStart;			//Starting angle
	float			rotStartVar;
	float 		rotVel;				//Starting rotational velocity
	float 		rotVelVar;			
	float 		rotAccel;			//Rotational acceleration
	float 		rotAccelVar;
	Color 			colStart;			//Color at start of particle's life
	Color 			colEnd;				//Color at end of particle's life
	Color			colVar;				//Variation in color (applied to both start and end colors)
	float 		tangentialAccel;	//Tangential acceleration
	float 		tangentialAccelVar;
	float 		normalAccel;		//Normal acceleration (away from emission point)
	float 		normalAccelVar;
	float 		lifetime;			//How many seconds the particles stay alive
	float 		lifetimeVar;
	float			lifetimePreFade;	//How long the particle stays alive before changing colors
	float			lifetimePreFadeVar;
	float			decay;				//How many seconds after firing to stop firing
	Vec3			rotAxis;			//What axis these particles rotate around
	Vec3			rotAxisVar;
	
	//Particle system variables
	Image*				img;				//Image to use for all of these particles
	vector<Rect>		imgRect;			//Possible positions in this image to use for each particle
	unsigned 			max;				//Max # of particles
	float 			rate;				//How many particles to emit per second
	float				curRate;			//[0,1.0] Factor for how many particles we spawn per second
	Rect 				emitFrom;			//Where particles are created
	particleBlendType 	blend;				//Blend type for how particles are drawn
	float 			emissionAngle;		//Angle particles are emitted at (degrees)
	float 			emissionAngleVar;	//Variation in emission angle
	bool				firing;				//If we're creating new particles or not
	bool				show;				//If this should draw or not
	bool				velRotate;			//If these particles should rotate according to their velocity
	bool				changeColor;		//If these particles change color as they update, or just alpha
	vector<string>		spawnOnDeath;		//Spawn a new particle system whenever one of these dies
	Point				emissionVel;		//Move the emission point every frame
	bool				particleDeathSpawn;	//If we spawn new particle systems on particle death or system death
	
	void update(float dt);
	void draw(bool bDebugInfo);
	void init();
	void fromXML(string sXMLFilename);		//Load particle definitions from XML file
	unsigned count() {return m_num;};		//How many particles are currently alive (read-only because reasons)
	void killParticles()	{m_num=0;};		//Kill all active particles
	void reload()			{fromXML(m_sXMLFrom);};	//Reload 
	bool done()				{return !(m_num || firing);};	//Test and see if effect is done
};

//External function you need to declare
void spawnNewParticleSystem(string sFilename, Point ptPos);

#endif







































