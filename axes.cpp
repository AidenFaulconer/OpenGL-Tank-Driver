#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>

#include "./axes.h"

// Functions to draw orthogonal x,y,z axes,
// Colourblind persons may wish to change this to avoid the Red-Green combo.

// GLfloat length;
// bool textSize;

void Axes::setlen(GLfloat len)
{
	length = len;
}

void Axes::setTextSize(GLfloat t)
{
	textSize = t;
}

int Axes::render()
{
	glPushMatrix();
	//preset();
	drawLabelledAxesP(length, textSize);
	glPopMatrix();
	return true;
}

Axes::Axes()
{
	//init();
	length = 1;
	textSize = 0.2f;
}

void Axes::drawAxesP(GLfloat len)
{
	glBegin(GL_LINES);
	//red x axis
	glColor3f(1, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(len, 0.0, 0.0);
	//green y axis
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, len, 0.0);
	//blue z axis
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, len);
	glEnd();
}

void Axes::drawLabelledAxesP(GLfloat len, GLfloat TxtSize)
{
	GLfloat lenP;
	lenP = len + TxtSize;
	drawAxesP(len);
	glBegin(GL_LINES);
	// Paint an "X" in red...
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(lenP, TxtSize, TxtSize);
	glVertex3f(lenP, -TxtSize, -TxtSize);

	glVertex3f(lenP, TxtSize, -TxtSize);
	glVertex3f(lenP, -TxtSize, TxtSize);

	// Paint a "Y" in green...
	glColor3f(0.0, 0.8f, 0.0);
	glVertex3f(TxtSize, lenP, TxtSize);
	glVertex3f(-TxtSize, lenP, -TxtSize);

	glVertex3f(TxtSize, lenP, -TxtSize);
	glVertex3f(0.0, lenP, 0.0);

	// Paint a "Z", in blue...
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(TxtSize, TxtSize, lenP);
	glVertex3f(-TxtSize, -TxtSize, lenP);

	glVertex3f(TxtSize, TxtSize, lenP);
	glVertex3f(-TxtSize, TxtSize, lenP);

	glVertex3f(TxtSize, -TxtSize, lenP);
	glVertex3f(-TxtSize, -TxtSize, lenP);
	glEnd();
}