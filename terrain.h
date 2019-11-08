#ifndef __Terrain_grid_ZZ__
#define __Terrain_grid_ZZ__
// terrain.h

#include <windows.h>
#include "Lights.h"
#include "fileOperations.h"
#include "texturePoly.h"

class terrain
{
protected:
	tex_triangle* thisTex = new tex_triangle;

	GLfloat xsize;
	GLfloat zsize;
	int xsteps;
	int zsteps;

	GLfloat map[MAXWIDTH][MAXDEPTH];//from fileoperations
	int mapInt[MAXWIDTH][MAXDEPTH];

public:

	int mapWidth, mapHeight;

	MaterialClass* mat;
	MaterialClass* matLines;

	MaterialClass* matHigh;
	MaterialClass* matBeach;
	MaterialClass* matLand;

	GLfloat highHeight;
	GLfloat grassHeight;

	//for collision detection, gets z value from an x and y coordinite
	float getHeight(const vec3& pos);

	int readHeightField(const char *filename, int *width, int *depth, int heights[][MAXDEPTH]);
	void setUp(GLfloat xsizeZ, GLfloat zsizeZ, int xstepsZ, int zstepsZ);
	void setMaterialHeight(GLfloat height);

	virtual int render();
	virtual int renderLines();
	terrain(char* fileName, int width, int height);//no need to store the filename, just pass it in for it to be read
	~terrain();
};

#endif
