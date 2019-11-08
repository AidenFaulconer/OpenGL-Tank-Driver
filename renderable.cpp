// renderable.cpp

#include "stdlib.h"
#include "renderable.h"

void RenderableParent::preset() {
	doTranslate();
	doRotate();
	doScale();
}

void RenderableParent::setTranslate(float xx, float yy, float zz)
{
	xPos = xx;
	yPos = yy;
	zPos = zz;
}

void RenderableParent::setRot(float xx, float yy, float zz)
{
	xRot = xx;
	yRot = yy;
	zRot = zz;
}

void RenderableParent::setScale(float xx, float yy, float zz)
{
	xScale = xx;
	yScale = yy;
	zScale = zz;
}

void RenderableParent::doTransform() // presets rotation and position and scale
{
	doTranslate();
	doRotate();
	doScale();
}

void RenderableParent::init()
{
	xScale = 1;
	yScale = 1;
	zScale = 1;
	xPos = 0;
	yPos = 0;
	zPos = 0;
	xRot = 0;
	yRot = 0;
	zRot = 0;
}

void RenderableParent::doTranslate()
{
	glTranslatef(xPos, yPos, zPos);
}

void RenderableParent::doScale()
{
	glScalef(xScale, yScale, zScale);
}

void RenderableParent::doRotate()
{
	glRotatef(xRot, 1, 0, 0);
	glRotatef(yRot, 0, 1, 0);
	glRotatef(zRot, 0, 0, 1);
}

void RenderableParent::undoRotate()
{
	glRotatef(-zRot, 0, 0, 1);
	glRotatef(-yRot, 0, 1, 0);
	glRotatef(-xRot, 1, 0, 0);
}

RenderableParent::RenderableParent()
{
	init();
}

// end