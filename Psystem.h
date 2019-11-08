#pragma once

#include <windows.h>

#define GLUT_DISABLE_ATEXIT_HACK
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>    // Only used for random seed initialisation

#include "vec3.h"
#include "vec4.h"

#define MAX_PSYSTEM 400
#define PI 3.14159265

extern GLfloat eyePosX; // needs to be set each frame if using bilboard
extern GLfloat eyePosZ;
extern GLfloat psSpinAngle; // set this to the scene total rotation if the scene is spinning (only for billboard particles)
extern GLuint  ttex[10];
 
struct particleStruct
{
	bool active;   // active Y or n
	int time;      // ticks of activity
	vec3 pos;      // position
	vec3 prevPos;  // prevPosition
	vec3 velosity; // how fast do i move
	vec4 colour;
	float pSize;   // size 
};

// 0=Points of initial size only
// 1=last turn lines
// 2=128x128 texture

struct PsystemStruct
{
	bool active;            // true if active
	int activeticks;       // num of ticks for the posys to be active -1 lasts forever
	bool particlesStillActive; // 
	int typeOfPsystem;      // 0,1,2 - see above
	GLuint texHandle;       // texture handle for type 2 systems
	bool billboard;         // true if billboard
	vec3 pos;               // location of start     
	GLfloat emissionRate;   // number of particles per tick (can be fractional)
	GLfloat emmisionRateRandom; // a max random number which we div by 100 and add to emision rate
	GLfloat residualEmmision; // how many to emit this turn - to allow for fractional emission rates
	int maxParticles;       // no more than this please
	int numParticles;       // the actual number 
	GLfloat particleSize;   // size in points 
	GLfloat particleSizeDelta;   // size in points change per tick 
	int max_ticks;          // after this many ticks die
	vec3 dieIfLess;      // kill the particl if its less than any of these values
	vec4 colour;     // particle color
	vec4 colourChange; // colour change + added each tick
	vec4 colourMax;  // 1,1,1 = white
	vec3 gravity;    // velosity change due to gravity
	vec3 wind;       // movement due to wind
	vec3 initialVelosity;
	vec3 initialVelosityRandom; // max random number which we dive by 100 and add to initial velosity 
	vec3 airResistance;    // how it slows always positive
	vec3 absTerminalVelosity; // max speed always positive

	vec4 maxColour;

	// particle data
	particleStruct p[MAX_PSYSTEM];
};

// ------------------- after definition globals ------------------------------
extern PsystemStruct psa[10]; // 0 through 9 Particle system array
// ----------------------- Methods ----------------------------------

void updatePsystem(PsystemStruct *ps);
void drawPsystem(PsystemStruct *ps);
void drawPsystem(int num);
void setPsEyePos(vec3 eyepos);

// end
