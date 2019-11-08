// cubetexture.cpp : Defines the entry point for the console application.
//
#include <windows.h>

#define GLUT_DISABLE_ATEXIT_HACK
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "bitmap.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#define Near 0.5
#define Far	900.0

#define cmdNoCull	1
#define cmdCWFront	2
#define cmdCCWFront	3

#define cmdExit		99

static int	AppMenu;

static GLfloat	Spin;
int err;
const GLubyte *oglVersion;
int decal = 1;
int filter= 1;
int spinning = true;
bool atEye=false;

GLfloat ambient[4] = {0.2,0.2,0.2,1};
GLfloat diffuse[4] = {0.8,0.8,0.8,1};
GLfloat specular[4] = {1,1,1,1};
GLfloat lightpos[4] = {5,0,5,1};

  GLfloat ambientMat[4] = {.6,0,0,1};
  GLfloat whiteMat[4]={.7,.7,.7,1};
  GLfloat specularMat[4] = {1,1,1,1};

  //GLfloat eye[4]= {3,0,3,1};
  GLfloat eye[4]= {30,2,30,1};
  GLfloat temp0[4]; // for use later
  GLfloat temp1[4]; // for use later
  GLfloat temp2[4]; // for use later

static GLfloat Verts[8][3] = {
  { -0.5,  0.5, -0.5 }, /* 0 left top rear */
  {  0.5,  0.5, -0.5 },	/* 1 right top rear */
  {  0.5, -0.5, -0.5 },	/* 2 right bottom rear */
  { -0.5, -0.5, -0.5 },	/* 3 left bottom rear */
  { -0.5,  0.5,  0.5 },	/* 4 left top front */
  {  0.5,  0.5,  0.5 },	/* 5 right top front */
  {  0.5, -0.5,  0.5 },	/* 6 right bottom front */
  { -0.5, -0.5,  0.5 }	/* 7 left bottom front */
};

static GLfloat TextureCoOrds[8][2] = {
  { -0.5,  0.5, }, /* 0 left top rear */
  {  0.5,  0.5, },	/* 1 right top rear */
  {  0.5, -0.5, },	/* 2 right bottom rear */
  { -0.5, -0.5, },	/* 3 left bottom rear */
  { -0.5,  0.5, },	/* 4 left top front */
  {  0.5,  0.5, },	/* 5 right top front */
  {  0.5, -0.5, },	/* 6 right bottom front */
  { -0.5, -0.5, }	/* 7 left bottom front */
};

static int Faces[6][4] = {
  { 4, 5, 6, 7 },	/* front */
  { 5, 1, 2, 6 },	/* right */
  { 0, 4, 7, 3 },	/* left */
  { 4, 0, 1, 5 },	/* top */
  { 7, 6, 2, 3 },	/* bottom */
  { 1, 0, 3, 2 }	/* rear */
};

GLfloat texColor[] = {0,0,0,0};

 /*
 GLUT_BITMAP_9_BY_15	
 GLUT_BITMAP_8_BY_13		
 GLUT_BITMAP_TIMES_ROMAN_10	
 GLUT_BITMAP_TIMES_ROMAN_24	

 GLUT_BITMAP_HELVETICA_10	
 GLUT_BITMAP_HELVETICA_12	
 GLUT_BITMAP_HELVETICA_18
*/

// The following example code to calculate a normal from 3 points
// and to make a normal into a unit normal
// is From the OpenGL Superbible


void ReduceToUnit(float vector[3])
// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.	
{
float length;
	
// Calculate the length of the vector		
length = (float)sqrt((vector[0]*vector[0]) + 
						(vector[1]*vector[1]) +
						(vector[2]*vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if(length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
	}



void calcNormal(float v[3][3], float out[3])
// calculates a normal
// Points p1, p2, & p3 specified in counter clock-wise order
	{
	float v1[3],v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y]*v2[z] - v1[z]*v2[y];
	out[y] = v1[z]*v2[x] - v1[x]*v2[z];
	out[z] = v1[x]*v2[y] - v1[y]*v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
	}

void calcNormalB(float p1[3], float p2[3], float p3[3], float out[3])
{
float v[3][3];
v[0][0]=p1[0];
v[0][1]=p1[1];
v[0][2]=p1[2];
v[1][0]=p2[0];
v[1][1]=p2[1];
v[1][2]=p2[2];
v[2][0]=p3[0];
v[2][1]=p3[1];
v[2][2]=p3[2];
calcNormal(v,out);
}

GLuint ttex[4];

GLuint skyTex;
UCO_BMP skyTexStruct;
#define SKYBOX_BITMAP_FILE "sky2.bmp"

GLvoid *font_style = GLUT_BITMAP_9_BY_15;
int ww,hh;

UCO_BMP butt;
#define GROUND_BITMAP_FILE "metal.bmp"

UCO_BMP texture;
#define TEXTURE_BITMAP_FILE "texture1.bmp"

void drawstr(GLuint x, GLuint y, char* format, ...)
// routine from the textbook
{
    va_list args;
    char buffer[255], *s;
    
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    
    glRasterPos2i(x, y);
    for (s = buffer; *s; s++)
        glutBitmapCharacter(font_style, *s);
}

void DrawSkyboxAt(float x,float y,float z, float sz)
//
// skybox routine  
//
{
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);									//disables depth test
	glDepthMask(0);												//so skybox will never cover any geometry
	glBindTexture(GL_TEXTURE_2D,ttex[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,1.0f);									// front face
	glVertex3f(-sz+x,-sz+y,-sz+z);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(sz+x,-sz+y,-sz+z);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(sz+x,sz+y,-sz+z);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-sz+x,sz+y,-sz+z);
	glTexCoord2f(0.0f,1.0f);									// left face
	glVertex3f(-sz+x,-sz+y,sz+z);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(-sz+x,-sz+y,-sz+z);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-sz+x,sz+y,-sz+z);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-sz+x,sz+y,sz+z);
	glTexCoord2f(0.0f,1.0f);									// right face
	glVertex3f(sz+x,-sz+y,-sz+z);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(sz+x,-sz+y,sz+z);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(sz+x,sz+y,sz+z);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(sz+x,sz+y,-sz+z);
	glTexCoord2f(0.0f,1.0f);									// back face
	glVertex3f(sz+x,-sz+y,sz+z);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(-sz+x,-sz+y,sz+z);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-sz+x,sz+y,sz+z);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(sz+x,sz+y,sz+z); 
	glTexCoord2f(0.0f,1.0f);									// upper face
	glVertex3f(-sz+x,sz+y,-sz+z);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(sz+x,sz+y,-sz+z);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(sz+x,sz+y,sz+z);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-sz+x,sz+y,sz+z);
	glTexCoord2f(0.0f,1.0f);									// lower face
	glVertex3f(-sz+x,-sz+y,sz+z);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(sz+x,-sz+y,sz+z);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(sz+x,-sz+y,-sz+z);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-sz+x,-sz+y,-sz+z);
	glEnd();
	glDepthMask(1);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	
}



static void drawCube()
{
  int face, vert;
  float norm[3];
  glDisable(GL_TEXTURE_2D);
  glPushMatrix();
  //glTranslatef(0,-1.5,0);

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,ambientMat);
  //glMaterialfv(GL_FRONT, GL_SPECULAR, ambient);
  
  glBegin(GL_QUADS);
  for (face = 0; face < 6; face++)
  {
  calcNormalB(Verts[Faces[face][2]],
	          Verts[Faces[face][1]],
			  Verts[Faces[face][0]]
              ,norm);
     
     for (vert = 0; vert < 4; vert ++)
	 {
	 glNormal3f(norm[0],norm[1],norm[2]);
     glVertex3fv(Verts[Faces[face][vert]]);
	 }
  }
  glEnd();
  glPopMatrix();
}


 void drawGround()
 {
 glEnable(GL_TEXTURE_2D);
 glBindTexture(GL_TEXTURE_2D,  ttex[0]);
 glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
 glBegin(GL_QUADS);
 glTexCoord2f(0,4);
 glVertex3f(-8,  0,  -8);
 glTexCoord2f(4,4);
 glVertex3f( 8,  0,  -8);
 glTexCoord2f(4,0);
 glVertex3f( 8, 0,  8);
 glTexCoord2f(0,0);
 glVertex3f(-8, 0,  8);
 glEnd();

 }

static void display ()
{
  GLfloat aspect;
  GLfloat norm[4];
 
  glDepthRange(Near,Far);
  glDepthFunc(GL_LESS);
 
  glClearDepth(Far);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, ww, hh);

 // draw skybox
 glDisable(GL_DEPTH_TEST);
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 aspect = (float)ww / (float)hh;
 gluPerspective(60.0, aspect, Near, Far);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 //gluLookAt(eye[0],eye[1],eye[2],
//	    0.0, 0.0, 0.0,
//	    0.0, 1.0, 0.0);
 GLfloat vx,vy,vz,temp;
 temp=1; //eye[0]+eye[1]+eye[2];
 vx=eye[0]/temp;
 vy=eye[1]/temp;
 vz=eye[2]/temp;

 gluLookAt(vx,vy,vz,
	       0,0,0,
	       0.0, 1.0, 0.0);

 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
 glEnable(GL_TEXTURE_2D); 
 glBindTexture(GL_TEXTURE_2D,  ttex[2]);
 glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
 if (atEye) DrawSkyboxAt(eye[0],eye[1],eye[2],200); 
 else DrawSkyboxAt(0,0,0,200); 
 

 /* glBegin(GL_QUADS);
 glTexCoord2f(0,1);
 glVertex3f(-8,  16,  +8);
 glTexCoord2f(1,1);
 glVertex3f( +8,  16,  +8);
 glTexCoord2f(1,0);
 glVertex3f( +8, 0,  +8);
 glTexCoord2f(0,0);
 glVertex3f(-8, 0,  +8);
 glEnd();

 glBegin(GL_QUADS);
 glTexCoord2f(0,1);
 glVertex3f(+8,  16,  -8);
 glTexCoord2f(1,1);
 glVertex3f( +8,  16, +8);
 glTexCoord2f(1,0);
 glVertex3f( +8, 0, +8);
 glTexCoord2f(0,0);
 glVertex3f(+8, 0,  -8);
 glEnd(); */

 glDisable(GL_TEXTURE_2D); 
 glEnable(GL_DEPTH_TEST);
 
// Do the 3D drawing bit
  glClear(GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  aspect = (float)ww / (float)hh;
  gluPerspective(60.0, aspect, Near, Far);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  glLightfv(GL_LIGHT0,GL_POSITION, lightpos);

  gluLookAt(eye[0],eye[1],eye[2],
	    0.0, 0.0, 0.0,
	    0.0, 1.0, 0.0);

  glPushMatrix();
  drawGround();
  glTranslatef(0.0, 1.0, 0.0);
  glRotatef(Spin, 0.0, 1.0, 0.0);

  drawCube();
  glPopMatrix();

  glPushMatrix();
  glRotatef(Spin, 0.0, 1.0, 0.0);
  glTranslatef(0.0, 1.0, 0.0);
  glEnable(GL_TEXTURE_2D);
  if (decal==1) glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  if (decal==2) glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
  if (decal==3) glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


  if (filter == 1)
  {
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }
  if (filter == 2)
  {
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  } 


  glBindTexture(GL_TEXTURE_2D, ttex[1]);

temp0[0]=-0.85;  
temp0[1]=0.85;
temp0[2]=0.85;
temp1[0]=0.85;
temp1[1]=0.85;
temp1[2]=0.85;
temp2[0]=0.85; 
temp2[1]=-0.85;
temp2[2]=0.85;

  calcNormalB(temp2,temp1,temp0,norm);
  glNormal3f(norm[0],norm[1],norm[2]);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,whiteMat);

  glBegin(GL_QUADS);
  glTexCoord2f(0,1);
  glVertex3f(-0.85,  0.85,  0.85);
  glTexCoord2f(1,1);
  glVertex3f( 0.85,  0.85,  0.85);
  glTexCoord2f(1,0);
  glVertex3f( 0.85, -0.85,  0.85);
  glTexCoord2f(0,0);
  glVertex3f(-0.85, -0.85,  0.85);
  glEnd();

if (filter == 1)
  {
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }
  if (filter == 2)
  {
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
/*
glBindTexture(GL_TEXTURE_2D, ttex[0]);

glBegin(GL_QUADS);
  glTexCoord2f(0,1);
  glVertex3f(-0.85,  0.45,  -0.85);
  glTexCoord2f(2,1);
  glVertex3f( 0.85,  0.45,  -0.85);
  glTexCoord2f(2,0);
  glVertex3f( 0.85, -0.45,  -0.85);
  glTexCoord2f(0,0);
  glVertex3f(-0.85, -0.45,  -0.85);
  glEnd();
*/
  glDisable(GL_TEXTURE_2D);

  

  glPopMatrix();

// Do the 2D drawing bit

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLfloat)ww, 0.0, (GLfloat)hh);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

 glColor3f(0.9,0.5,0);
 glColor3f(1, 1, 1);
 //drawstr(10,10, "b=GL_BLEND");
 //drawstr(10,25, "m=GL_MODULATE");
 //drawstr(10,40, "d=GL_DECAL");
 //drawstr(10,55, "n=GL_NEAREST");
 //drawstr(10,70, "l=GL_LINEAR");
 drawstr(10,85, "e=Skybox at eye or 0,0,0");
 drawstr(10,99, "0 to 9 = diferent views");
 glRasterPos2f(200,40);
 glPixelZoom(1,1);
 //glBindTexture(GL_TEXTURE_2D, ttex1);
 /*
 glDrawPixels(butt.BitmapInfo->bmiHeader.biWidth,
               butt.BitmapInfo->bmiHeader.biHeight,
               GL_BGR_EXT, GL_UNSIGNED_BYTE, butt.BitmapBits);
  */
  //glFlush();

  //drawBmpButton();


  err = glGetError();
  if (err == GL_NO_ERROR)
		glutSwapBuffers();
}

static void spinDisplay (void)
{
  if (spinning) Spin += 0.02;
  if (Spin > 360.0)
    Spin -= 360.0;
  glutPostRedisplay();
}

static void menuChoice (int item)
{
  switch (item) {
    case cmdNoCull:
      glDisable(GL_CULL_FACE);
      break;
    case cmdCWFront:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CW);
      break;
    case cmdCCWFront:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CCW);
      break;
    case cmdExit:
      exit(0);
      break;
    default:
      break;
  }
}

void myExitfn()
{
printf("Exiting\n");
free(butt.BitmapBits);
free(butt.BitmapInfo);
free(texture.BitmapBits);
free(texture.BitmapInfo);
}

static void initGraphics (void)
{
int rc;

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  Spin = 0.0;

  AppMenu = glutCreateMenu(menuChoice);
  glutSetMenu(AppMenu);
  glutAddMenuEntry("No cull", cmdNoCull);
  glutAddMenuEntry("CW faces", cmdCWFront);
  glutAddMenuEntry("CCW faces", cmdCCWFront);
  glutAddMenuEntry("----", 0);
  glutAddMenuEntry("Exit", cmdExit);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  rc=LoadBMP(GROUND_BITMAP_FILE, &butt,false);
  if (!rc) printf("ERROR with bitmap\n");

  rc=LoadBMP(TEXTURE_BITMAP_FILE, &texture,false);
  if (!rc) printf("ERROR with bitmap texture\n");

  rc=LoadBMP(SKYBOX_BITMAP_FILE, &skyTexStruct,false);
  if (!rc) printf("ERROR with bitmap skybox texture\n");

  glGenTextures(4, ttex);

  glBindTexture(GL_TEXTURE_2D, ttex[2]);
  glTexImage2D(GL_TEXTURE_2D,0,3,
   	           skyTexStruct.BitmapInfo->bmiHeader.biWidth,
               skyTexStruct.BitmapInfo->bmiHeader.biHeight,
  	           0,GL_BGR_EXT,GL_UNSIGNED_BYTE,skyTexStruct.BitmapBits);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glBindTexture(GL_TEXTURE_2D, ttex[0]);
  glTexImage2D(GL_TEXTURE_2D,0,3,
   	           butt.BitmapInfo->bmiHeader.biWidth,
               butt.BitmapInfo->bmiHeader.biHeight,
  	           0,GL_BGR_EXT,GL_UNSIGNED_BYTE,butt.BitmapBits);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  
  glBindTexture(GL_TEXTURE_2D, ttex[1]);
  glTexImage2D(GL_TEXTURE_2D,0,3,
	           texture.BitmapInfo->bmiHeader.biWidth,
               texture.BitmapInfo->bmiHeader.biHeight,
        	   0,GL_BGR_EXT,GL_UNSIGNED_BYTE,texture.BitmapBits);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


//set the global lighting
glShadeModel(GL_FLAT); // or GL_SMOOTH
//glEnable(GL_NORMALIZE); //or not
glEnable(GL_LIGHTING);
//set the global ambient light
glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

glEnable(GL_LIGHT0); //enable the light



  atexit(myExitfn);
}

static void resize (int width, int height)
{
  ww=width;
  hh=height;
}

static void asciiKey (unsigned char key, int x, int y)
{
  if (key == 27) /* ESC */
    exit(0);
  if (key == 'd' || key == 'D') decal=1; // decal
  if (key == 'b' || key == 'B') decal=2; // blend
  if (key == 'm' || key == 'M') decal=3; // modulate  
  
  if (key == 'n' || key == 'N') filter=1; // GL_NORMAL
  if (key == 'l' || key == 'L') filter=2; // GL_LINEAR (bilinear)
  if (key == 'e' || key == 'E') atEye=!atEye;; 
  
  if (key == ' ' ) spinning=!spinning;

  if (key == '0') {eye[0]=4; eye[1]=2; eye[2]=4;}
  if (key == '1') {eye[0]=30; eye[1]=4; eye[2]=30;}
  if (key == '2') {eye[0]=7; eye[1]=3; eye[2]=7;}
  if (key == '3') {eye[0]=20; eye[1]=3; eye[2]=20;}
  if (key == '4') {eye[0]=20; eye[1]=4; eye[2]=10;}
  if (key == '5') {eye[0]=2; eye[1]=25; eye[2]=2;}
  if (key == '6') {eye[0]=9; eye[1]=1; eye[2]=9;}
  if (key == '7') {eye[0]=1; eye[1]=50; eye[2]=1;}
  if (key == '8') {eye[0]=1; eye[1]=900; eye[2]=1;}
  if (key == '9') {eye[0]=-10; eye[1]=0.5; eye[2]=-12;}


   glutPostRedisplay();
}

int main (int argc, char * argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 75);
  glutCreateWindow("Cube");

  initGraphics();
  
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutIdleFunc(spinDisplay);
  glutKeyboardFunc(asciiKey);

  oglVersion=glGetString(GL_VERSION);
  printf("openGl version %s \n",oglVersion);
  glutMainLoop();	
  return 0;
}





