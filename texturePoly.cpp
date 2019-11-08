// texturepoly.cpp

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "glut.h"
#include "fileOperations.h"
#include "texturePoly.h"

// -----------------------------------------------------------------------------------

void tex_poly::setVertexV3(int vnum, vec3 v)
{
	vertex[vnum].x = v.x;
	vertex[vnum].y = v.y;
	vertex[vnum].z = v.z;
}

void tex_poly::setVertex(int vnum, GLfloat x, GLfloat y, GLfloat z)
{
	vertex[vnum].x = x;
	vertex[vnum].y = y;
	vertex[vnum].z = z;
}

void tex_poly::computeNormal()
{
	//v3PolygonNormal(normal, vertex[0], vertex[1], vertex[2]);
	polygonNormal(normal, vertex[0], vertex[1], vertex[2]);
	//p1 = vertex[0];
	//v3Plus(p1,p1,vertex[1]);
	//v3Plus(p1,p1,vertex[2]);
	//v3ScalarMult(p1,1.0/3.0,p1);
	//v3Plus(p2,p1, normal);
}

//vec3 tex_poly::getNormal() {return normal;}

int tex_poly::renderNormal()
{
	glPushMatrix();

	mat->doMaterial();
	glBegin(GL_LINES);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glEnd();
	glPopMatrix();
	return true;
}

tex_poly::tex_poly()
{
	mat = new MaterialClass();
	mat->setMaterialWhiteStuff();
}

tex_poly::~tex_poly()
{
	delete mat;
}

bool tex_poly::loadTexture(char *fname, GLboolean buildMipmaps)
{
	texHandle = getBMPTexture(fname, buildMipmaps);
	tex = texHandle;
	return (texHandle != -1);
}

bool tex_poly::texOk() // return true if the texture loaded ok
{
	return (texHandle != -1);
}

void tex_poly::setTextureHandle(GLuint t)
{
	tex = t;
}

//------------------------------- tex_triangle ------------------------
int tex_triangle::render()
{
	return render1(false);
}

int tex_triangle::render1(bool lightingOn)
{
	glPushMatrix();

	if (lightingOn)
	{
		mat->doMaterial();
		glEnable(GL_LIGHTING);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	else { glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); }
	glBegin(GL_TRIANGLES);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(vertex[0].x, vertex[0].y, vertex[0].z);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(vertex[1].x, vertex[1].y, vertex[1].z);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(vertex[2].x, vertex[2].y, vertex[2].z);
	glEnd();
	glPopMatrix();

	return true;
}
tex_triangle::tex_triangle()
{
	setVertex(0, 0, 0, 0);
	setVertex(1, 0, 1, 0);
	setVertex(2, 0, 1, 1);
	//setVertex(4, 0,0,1);
	computeNormal();
}

tex_triangle::~tex_triangle()
{
}

//------------------------------- tex_quad -------------------------------

int tex_quad::render()
{
	return render1(false);
}

int tex_quad::render1(bool lightingOn)
{
	glPushMatrix();

	if (lightingOn)
	{
		mat->doMaterial();
		glEnable(GL_LIGHTING);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	else { glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); }
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBegin(GL_QUADS);
	glNormal3f(normal.x, normal.y, normal.z);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(vertex[0].x, vertex[0].y, vertex[0].z);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(vertex[1].x, vertex[1].y, vertex[1].z);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(vertex[2].x, vertex[2].y, vertex[2].z);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(vertex[3].x, vertex[3].y, vertex[3].z);

	//   glNormal3f(normal.x,normal.y,normal.z);
	   //glTexCoord2f(0.0, 0.0);
	   //glVertex3f(vertex[0].x,vertex[0].y,vertex[0].z);
	   //glTexCoord2f(0.0, 3.0);
	   //glVertex3f(vertex[1].x,vertex[1].y,vertex[1].z);
	   //glTexCoord2f(3.0, 3.0);
	   //glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
	   //glTexCoord2f(3.0, 0.0);
	   //glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);

	   //glNormal3f(normal.x,normal.y,normal.z);
	   //glTexCoord2f(0.0, 0.0);
	   //glVertex3f(vertex[0].x,vertex[0].y,vertex[0].z);
	   //glTexCoord2f(0.0, 0.5);
	   //glVertex3f(vertex[1].x,vertex[1].y,vertex[1].z);
	   //glTexCoord2f(0.5, 0.5);
	   //glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
	   //glTexCoord2f(0.5, 0.0);
	   //glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);

	glEnd();
	glPopMatrix();

	return true;
}

tex_quad::tex_quad()
{
	setVertex(0, 0, 0, 0);
	setVertex(1, 0, 3, 0);
	setVertex(2, 0, 3, 3);
	setVertex(3, 0, 0, 3);
	computeNormal();
}

tex_quad::~tex_quad()
{
}

// ----------------------------------------------------------------------

// end