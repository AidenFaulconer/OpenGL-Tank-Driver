// PSystem.cpp


#include "Psystem.h"

// random integer value 0 to max (macro for speed)
#define rndToMax(max) (rand()%(int)max)

// random float value 0 to max (macro for speed)
#define rndToMaxf(max) ((float)(rand()%(int)max))

// random value between -1 and 1 (macro for speed)
//#define rnd() ((rand()%10000)/5000.0-1)

// ------------------------ variables --------------------------------------------------------------------

GLfloat billboardAngle = 0; // internal use for bilboard particles

GLfloat eyePosX = 0, eyePosZ = 0; // needs to be set each frame if using bilboard
GLfloat psSpinAngle = 0; // set this to the scene total rotation if the scene is spinning (only for billboard particles)

GLuint  ttex[10]; // list of file handles if usig textures
int psCurr; // index of current particle system
PsystemStruct psa[10]; // 0 through 9

// -----------    ---------- methods ---------------------------------------------------------------------

void setPsEyePos(vec3 eyepos) // each turn if you use billboard particles
{
	eyePosX = eyepos.x;
	eyePosZ = eyepos.z;
}

double dist(double x1, double y1, double x2, double y2)
{
	/* calc the distance between 2 points */
	double retv;
	double d1, d2;
	double temp;

	d1 = x1 - x2;
	d2 = y1 - y2;
	temp = d1 * d1 + d2 * d2;
	retv = sqrt(temp);
	return(retv);
}

void direct(GLfloat xx1, GLfloat yy1, GLfloat xx2, GLfloat yy2, GLfloat *angle, GLfloat *distz)
/* calculate angle in degrees and distance to location */
{
	*distz = dist(xx1, yy1, xx2, yy2);
	GLfloat result = atan2(yy1 - yy2, xx1 - xx2) * 180 / PI;
	*angle = result;
}


void drawPsystemType0(PsystemStruct *ps)
{
	int i;

	if (!ps->active && !ps->particlesStillActive) return;
	for (i = 0; i < ps->maxParticles; i++)
	{
		glColor4fv(ps->p[i].colour);
		glPointSize(ps->particleSize);
		glBegin(GL_POINTS);
		if (ps->p[i].active)
		{
			//glPointSize(ps->p[i].pSize);
			glVertex3fv(ps->p[i].pos.fv());
			//printf("%d %f %f %f\n", i, ps->p[i].pos[X], ps->p[i].pos[Y], ps->p[i].pos[Z]);
		}
		glEnd();
	}
}

void drawPsystemType1(PsystemStruct *ps)
{
	int i;

	if (!ps->active && !ps->particlesStillActive) return;
	glBegin(GL_LINES);
	for (i = 0; i < ps->maxParticles; i++)
	{
		glColor4fv(ps->p[i].colour);
		//glPointSize(ps->particleSize);

		if (ps->p[i].active)
		{
			//glPointSize(ps->p[i].pSize);
			glVertex3fv(ps->p[i].prevPos.fv());
			glVertex3fv(ps->p[i].pos.fv());

			//printf("%d %f %f %f\n", i, ps->p[i].pos[X], ps->p[i].pos[Y], ps->p[i].pos[Z]);
		}
	}
	glEnd();
}

void drawPsystemType2(PsystemStruct *ps)
{
	int i;
	GLfloat temp;
	if (!ps->active && !ps->particlesStillActive) return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, ps->texHandle);//was ttex[] which wont work here

	direct(0, 0, eyePosX, eyePosZ, &billboardAngle, &temp); // I look at 0,0

	for (i = 0; i < ps->maxParticles; i++)
	{
		if (ps->p[i].active)
		{
			//glColor4fv(ps->p[i].colour);
			glColor4f(1, 1, 1, 1);
			glPushMatrix();
			glTranslatef(ps->p[i].pos[X], ps->p[i].pos[Y], ps->p[i].pos[Z]);
			glScalef(ps->p[i].pSize, ps->p[i].pSize, ps->p[i].pSize);
			if (ps->billboard)
			{
				glRotatef(billboardAngle - psSpinAngle, 0.0, 1.0, 0.0);
			}
			glBegin(GL_QUADS);
			glTexCoord2f(0, 1);
			glVertex3f(-0.5, 0.5, 0.0);
			glTexCoord2f(1, 1);
			glVertex3f(0.0, 0.5, 0.0);
			glTexCoord2f(1, 0);
			glVertex3f(0.0, -0.5, 0.0);
			glTexCoord2f(0, 0);
			glVertex3f(-0.5, -0.5, 0.0);
			glEnd();
			glPopMatrix();
		}
	}
	glDisable(GL_TEXTURE_2D);
}

void drawPsystem(int num)
{
	psCurr = num;
	drawPsystem(&(psa[num]));
}

void drawPsystem(PsystemStruct *ps)
{
	if (ps->typeOfPsystem == 0) drawPsystemType0(ps); // points of a fixed size
	if (ps->typeOfPsystem == 1) drawPsystemType1(ps); // lines
	if (ps->typeOfPsystem == 2) drawPsystemType2(ps); // 64 x 64 bmp with alpha
}


void updatePsystem(PsystemStruct *ps)
{
	int i;
	GLfloat f;

	if (!ps->active && !ps->particlesStillActive) return;
	if (ps->activeticks > 0)
	{
		ps->activeticks--;
		if (ps->activeticks <= 0) { ps->active = false; ps->residualEmmision = 0; }
	}

	// process each particle slot for this time step...
	if (ps->active)
	{
		ps->residualEmmision = ps->residualEmmision + ps->emissionRate;
		if (ps->emmisionRateRandom != 0) ps->residualEmmision = ps->residualEmmision + rndToMaxf(ps->emmisionRateRandom) / 100;
	}

	ps->particlesStillActive = false;

	for (i = 0; i < ps->maxParticles; i++)
	{
		if ((!ps->p[i].active) && ps->residualEmmision > 1)
		{
			// create a new particle
			ps->residualEmmision = ps->residualEmmision - 1;
			ps->p[i].active = true;
			ps->numParticles++;
			ps->p[i].pSize = ps->particleSize;
			ps->p[i].time = 0;
			v3CopyM(ps->p[i].pos, ps->pos);
			v3CopyM(ps->p[i].prevPos, ps->pos);
			v3CopyM(ps->p[i].velosity, ps->initialVelosity);
			v4CopyM(ps->p[i].colour, ps->colour);
			//for(n=0;n<3;n++)
			  // {
			if ((int)ps->initialVelosityRandom[X] != 0)
			{
				f = rndToMaxf(ps->initialVelosityRandom[X]) / 100;
				ps->p[i].velosity.x = ps->p[i].velosity.x + f;
			}
			if ((int)ps->initialVelosityRandom[Y] != 0)
			{
				f = rndToMaxf(ps->initialVelosityRandom[Y]) / 100;
				ps->p[i].velosity.y = ps->p[i].velosity.y + f;
			}
			if ((int)ps->initialVelosityRandom[Z] != 0)
			{
				f = rndToMaxf(ps->initialVelosityRandom[Z]) / 100;
				ps->p[i].velosity.z = ps->p[i].velosity.z + f;
			}
		}

		//... then move all the particles...
		if (ps->p[i].active)
		{
			ps->particlesStillActive = true;
			ps->p[i].time++;
			v3CopyM(ps->p[i].prevPos, ps->p[i].pos);
			if (ps->p[i].time > ps->max_ticks) { ps->p[i].active = false; ps->numParticles--; continue; }
			v3AddToVecM(ps->p[i].pos, ps->p[i].velosity); // my velosity
			v3AddToVecM(ps->p[i].pos, ps->wind); // wind
			if (v3isLess(ps->p[i].pos, ps->dieIfLess)) { ps->p[i].active = false; ps->numParticles--; continue; }

			v3AddToVecM(ps->p[i].velosity, ps->gravity); // gravity
			v3clipAbsM(ps->p[i].velosity, ps->absTerminalVelosity);
			v3dampM(ps->p[i].velosity, ps->airResistance); // air resistance
			v3clipAbsM(ps->p[i].velosity, ps->absTerminalVelosity); // terminal velosity

			v4AddToVecM(ps->p[i].colour, ps->colourChange);
			v4ClipMaxM(ps->p[i].colour, ps->colourMax);

			ps->p[i].pSize = ps->p[i].pSize + ps->particleSizeDelta;
			if (ps->p[i].pSize < 0)ps->p[i].pSize = 0;
		}
	}
}


// end