#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "grid.h"

void triangle::setVertexV3(int vnum, vec3 v)
{
	vertex[vnum] = v;
}

void triangle::setVertex(int vnum, GLfloat x, GLfloat y, GLfloat z)
{
	vertex[vnum].x = x;
	vertex[vnum].y = y;
	vertex[vnum].z = z;
}

void triangle::computeNormal()
{
	polygonNormal(normal, vertex[0], vertex[1], vertex[2]);
	p1 = vertex[0];
	p1 = p1 + vertex[1];
	p1 = p1 + vertex[2];
	p1 = p1 * 1.0 / 3.0;
	p2 = p1 + normal;
}

vec3 triangle::getNormal() { return normal; }

int triangle::render()
{
	glPushMatrix();
	preset();
	matT->doMaterial();
	glBegin(GL_TRIANGLES);
	glNormal3fv(normal.fv());
	glVertex3fv(vertex[0].fv());
	glVertex3fv(vertex[1].fv());
	glVertex3fv(vertex[2].fv());
	glEnd();
	glPopMatrix();
	/*
	printf("-0-");
	v3PrintVec(vertex[0],"%.2f");
	printf("\n-1-");
	v3PrintVec(vertex[1],"%.2f");
	 printf("\n-2-");
	v3PrintVec(vertex[2],"%.2f");
	 printf("\n\n");
	 */

	return true;
}

int triangle::renderNormal()
{
	glPushMatrix();
	preset();
	matN->doMaterial();
	glBegin(GL_LINES);

	glVertex3fv(p1.fv());
	glVertex3fv(p2.fv());
	glEnd();
	glPopMatrix();
	return true;
}

triangle::triangle()
{
	matT = new MaterialClass();
	matN = new MaterialClass();
	matT->setMaterialJade();
	matN->setMaterialRedPlastic();
	//vertex[0]= new vec3(0,0,0); // see vec3.h for this type
	//vertex[1]= new vec3(1,0,0); // see vec3.h for this type
	//vertex[2]= new vec3(0,1,0); // see vec3.h for this type
	//normal= new vec3();
	//p1= new vec3();
	//p2= new vec3();;
	setVertex(0, 0, 0, 0);
	setVertex(1, 1, 0, 0);
	setVertex(2, 0, 1, 0);
	computeNormal();
}

triangle::~triangle()
{
	delete matN;
	delete matT;
}

// -----------------------------------------------------------------------------------

//GLfloat xsize;
//GLfloat zsize;
//int xsteps;
//int zsteps;

//MaterialClass* mat;
//MaterialClass* matLines;

void Grid::setUp(GLfloat xsizeZ, GLfloat zsizeZ, int xstepsZ, int zstepsZ)
{
	xsize = xsizeZ;
	zsize = zsizeZ;
	xsteps = xstepsZ;
	zsteps = zstepsZ;
}

int Grid::render()
{
	glPushMatrix();
	preset();
	mat->doMaterial();

	GLfloat xdelta = xsize / xsteps;
	GLfloat zdelta = zsize / zsteps;
	glBegin(GL_QUADS);
	for (int x = 0; x < xsteps; x++)
		for (int z = 0; z < zsteps; z++)
		{
			glNormal3fv(polygonNormal(vec3(xdelta*x, 0, zdelta*z), vec3(xdelta*x + xdelta, 0, zdelta*z), vec3(xdelta*x + xdelta, 0, zdelta*z + zdelta), vec3(xdelta*x, 0, zdelta*z + zdelta)).fv());
			glVertex3f(xdelta*x, 0, zdelta*z);

			glVertex3f(xdelta*x + xdelta, 0, zdelta*z);

			glVertex3f(xdelta*x + xdelta, 0, zdelta*z + zdelta);

			glVertex3f(xdelta*x, 0, zdelta*z + zdelta);


		}
	glEnd();
	glPopMatrix();
	return true;
}

int Grid::renderLines()
{
	glPushMatrix();
	preset();
	matLines->doMaterial();

	GLfloat xdelta = xsize / xsteps;
	GLfloat zdelta = zsize / zsteps;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);
	for (int x = 0; x < xsteps; x++)
		for (int z = 0; z < zsteps; z++)
		{
			glNormal3f(x, 0, z); // yes veronica all normals point up
			glVertex3f(xdelta*x, 0.01, zdelta*z); // Grid above and below surface
			glVertex3f(xdelta*x + xdelta, 0.01, zdelta*z);
			glVertex3f(xdelta*x + xdelta, 0.01, zdelta*z + zdelta);
			glVertex3f(xdelta*x, 0.01, zdelta*z + zdelta);
			glVertex3f(xdelta*x, -0.01, zdelta*z);
			glVertex3f(xdelta*x + xdelta, -0.01, zdelta*z);
			glVertex3f(xdelta*x + xdelta, -0.01, zdelta*z + zdelta);
			glVertex3f(xdelta*x, -0.01, zdelta*z + zdelta);
		}

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();

	//debug normals
	for (int x = 0; x < xsteps; x++)
		for (int z = 0; z < zsteps; z++)
		{
			displayNormal4(
				vec3(xdelta*x, 0, zdelta*z),
				vec3(xdelta*x + xdelta, 0, zdelta*z),
				vec3(xdelta*x + xdelta, 0, zdelta*z + zdelta),
				vec3(xdelta*x, 0, zdelta*z + zdelta),
				vec3(x, 0, z)
			);
		}
	return true;
}

Grid::Grid()
{
	mat = new MaterialClass();
	matLines = new MaterialClass();
	mat->setMaterialYellowSand();
	matLines->setMaterialBlackRubber();
	setUp(1, 1, 5, 5);
}

Grid::~Grid()
{
	delete mat;
	delete matLines;
}

// end