#ifndef __Texpoly__
#define __Texpoly__

#include "vec3.h"
#include "Lights.h"

class tex_poly
{
protected:

	MaterialClass* mat;
	vec3 vertex[4]; // see vec3.h for this type
	vec3 normal;
	vec3 p1, p2; // for drawing the normal (not needed in reality)
	GLuint tex; // the open gl texture handle
	GLuint texHandle; // temp for file reading

public:
	void setVertexV3(int vnum, vec3 v);
	void setVertex(int vnum, GLfloat x, GLfloat y, GLfloat z);
	void computeNormal();
	float *getNormal();

	bool loadTexture(char *fname, GLboolean buildMipmaps);
	void setTextureHandle(GLuint t);
	bool texOk(); // return true if the texture loaded ok

	virtual int renderNormal(); // for debugging only

	tex_poly();
	~tex_poly();
};

class tex_triangle : public tex_poly
{
protected:

public:

	virtual int render();
	int render1(bool lightingOn);
	tex_triangle();
	~tex_triangle();
};

class tex_quad : public tex_poly
{
protected:

public:

	virtual int render();
	int tex_quad::render1(bool lightingOn);
	tex_quad();
	~tex_quad();
};

#endif
