#ifndef __grid_ZZ__
#define __grid_ZZ__

#include "renderable.h"
#include "Lights.h"
#include "vec3.h"

class triangle : public RenderableParent
{
	//
	// WARNING this is a slow demonstration class
	// WARNING built for teaching purposes - specifically to demonstrate triangle normals
	// WARNING to slow for serious code
	//
protected:

	vec3 vertex[3]; // see vec3.h for this type
	vec3 normal;
	vec3 p1, p2; // for drawing the normal (not needed in reality)

public:

	MaterialClass *matT;
	MaterialClass *matN;

	void setVertexV3(int vnum, vec3 v);
	void setVertex(int vnum, GLfloat x, GLfloat y, GLfloat z);
	void computeNormal();
	vec3 getNormal();

	virtual int render();
	virtual int renderNormal();

	triangle();
	~triangle();
};

class Grid : public RenderableParent
{
protected:

	GLfloat xsize;
	GLfloat zsize;
	int xsteps;
	int zsteps;

public:

	MaterialClass* mat;
	MaterialClass* matLines;

	void setUp(GLfloat xsizeZ, GLfloat zsizeZ, int xstepsZ, int zstepsZ);

	virtual int render();
	virtual int renderLines();
	Grid();
	~Grid();
};

#endif
