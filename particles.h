/*
    Pony48 header - particles.h
    Copyright (c) 2014 Mark Hutcheson
*/

#ifndef PARTICLES_H
#define PARICLES_H

#include "globaldefs.h"
#include "Image.h"

#ifndef PARTICLES_H
#define PARICLES_H

extern float32 g_fParticleFac;

typedef enum 
{
	ADDITIVE,
	NORMAL,
	SUBTRACTIVE,
} particleBlendType;

class ParticleSystem
{
protected:
	//Arrays of particle fields (structure-of-array format rather than array-of structure for speed)
	Rect* 		m_imgRect;			//Rectangle of the image to draw
	Point* 		m_pos;				//Position, in 2D space, of particle
	Point* 		m_sizeStart;		//Size at start of particle's life
	Point* 		m_sizeEnd;			//Size at end of particle's life
	Point* 		m_vel;				//Linear velocity
	Point* 		m_accel;			//Linear acceleration
	float32* 	m_rot;				//Angle to draw image at
	float32* 	m_rotVel;			//Rotational velocity (spinning speed of image)
	float32* 	m_rotAccel;			//Rotational acceleration (if spinning speed increases or decreases)
	Color* 		m_colStart;			//Color at the start of life
	Color* 		m_colEnd;			//Color at end of life
	float32*	m_tangentialAccel;	//Tangential acceleration (acceleration at an angle perpendicular to the emission point)
	float32*	m_normalAccel;		//Normal acceleration (acceleration away from the emission point)
	float32*	m_lifetime;			//How long this particle is alive for
	float32*	m_lifePreFade;		//How long the particle is alive before it starts fading colors
	float32*	m_created;			//When this particle was created/spawned
	Vec3*		m_rotAxis;			//What axis this particle rotates around when it rotates
	
	uint32_t m_num;					//How many actual particles there are active (i.e. current size of above arrays)
	uint32_t m_totalAmt;			//max times particle factor (i.e. true total max)
	void _deleteAll();				//Delete all memory associated with particles
	void _newParticle();			//Create a new particle
	void _rmParticle(uint32_t idx);	//Delete an expired particle (i.e. copy particle at end of the list to where this one was)
	void _initValues();				//Initialize particle system variables
	
	float32 curTime;
	float32 spawnCounter;
	float32 startedFiring;			//When we started firing (to keep track of decay)
	
	string m_sXMLFrom;	//So we know what XML file we should reload from
	
public:
	
	ParticleSystem();
	~ParticleSystem();
	
	//Variables used to determine starting values for each particle
	Point			sizeStart;			//Drawing size on particle spawn
	Point			sizeEnd;			//Drawing size at end of particle life
	float32			sizeVar;			//Random variation in particle size
	float32			speed;				//Speed at which particles are initially launched along emissionAngle
	float32			speedVar;
	Point			accel;				//Linear acceleration
	Point			accelVar;
	float32			rotStart;			//Starting angle
	float32			rotStartVar;
	float32 		rotVel;				//Starting rotational velocity
	float32 		rotVelVar;			
	float32 		rotAccel;			//Rotational acceleration
	float32 		rotAccelVar;
	Color 			colStart;			//Color at start of particle's life
	Color 			colEnd;				//Color at end of particle's life
	Color			colVar;				//Variation in color (applied to both start and end colors)
	float32 		tangentialAccel;	//Tangential acceleration
	float32 		tangentialAccelVar;
	float32 		normalAccel;		//Normal acceleration (away from emission point)
	float32 		normalAccelVar;
	float32 		lifetime;			//How many seconds the particles stay alive
	float32 		lifetimeVar;
	float32			lifetimePreFade;	//How long the particle stays alive before changing colors
	float32			lifetimePreFadeVar;
	float32			decay;				//How many seconds after firing to stop firing
	Vec3			rotAxis;			//What axis these particles rotate around
	Vec3			rotAxisVar;
	
	//Particle system variables
	Image*				img;				//Image to use for all of these particles
	vector<Rect>		imgRect;			//Possible positions in this image to use for each particle
	uint32_t 			max;				//Max # of particles
	float32 			rate;				//How many particles to emit per second
	Rect 				emitFrom;			//Where particles are created
	particleBlendType 	blend;				//Blend type for how particles are drawn
	float32 			emissionAngle;		//Angle particles are emitted at (degrees)
	float32 			emissionAngleVar;	//Variation in emission angle
	bool				firing;				//If we're creating new particles or not
	bool				show;				//If this should draw or not
	bool				velRotate;			//If these particles should rotate according to their velocity
	bool				changeColor;		//If these particles change color as they update, or just alpha
	vector<string>		spawnOnDeath;		//Spawn a new particle system whenever one of these dies
	Point				emissionVel;		//Move the emission point every frame
	bool				particleDeathSpawn;	//If we spawn new particle systems on particle death or system death
	
	void update(float32 dt);
	void draw();
	void init();
	void fromXML(string sXMLFilename);		//Load particle definitions from XML file
	uint32_t count() {return m_num;};		//How many particles are currently alive (read-only because reasons)
	void killParticles()	{m_num=0;};		//Kill all active particles
	void reload()			{fromXML(m_sXMLFrom);};	//Reload 
	bool done()				{return !(m_num || firing);};	//Test and see if effect is done
};

//External function you need to declare
void spawnNewParticleSystem(string sFilename, Point ptPos);

#endif








































#endif



