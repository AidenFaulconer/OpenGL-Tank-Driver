#ifndef __RENDERABLE_H_
#define __RENDERABLE_H_
//renderable.h

#define GLUT_DISABLE_ATEXIT_HACK
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "vec3.h"

class RenderableParent
{
protected:
	float  xScale, yScale, zScale;
	float  xPos, yPos, zPos;
	float  xRot, yRot, zRot;

public:
	virtual void setTranslate(float xx, float yy, float zz);
	virtual void setRot(float xx, float yy, float zz);
	virtual void setScale(float xx, float yy, float zz);
	virtual int render() = 0; // returns (true)if succeed false if not
	virtual void init();
	virtual void preset();
	virtual void doTransform(); // presets rotation and position and scale
	virtual void doTranslate();
	virtual void doScale();
	virtual void doRotate();
	virtual void undoRotate();
	RenderableParent();
};

inline void displayNormal(const vec3& v1, const vec3& normalz)
{
	vec3 original = v1;
	vec3 translated = v1;
	translated = v1 + 1;

	glColor3f(1, 0, 0);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	glVertex3fv(original.fv());
	glVertex3fv(translated.fv());//translated up

	glEnd();
}
inline void renderNormal(const vec3& v1, const vec3& normalz)
{
	vec3 original = v1;
	vec3 translated = v1;
	translated = v1 + 1;
	glNormal3f(translated[0], translated[1], translated[2]);
}

//for triangles
inline void displayNormal3(const vec3& v1, const  vec3& v2, const  vec3& v3, const vec3& normalz)
{
	// displays the normal of a triangle
	vec3 avg;
	avg.x = (v1[0] + v2[0] + v3[0]) / 3;
	avg.y = (v1[1] + v2[1] + v3[1]) / 3;
	avg.z = (v1[2] + v2[2] + v3[2]) / 3;
	displayNormal(avg, normalz);
}

inline void renderNormal3(const vec3& v1, const  vec3& v2, const  vec3& v3, const vec3& normalz)
{
	// displays the normal of a triangle
	vec3 avg;
	avg.x = (v1[0] + v2[0] + v3[0]) / 3;
	avg.y = (v1[1] + v2[1] + v3[1]) / 3;
	avg.z = (v1[2] + v2[2] + v3[2]) / 3;
	renderNormal(avg, normalz);
}

//for quads
inline void displayNormal4(const vec3& v1, const  vec3& v2, const  vec3& v3, const  vec3& v4, const vec3& normalz)
{
	// displays the normal of a triangle
	vec3 avg;
	avg.x = (v1[0] + v2[0] + v3[0] + v4[0]) / 4;
	avg.y = (v1[1] + v2[1] + v3[1] + v4[1]) / 4;
	avg.z = (v1[2] + v2[2] + v3[2] + v4[2]) / 4;
	displayNormal(avg, normalz);
}
// end
#endif
