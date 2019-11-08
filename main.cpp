//  Draw objects using Vertex Array construct with indexed geometry.

#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <iostream>
#include <cmath>

#define GLUT_DISABLE_ATEXIT_HACK
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "bitmap.h"
#include "vec3.h"
#include "lights.h"
#include "OrthoFrame.h"
#include "mesh.h"
#include "axes.h"
#include "grid.h"

#include "texturePoly.h"
#include "terrain.h"

#include "Psystem.h"

using namespace std;

//triginometry definitions
const float rad2Deg = 180.0 / M_PI;
const float deg2Rad = M_PI / 180.0;
#define degrees(a) ((a)*rad2Deg)
#define radians(a) ((a)*deg2Rad)
#define miliseconds(a) (a*(1/10));

//camera definitions
#define Near  0.5
float Far = 150.0;//adjusts realtime
vec3 playerPosition(0.0, 0.0, 4.0);//player pos (do not change)
vec3 homePosition = vec3(0, 2, -3.6);//player camera
vec3 rotationAxis(0.0, 4.0, 0.0);
float   cameraStep = 0.1;
float   cameraAngleStep = 1.0 * M_PI / 180.0;  // This is 1 degree, in rads
float   cameraTfm[16];
static int FOV = 60;
OrthoFrame* playerControlled;

//camera command list
enum commandList {
	cmdNull,
	cmdCameraMode,
	cmdMoveMode,
	cmdMesh,
	cmdFill,
	cmdWire,
	cmdFlat,
	cmdSmooth,
	cmdCull,
	cmdNoCull,
	cmdFullScreen,
	cmdToggleCamera,
	cmdIncreaseFClip,
	cmdDecreaseFClip,
	cmdToggleSun,
	cmdReturnHome,
	cmdToggleHeightFeild,
	cmdExit
};

//lighting and materials
GLfloat FOG_COLOR[4] = { .1,.1,.1,.1 };

MaterialClass ground(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

MaterialClass building(
	0.1, 0.1, 0.1,                 // ambient
	0.914, 0.4284, 0.18144,                 // diffuse
	1.2, 1.2, 1.0,                 // specular
	2                            // shininess
);

MaterialClass rock(
	.1, .1, .1,                 // ambient
	0.714, 0.4284, 0.18144,                  // diffuse
	1.2, 1.2, 1.0,             // specular
	32                            // shininess
);

MaterialClass whiteStuff(
	1.0, 1.0, 1.0,                 // ambient
	1.0, 1.0, 1.0,                // diffuse
	1.0, 1.0, 1.0,                // specular
	32                            // shininess
);
MaterialClass bronze(
	0.2125, 0.1275, 0.054,         // ambient
	0.714, 0.4284, 0.18144,        // diffuse
	0.393548, 0.271906, 0.166721,  // specular
	26                            // shininess
);
MaterialClass brass(
	0.329412, 0.223529, 0.027451,
	0.780392, 0.568627, 0.113725,
	0.992157, 0.941176, 0.807843,
	(int)(0.21794872 * 128)
);
MaterialClass orangePlastic(
	0.35, 0.15, 0.0,              // ambient
	0.75, 0.4, 0.0,               // diffuse
	0.35, 0.25, 0.25,             // specular
	18                            // shininess
);
MaterialClass whitePlastic(
	0.0, 0.0, 0.0,                 // ambient
	0.55, 0.55, 0.55,             // diffuse
	0.7, 0.7, 0.7,                // specular
	32                            // shininess
);
MaterialClass bluePlastic(
	0.1, 0.1, 0.2,                 // ambient
	0.3, 0.3, 0.8,                 // diffuse
	0.6, 0.6, 0.6,                  // specular
	32.0                            // shininess
);
MaterialClass yellowPlastic(
	0.2, 0.2, 0.2,                  // ambient
	0.7, 0.7, 0.0,                 // diffuse
	0.7, 0.7, 0.6,                 // specular
	45                        // shininess
);
MaterialClass emerald(
	0.0215, 0.1745, 0.0215,       // ambient
	0.07568, 0.61424, 0.07568,    // diffuse
	0.633, 0.727811, 0.633,       // specular
	77                               // shininess
);

LightClass sun;
//const unsigned int SHADOW_WIDTH, SHADOW_HEIGHT = 1024;
unsigned int depthMap;
unsigned int depthMapFBO;

//Mesh definitions
//GLuint cokeTexture;
GLuint skyboxTextureHandle;
OrthoFrame* camera;
Cube* cube; Mesh* skybox;

//player (tank)
GLuint launchTextureHandle;
PsystemStruct *launchEffect;
MoveableMesh* TankBody; MoveableMesh* Turrent;
MoveableMesh* Wheel1; MoveableMesh* Wheel2; MoveableMesh* Wheel3;
MoveableMesh* MouseTarget;

float WHEEL_SPEED = 25;
float TURRENT_SPEED = .1;//turrent speed based on mouse
float WHEEL_TURN_OFFSET = .03;
float isReverse;

//scene decoration
Mesh* Boulder1; Mesh* Boulder2;
Mesh* Boulder3; Mesh* Boulder4;
Mesh* Boulder5; Mesh* Boulder6;
Mesh* Boulder7; Mesh* Boulder8;

GLuint buildingTextureHandle;
Mesh* BuildingS1; Mesh* BuildingS2;
Mesh* BuildingS3; Mesh* BuildingL1;
Mesh* BuildingL2;

Axes* axes; Grid* grid;
GLuint concreteTextureHandle;

//configuration
bool spinning, wireFrame, smoothShading, culling, cameraMode, notexturing, isMoving, sunEnabled, terrainEnabled;
GLfloat	spinAngle, spinAngleStep = 0.2 * 2 * M_PI / 360.0;
static int	WIN_WIDTH, WIN_HEIGHT;
static int  MAX_PITCH_ANGLE = 1.5; //in radians
const GLubyte* OGL_VER;
float oldmouseX = 0; float oldmouseY = 0; float angleX, angleY;
int DELTA_TIME; int oldTimeSinceStart = 0; int timeSinceStart = 0;

#define framesPerSecond 60//25, now 60 for good computer
#define timerMSecs ((int)1000/framesPerSecond)  // compute time in miliseconds for a frame
#define lerp(from,to,smooth)(from+smooth * (to-from))

bool timeForNextFrame = false;

//terrain (heightfeild) configuration
terrain* ter;
GLuint terTextureHandle;
bool fillq = false;
GLfloat TERRAIN_WIDTH = 10; GLfloat TERRAIN_HEIGHT = 10;

//step should be a percent less than 1.0 and determines accuracy of the detection
static float detectGround(Mesh* object, terrain* ter, float sensitivity = 1.00005, int xoffset = 0, int zoffset = 0) {
	vec3 currentPosition = object->frame.getPosition();
	currentPosition.x -= xoffset; currentPosition.y -= zoffset;

	//get difference
	float res = (ter->getHeight(currentPosition) - currentPosition.y);
	cout << "dif =" << res << endl;
	return sqrt(res*res)/*always positive comparison*/ > sensitivity - 1 ? res * sensitivity : 0.0;//0 if no collision, heightfeild height + 1 if collision
}
void showBMPInfo(CBitmap& bmp)
{
	cout << "BMP data loaded:" << endl;
	cout << "  Width:   " << bmp.GetWidth() << endl;
	cout << "  Height:  " << bmp.GetHeight() << endl;
	cout << "  Bits per pixel: " << bmp.GetBitCount() << endl;
}
static void drawShadows() {
	//src https://www.opengl.org/archives/resources/features/StencilTalk/sld005.htm
	glStencilFunc(GL_STENCIL_FUNC, launchTextureHandle, GL_STENCIL_WRITEMASK);//requires you pass in a texture ref
	//glStencilMask();
	glStencilOp(GL_STENCIL_FAIL, GL_STENCIL_PASS_DEPTH_FAIL, GL_STENCIL_PASS_DEPTH_FAIL);
	glClear(GL_STENCIL_BITS | GL_STENCIL_BUFFER_BIT);
}

//where we configure how things render each frame
void display(void)
{
	//get difference in time from last frame to this frame (GET DELTA TIME)
	oldTimeSinceStart = 0;
	timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	DELTA_TIME = (timeSinceStart - oldTimeSinceStart);//in miliseconds

	oldTimeSinceStart = timeSinceStart;

	//gl configuration
	//drawShadows();
	glClearColor(FOG_COLOR[0], FOG_COLOR[1], FOG_COLOR[2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear z buffer
	glClearDepth(Far);//clear depth buffer up till far clip

	glEnable(GL_LIGHTING);
	if (wireFrame)glPolygonMode(GL_FRONT, GL_LINE);
	else glPolygonMode(GL_FRONT, GL_FILL);

	if (smoothShading) glShadeModel(GL_SMOOTH);
	else glShadeModel(GL_FLAT);

	if (culling) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glDepthRange(Near, Far);
	glEnable(GL_NORMALIZE);//LIGHTING

	glLoadIdentity();

	glPushMatrix(); //or else transparent objects will fuck up
	axes->render();
	//camera and lighing do camera last
	// First apply camera transformation.

			//player movement (player is a tank), this is the top level positioning for child mesh's in scene graph (will be refactored)
	vec3 tankPosition = TankBody->frame.getPosition();

	//WARNING, CHANGES IDENTITY MATRIX, ALL POSITIONING MUST HAPPEN AFTER THIS IF FETCHING FROM ORTHOFRAME!
	if (playerControlled == camera) {
		cout << "watching player with camera" << TankBody->frame.getPosition() << endl;
		camera->setLookAtPosition(tankPosition);
		camera->cameraTransform();//causes some fucked up problems... I think it modifies identity matrix...
		// Move camera relative to tank
	}

	//particles
	if (launchEffect->active) {
		glPushMatrix();
		glClearColor(0.1, .1, .1, .1);//clear fog
		whiteStuff.doMaterial();//clean material to render onj
		glFrontFace(GL_CCW);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_COLOR_BUFFER_BIT);
		launchEffect->texHandle = launchTextureHandle;
		launchEffect->pos = Turrent->frame.getPosition();
		//glRotatef(90, 1, 0, 0);
		updatePsystem(launchEffect);
		drawPsystem(launchEffect);
		glPopMatrix();
	}

	//handle player collision with ground (heightfield), this is calculated relative to the middle wheel (could use all wheels for more accuracy)
	float playerGroundCollision = detectGround(Wheel1, ter, 1.05);
	cout << "Debug display.. ismoving: " << isMoving << " Tank position " << Wheel1->frame.getPosition() << " Collisions? " << playerGroundCollision << endl;
	if (playerGroundCollision != 0.0) {
		//previous positions
		vec3 pos1 = TankBody->frame.getPosition();
		vec3 pos2 = Turrent->frame.getPosition();
		vec3 pos3 = Wheel1->frame.getPosition();
		vec3 pos4 = Wheel2->frame.getPosition();
		vec3 pos5 = Wheel3->frame.getPosition();

		//set new position
		TankBody->frame.setPosition(vec3(pos1.x, pos1.y + playerGroundCollision, pos1.z));
		Turrent->frame.setPosition(vec3(pos2.x, pos2.y + playerGroundCollision, pos2.z));
		Wheel1->frame.setPosition(vec3(pos3.x, pos3.y + playerGroundCollision, pos3.z));
		Wheel2->frame.setPosition(vec3(pos4.x, pos4.y + playerGroundCollision, pos4.z));
		Wheel3->frame.setPosition(vec3(pos5.x, pos5.y + playerGroundCollision, pos5.z));

		////calculate new angle for tank to have from wheel1's perspective
		//float theta = findAngle(Wheel1->frame.getPreviousPosition(), pos1);
		//cout << findAngle(pos1, TankBody->frame.getPreviousPosition()) << endl;

		//GLint upordown = playerGroundCollision > 0 ? 1 : -1;

		////rotate tank attempting to match terrains angle
		//TankBody->setRot(theta*upordown * 100, 0, 0);
		//Turrent->setRot(theta*upordown * 100, 0, 0);

		//aply new positon
		TankBody->frame.applyTransform();
		Turrent->frame.applyTransform();
		Wheel1->frame.applyTransform();
		Wheel2->frame.applyTransform();
		Wheel3->frame.applyTransform();
	}

	{//brackets help me know where im at... also creates a seperate stack ohwell
		//if (!TankBody || !cube || !Wheel1 || !Wheel2 || !Wheel3 || !Turrent) { cout << "One of your objects is null!!!" << endl; exit(0); }//if any object null exit immedietly
		glPushMatrix();
		bluePlastic.doMaterial();
		cube->render();
		cube->setRot(0.0, degrees(2 * (spinAngle)), 0.0); // extra spinning for cube.
		glPopMatrix();

		//float tankRotation = TankBody->frame.getRotRadian();

		//cout << "x: " << tankPosition.x << " y: " << tankPosition.y << " z: " << tankPosition.z << endl;

		//tank body and start of tank scene graph
		glPushMatrix();//copy identity (at origin) for the tank

					   //glTranslatef(tankPosition.x, tankPosition.y, tankPosition.z);//everyone translates the same
		//glRotatef(tankRotation, 0, 1, 0);//everyone rotates the same
		//brass.doMaterial();

		TankBody->render();//render tank body with own box collider, cannon with particles
		axes->render();

		glPushMatrix();
		//turrent
		bronze.doMaterial();
		Turrent->render();
		Turrent->setRot(270, 0, oldmouseX*TURRENT_SPEED);
		glPopMatrix(); //go back to tankbodys position

		//render wheels relative to the tank body
		glPushMatrix();
		if (isMoving) {
			Wheel1->setRot(0, oldmouseX * WHEEL_TURN_OFFSET, 0);
			Wheel1->setRot(degrees(2 * (isReverse ? -spinAngle * WHEEL_SPEED : spinAngle * WHEEL_SPEED)), 0, 0);
		}
		Wheel1->render();
		bronze.doMaterial();
		axes->render();
		glPopMatrix();

		glPushMatrix();
		bronze.doMaterial();
		if (isMoving) {
			Wheel2->setRot(0, oldmouseX * WHEEL_TURN_OFFSET, 0);
			Wheel2->setRot(degrees(2 * (isReverse ? -spinAngle * WHEEL_SPEED : spinAngle * WHEEL_SPEED)), 0, 0);
		}
		Wheel2->render();
		glPopMatrix();

		glPushMatrix();
		bronze.doMaterial();
		if (isMoving) {
			Wheel3->setRot(0, oldmouseX * WHEEL_TURN_OFFSET, 0);
			Wheel3->setRot(degrees(2 * (isReverse ? -spinAngle * WHEEL_SPEED : spinAngle * WHEEL_SPEED)), 0, 0);
		}
		Wheel3->render();
		glPopMatrix();

		glPopMatrix();//finish rendering tank

		//render scene objects with box colliders
		glPushMatrix();
		rock.doMaterial();
		Boulder1->render();
		Boulder2->render();
		Boulder3->render();
		Boulder4->render();
		Boulder5->render();
		Boulder6->render();
		Boulder7->render();
		Boulder8->render();
		glPopMatrix();

		glPushMatrix();
		building.doMaterial();
		BuildingS1->render();
		BuildingS2->render();
		BuildingS3->render();

		BuildingL1->render();
		BuildingL2->render();
		glPopMatrix();

		//texturing

		whiteStuff.doMaterial();

		//skybox IS A SKYSPHERE
		glPushMatrix();//counts as the minimum texture
		//glClearColor(0.0, 0.0, 0.0, 0.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, skyboxTextureHandle);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		skybox->render();
		glPopMatrix();

		//lighting
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glPushMatrix();
		//rotate pivot point, then translate outwards from this
		glRotatef(degrees(spinAngle) * 2, rotationAxis.x, rotationAxis.y, rotationAxis.z);
		glTranslatef(2, 5, 2);
		if (sunEnabled) {
			sun.letThereBeLight();
		}
		glPopMatrix();
		//rotates world

		//grid
		glPushMatrix();
		//glEnable(GL_TEXTURE_2D);
		//ground.doMaterial();
		glFrontFace(GL_CW);
		//glBindTexture(GL_TEXTURE_2D, concreteTextureHandle);
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glEnable(GL_DEPTH_TEST);
		//glEnable(GL_NORMALIZE);//LIGHTING
		if (wireFrame) {
			glRotatef(90, 0, 1, 0);
			grid->renderLines();
			glRotatef(90, 0, 1, 0);
			grid->renderLines();
			glRotatef(90, 0, 1, 0);
			grid->renderLines();
		}
		else {
			glRotatef(90, 0, 1, 0);
			grid->render();
			glRotatef(90, 0, 1, 0);
			grid->render();
			glRotatef(90, 0, 1, 0);
			grid->render();
		}
		glPopMatrix();

		glPushMatrix();
		if (terrainEnabled) {
			glFrontFace(GL_CW);
			if (wireFrame) ter->renderLines();
			else ter->render();
		}
		else {
			///glRotatef(0, 0, 1, 0);
			if (wireFrame) grid->renderLines();
			else grid->render();
		}
		glPopMatrix();
	}
	glPopMatrix();

	isMoving = false; isReverse = false;//always false at end of frame, only changes on keyboard input

	int err = glGetError();
	if (err == GL_NO_ERROR)
		glutSwapBuffers();
	else
	{
		cout << "OpenGL error occurred:  " << endl;
		cout << "  " << gluErrorString(err) << endl;
		cout << "  Freezing...  (Type 'q' to quit):" << endl;
		char response;
		cin >> response;
		if (response == 'q')
			exit(1);
	}
}

//idle frames
static void idle(void)
{
	if (spinning) {
	}
	spinAngle = fmod(spinAngle + spinAngleStep, float(2 * M_PI));
	glutPostRedisplay();
}

//menus
static void fillMenuChoice(int item)
{
	switch (item) {
	case cmdFill:
		wireFrame = GL_FALSE;
		break;
	case cmdWire:
		wireFrame = GL_TRUE;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
static void performanceMenuChoice(int item)
{
	GLfloat aspect = (float)WIN_WIDTH / (float)WIN_HEIGHT;
	switch (item) {
	case cmdIncreaseFClip:
		Far >= 15 ? Far -= 15 : printf("Min far clip reached");

		gluPerspective(FOV, aspect, Near, Far);
		break;
	case cmdDecreaseFClip:
		Far >= 15 ? Far -= 15 : printf("Min far clip reached");
		gluPerspective(FOV, aspect, Near, Far);
		break;
	case cmdToggleSun:
		sunEnabled = !sunEnabled;
	case cmdToggleHeightFeild:
		terrainEnabled = !terrainEnabled;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
static void shadingMenuChoice(int item)
{
	switch (item) {
	case cmdFlat:
		smoothShading = GL_FALSE;
		break;
	case cmdSmooth:
		smoothShading = GL_TRUE;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
void cullingMenuChoice(int item)
{
	switch (item) {
	case cmdCull:
		culling = GL_TRUE;
		break;
	case cmdNoCull:
		culling = GL_FALSE;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
void appMenuChoice(int item)
// Bind actions to menu choices...
{
	switch (item) {
	case cmdToggleCamera:
		playerControlled->ToggleAttatchement();
	case cmdFullScreen:
		glutFullScreen();
		glutPostRedisplay();
		break;
	case cmdReturnHome:
		if (playerControlled == camera) {
			playerControlled->goHome();
			playerControlled->setPosition(homePosition);

			TankBody->frame.goHome();
			TankBody->frame.setPosition(homePosition);

			Turrent->frame.goHome();
			Turrent->frame.setPosition(homePosition);

			Wheel1->frame.goHome();
			Wheel1->frame.setPosition(homePosition);

			Wheel2->frame.goHome();
			Wheel2->frame.setPosition(homePosition);

			Wheel3->frame.goHome();
			Wheel3->frame.setPosition(homePosition);
		}
		break;
	case cmdExit:
		exit(0);
		break;
	default:
		break;
	}
}
void moveModeMenuChoice(int item)
// Bind actions to menu choices...
{
	switch (item) {
	case cmdCameraMode:
		cameraMode = true;
		playerControlled = camera;
		break;
	case cmdMoveMode:
		cameraMode = true;
		playerControlled = &(TankBody->frame);
		break;
	default:
		break;
	}
}

//initialization
static void buildMenus(void)
{
	int appMenu, objectMenu, cullingMenu, shadingMenu, fillMenu, moveModeMenu, performanceMenu;

	appMenu = glutCreateMenu(appMenuChoice);
	fillMenu = glutCreateMenu(fillMenuChoice);
	shadingMenu = glutCreateMenu(shadingMenuChoice);
	cullingMenu = glutCreateMenu(cullingMenuChoice);
	moveModeMenu = glutCreateMenu(moveModeMenuChoice);
	performanceMenu = glutCreateMenu(performanceMenuChoice);

	glutSetMenu(moveModeMenu);
	glutAddMenuEntry("Camera mode", cmdCameraMode);
	glutAddMenuEntry("Move mode", cmdMoveMode);

	glutSetMenu(fillMenu);
	glutAddMenuEntry("Filled polygons", cmdFill);
	glutAddMenuEntry("Wire frame", cmdWire);

	glutSetMenu(shadingMenu);
	glutAddMenuEntry("Smooth shading", cmdSmooth);
	glutAddMenuEntry("Flat shading", cmdFlat);

	glutSetMenu(cullingMenu);
	glutAddMenuEntry("Enable culling", cmdCull);
	glutAddMenuEntry("Disable culling", cmdNoCull);

	//added in after demonstration to account for performance needs
	glutSetMenu(performanceMenu);
	glutAddMenuEntry("Increase Far plane", cmdIncreaseFClip);
	glutAddMenuEntry("Decrease Far plane", cmdDecreaseFClip);
	glutAddMenuEntry("Toggle Lighting (wont change ambient lighting)", cmdToggleSun);
	glutAddMenuEntry("Toggle HeightFeild (significant boost)", cmdToggleHeightFeild);

	glutSetMenu(appMenu);//mainmenu
	glutAddSubMenu("Movement bindings", moveModeMenu);
	glutAddSubMenu("Polygon fill", fillMenu);
	glutAddSubMenu("Shading", shadingMenu);
	glutAddSubMenu("Culling", cullingMenu);
	glutAddSubMenu("Performance", performanceMenu);
	glutAddMenuEntry("Full Screen", cmdFullScreen);
	glutAddMenuEntry("Return Home (if in camera mode)", cmdReturnHome);

	glutAddMenuEntry("Detatch camera", cmdToggleCamera);
	glutAddMenuEntry("----", cmdNull);
	glutAddMenuEntry("Exit", cmdExit);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
static void initializeParticle(PsystemStruct *particle, int num, bool texture = false, GLuint *texHandle = 0) {
	particle->active = false;
	particle->activeticks = -1;
	particle->typeOfPsystem = 2;
	particle->texHandle = *texHandle;           // sane value
	particle->billboard = true;       // sane value
	v3FloatToVecM(particle->pos, num % 4, 0, num / 4);
	particle->emissionRate = 1;      // number of particles per tick (can be fractional)
	particle->emmisionRateRandom = 0;  // a max random number which we div by 100 and add to emision rate
	particle->residualEmmision = 0;
	particle->maxParticles = 3;      // no more than this please
	particle->numParticles = 6;        // the actual number
	particle->particleSize = 1;        // size in points
	particle->particleSizeDelta = 0;   // no change
	particle->max_ticks = 100;         // after this many ticks die
	v3FloatToVec(-10000, 0, -10000, particle->dieIfLess);      // kill the particl if its less than any of these values
	//v4FloatToVecM(particle->colour, 0.1, 0.8, 0.1, 1); // particle color
	//v4FloatToVecM(particle->colourChange, 0.06, 0.06, 0.06, 0);   // colour drift + added each tick
	//v4FloatToVecM(particle->colourMax, 1, 1, 1, 1);
	v3FloatToVec(0, -0.02, 0, particle->gravity);    // velosity change due to gravity
	v3FloatToVec(0, 0, 0, particle->wind);           // movement due to wind
	v3FloatToVec(-0.025, 0.3, -0.025, particle->initialVelosity);
	v3FloatToVec(5, 5, 5, particle->initialVelosityRandom); // max random number which we dive by 100 and add to initial velosity
	v3FloatToVec(0, 0, 0, particle->airResistance);       // how it slows
	v3FloatToVec(5, 5, 5, particle->absTerminalVelosity);    // max speed

	for (int i = 0; i < MAX_PSYSTEM; i++)
	{
		particle->p[i].active = false;   // active Y or n
		particle->p[i].time = 0;      // ticks of activity
		v3FloatToVec(0, 0, 0, particle->p[i].pos);      // position
		v3FloatToVec(0, 0, 0, particle->p[i].velosity); // how fast do i move
	}
}
void initializeTexture(string fileName, GLuint *texHandle, bool mipmap = true, bool transparency = false) {
	// As the texBitmp goes out of scope it will be destroyed.
	// But the pixels have been copied into OpenGL's memory area,
	// so that is okay.
	CBitmap texBitmap;//create a bitmap class to use its operations
	const char* filename_Char = fileName.c_str();
	bool success = texBitmap.Load(filename_Char);
	if (success)
	{
		int texWidth = texBitmap.GetWidth();
		int texHeight = texBitmap.GetHeight();
		if (texWidth == 0 || texHeight == 0 || texBitmap.GetBits() == NULL)
		{
			char response;
			cerr << "Unable to load BMP file " << fileName << endl;
			cerr << "Press 'q' <ENTER> to exit: ";
			cin >> response;
			exit(1);
		}
		// OK, looks like we got BMP data.
		cout << "Loaded BMP file  " << fileName << endl;
		showBMPInfo(texBitmap);
		// The file read has arranged it to be RGBA
		// (whereas 24-bit BMPs are natively in BGR).
		// Now attach it to OpenGL texture...
		//glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&texHandle);
		if (transparency)
			texBitmap.SetAlphaBits('0');

		glGenTextures(1, texHandle);//must be in the same context as the object, if we refrence this in a funciton it wont work because we dont copy opengl objects, we must refrence or point to them!
		glBindTexture(GL_TEXTURE_2D, *texHandle);
		cout << "Attaching pixel data from " << fileName << " to texture handle "
			<< texHandle << endl;
		if (mipmap) {
			//DEPRECATED CALL IN MODERN OPENGL
			int returnCode = gluBuild2DMipmaps(
				GL_TEXTURE_2D,       // target type of texture
				GL_RGBA,              // format in which GL should store it
				texWidth, texHeight, // [obvious]
				GL_RGBA,             // format of data
				GL_UNSIGNED_BYTE,    // each R,G,B,A component is a byte
				texBitmap.GetBits()    // pointer to location of pixel data
			);
			if (returnCode != 0)
				cout << gluErrorString(returnCode) << endl;
		}
		else {
			glTexImage2D(
				GL_TEXTURE_2D, 0, 4,//rgba
				texWidth, texHeight,
				0,
				GL_RGBA, GL_UNSIGNED_BYTE, texBitmap.GetBits()
			);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);//use clamp not repeat for better fits
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
		cout << "Error: Unable to load BMP file " << fileName << endl;
}
MoveableMesh* initializeMesh(string fileName, GLenum vertexOrder = GL_CCW, vec3 initRot = vec3(0, 0, 0), vec3 initTrans = vec3(0, 0, 0), vec3 initScale = (0.2, 0.2, 0.2)) {
	cout << endl << "Creating " << fileName << " mesh..." << endl;

	MoveableMesh* mesh = new MoveableMesh();

	char* fileName_char = new char[fileName.length() + 1];
	strcpy(fileName_char, fileName.c_str());
	cout << mesh->isTextured << endl;
	bool success = mesh->read3dsFile(fileName_char);//expects char* NOT const char*
	if (success)
	{
		mesh->vertexOrder = vertexOrder;
		mesh->name = fileName;
		mesh->findVertexNormals();
		mesh->frame.setPosition(vec3(0.0, 0.0, 0.0));//camera movement pos
		mesh->setTranslate(initTrans.x, initTrans.y, initTrans.z);
		mesh->setRot(initRot.x, initRot.y, initRot.z);
		mesh->setScale(initScale.x, initScale.y, initScale.z);
		mesh->printProperties(true);  // Recursive print of properties.
	}
	else
	{
		cout << "Error: could not read in file" << fileName << endl;
	}
	delete[] fileName_char;
	return mesh;
}
static void initialise(void)
{
	bool success;
	sunEnabled = true;
	terrainEnabled = true;

	//camera
	cout << "  Creating camera..." << endl;
	camera = new OrthoFrame();
	camera->setPosition(homePosition);

	//gl config

	//color and depth testing
	glClearColor(0, 0, 0, 1.0);
	// Draw back-facing polygons in wire-frame.
	// That way we'll know if some accidentally become visible...
	glPolygonMode(GL_BACK, GL_LINE);
	glLineWidth(2);
	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	//fog
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, FOG_COLOR);
	glFogf(GL_FOG_DENSITY, 0.021);
	glFogf(GL_FOG_START, 0);
	glFogf(GL_FOG_END, 80);

	//trees
	////make a scene graph function to construct one with textured gluquadric planes
	//GLUquadric* leave1 = gluNewQuadric();
	//gluQuadricNormals(leave1, GLU_FLAT);//FLAT SHADED NOT SMOOTH
	//gluQuadricTexture(leave1, GL_TRUE);

	//scene decoration
	Boulder1 = initializeMesh("Boulder.3ds", GL_CCW, vec3(-90, 0, 34), vec3(30, 0, 0), vec3(1, 1, 1));
	Boulder2 = initializeMesh("Boulder.3ds", GL_CCW, vec3(-90, 0, 0), vec3(8, 0, 23), vec3(1, 1, 1));
	Boulder3 = initializeMesh("Boulder.3ds", GL_CCW, vec3(-90, 0, 90), vec3(-19, 0, 0), vec3(1, 1, 1));
	Boulder4 = initializeMesh("Boulder.3ds", GL_CCW, vec3(-90, 0, 0), vec3(9, 0, 23), vec3(1, 1, 1));
	Boulder5 = initializeMesh("Boulder.3ds", GL_CCW, vec3(-90, 0, 45), vec3(-10, 0, 4), vec3(1, 1, 1));
	Boulder6 = initializeMesh("Boulder.3ds", GL_CCW, vec3(-90, 0, 0), vec3(9, 0, 0), vec3(1, 1, 1));
	Boulder7 = initializeMesh("Boulder.3ds", GL_CCW, vec3(-90, 0, 24), vec3(-14, 0, 0), vec3(1, 1, 1));
	Boulder8 = initializeMesh("Boulder.3ds", GL_CCW, vec3(-90, 0, 0), vec3(20, 0, 24), vec3(1, 1, 1));

	BuildingS1 = initializeMesh("ShortBuilding.3ds", GL_CCW, vec3(-90, 0, 50), vec3(28, 0, 10), vec3(1, 1, 1));
	BuildingS2 = initializeMesh("ShortBuilding.3ds", GL_CCW, vec3(-90, 0, 80), vec3(-15, 0, -10), vec3(1, 1, 1));
	BuildingS3 = initializeMesh("ShortBuilding.3ds", GL_CCW, vec3(-90, 0, -50), vec3(-17, 0, 20), vec3(1, 1, 1));

	BuildingL1 = initializeMesh("TallBuilding.3ds", GL_CCW, vec3(-90, 0, 90), vec3(20, 0, 10), vec3(1, 1, 1));
	BuildingL2 = initializeMesh("TallBuilding.3ds", GL_CCW, vec3(-90, 0, -50), vec3(-5, 0, 20), vec3(1, 1, 1));

	//skybox
	cout << "creating skybox" << endl;

	//initializeTexture("sky.bmp", skyboxTextureHandle, skyboxBitmap);
	skybox = initializeMesh("skybox.3ds", GL_CCW, vec3(90, 0, 0), vec3(0, 33, 0), vec3(5.4, 5.4, 5.4));
	skybox->isTextured = true;
	initializeTexture("sky.bmp", &skyboxTextureHandle);

	cout << "Initialising..." << endl;

	//Mouse Target Mesh
	//MouseTarget = initializeMesh("MouseTarget.3ds", GL_CCW, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1.2, 1.2, 1.2));

	//runtime configuration
	spinAngle = 0.0;
	spinning = false;
	wireFrame = false;
	smoothShading = true;
	culling = true;

	cout << "  Building menus..." << endl;
	buildMenus();

	//ground
	//ground.setMaterialYellowSand();
	grid = new Grid();
	grid->setUp(50, 50, 25, 25);
	//glGenTextures(1, &concreteTextureHandle);//must be in the same context as the object, if we refrence this in a funciton it wont work because we dont copy opengl objects, we must refrence or point to them!
	//initializeTexture("concrete.bmp", concreteTextureHandle);

	//misc objects
	axes = new Axes();

	cout << endl << "Creating cube mesh..." << endl;
	cube = new Cube(0.5);
	cube->setTranslate(-1.0, 0.2, 0.1);
	cube->setRot(0.0, 0.0, 0.0);
	cube->setScale(0.8f, 0.4f, 0.2f);
	cube->printProperties();

	//offset height by .3 was .1
	//offset sizing by .35
	//offset wheels by .2
	TankBody = initializeMesh("TankBody.3ds", GL_CCW, vec3(-90, 0, 0), vec3(0, 0.6, 0), vec3(0.35, 0.35, 0.35));
	TankBody->frame.setPosition(vec3(0, 0, 0));

	Turrent = initializeMesh("Turrent.3ds", GL_CCW, vec3(0, 0, 0), vec3(0, 0.93, -0.4), vec3(0.35, 0.35, 0.35));
	Turrent->frame.setPosition(vec3(0, 0, 0));

	Wheel1 = initializeMesh("Wheel.3ds", GL_CCW, vec3(0, 0, 0), vec3(0, 0.3, 0.7), vec3(0.35, 0.35, 0.35));
	Turrent->frame.setPosition(vec3(0, 0, 0));

	Wheel2 = initializeMesh("Wheel.3ds", GL_CCW, vec3(0, 0, 0), vec3(0, 0.3, 0.05), vec3(0.35, 0.35, 0.35));
	Turrent->frame.setPosition(vec3(0, 0, 0));

	Wheel3 = initializeMesh("Wheel.3ds", GL_CCW, vec3(0, 0, 0), vec3(0, 0.3, -0.6), vec3(0.35, 0.35, 0.35));
	Turrent->frame.setPosition(vec3(0, 0, 0));

	//particles
	initializeTexture("BH.bmp", &launchTextureHandle, true, true);
	launchEffect = new PsystemStruct();
	initializeParticle(launchEffect, 0, true, &launchTextureHandle);

	//terrain
	glEnableClientState(GL_VERTEX_ARRAY);
	ter = new terrain("HeightFeild_Data.txt", TERRAIN_WIDTH, TERRAIN_HEIGHT);
	//ter->setUp(TERRAIN_WIDTH, TERRAIN_HEIGHT, 10, 10);
	//initializeTexture("terrain.bmp", &terTextureHandle);

	cameraMode = true;
	playerControlled = camera;
	cout << endl << "Initialisation complete." << endl << endl;

	//lighting (calculated last to avoid stuttering effect from how movement calculated on player)
	cout << "  Creating light..." << endl;
	sun.setLightId(GL_LIGHT0);
	sun.setKind(sun.spotL);
	sun.setGlobalAmbient(0.201, 0.226, 0.255, 1.0);//src https://www.reddit.com/r/spaceengineers/comments/3e0k38/rgb_values_for_various_types_of_realworld_lights/
	sun.setAmbient(0.1, 0.1, 0.1, 1.0);
	sun.setDiffuse(0.7, 0.7, 0.7, 1.0);
	sun.setSpecular(0.7, 0.7, 0.7, 1.0);
	sun.setPos(6.0, 1.0, 2.0, 0.0); // Directional light (i.e. at infinity)
	sun.setScale(2, 2, 2);//large light source
	sun.setRadius(30);
}                         // End of initialise()

//event handlers
static void resizeWindow(int width, int height)
{
	GLfloat aspect;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	WIN_WIDTH = width; WIN_HEIGHT = height;
	aspect = (float)width / (float)height;
	gluPerspective(FOV, aspect, Near, Far);
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
}
void mouseHandler(int x, int y) {
	float originX = WIN_WIDTH / 2; float originY = WIN_HEIGHT / 2;

	//calculate this distance form origin
	x = -(x - originX);		y = -(y - originY);

	//calculate difference of last distance to the current
	float mouseX = x - oldmouseX; float mouseY = y - oldmouseY;

	//after comparing save this current position
	oldmouseX = x; oldmouseY = y;

	//cout << endl << " mousex  " << mouseX << " mousey  " << mouseY << endl;

	// mouseX = -1.0 + 2.0 * x / 320 ;
	//mouseY = 1.0 - 2.0 * y / 320 ;
	//angle = 90 + atan2(pos.y-mouseY, pos.x-mouseX) * 180 / 3.1415926;

	angleX = atan2f(mouseX, originX) * 120; angleY = atan2f(mouseY, originY) * 120;

	//cout << " anglex  " << angleX << " angley  " << angleY << endl;

	if (mouseX != 0 && isMoving) {
		TankBody->frame.rotateAboutV(radians(angleX));//left and right
		//TankBody->frame.setRotRadian(radians(angleX));

		Turrent->frame.rotateAboutV(radians(angleX));//left and right
		//Turrent->frame.setRotRadian(radians(angleX));

		Wheel1->frame.rotateAboutV(radians(angleX));//left and right
		//Wheel1->frame.setRotRadian(radians(angleX));

		Wheel2->frame.rotateAboutV(radians(angleX));//left and right
		//Wheel2->frame.setRotRadian(radians(angleX));

		Wheel3->frame.rotateAboutV(radians(angleX));//left and right
		//Wheel3->frame.setRotRadian(radians(angleX));

		//playerControlled->moveBy();
		//playerControlled->moveBy(upVec) (radians(angleX));
	}
	//if (mouseY != 0) {
	//	playerControlled->rotateAboutU(radians(angleY));// down and up
	//}
}
void mouseToWorld() {
}
void asciiKey(unsigned char key, int x, int y)
// Bind actions to some keys...
{
	static const vec3 backVec(0.0, 0.0, cameraStep);
	static const vec3 rightVec(cameraStep, 0.0, 0.0);
	static const vec3 upVec(0.0, cameraStep, 0.0);
	switch (key) {
	case 27:                 // ESC
	case 'w':    // object/camera moves forward
		//TankBody->setPosition(vec3(0, 0, playerPosition.z += WHEEL_SPEED));
		TankBody->frame.moveBy(backVec);
		Turrent->frame.moveBy(backVec);
		Wheel1->frame.moveBy(backVec);
		Wheel2->frame.moveBy(backVec);
		Wheel3->frame.moveBy(backVec);

		isMoving = true;
		//playerControlled->moveBy(-backVec);
		break;
	case 'D'://detatch camera or offset it
		playerControlled->ToggleAttatchement();
	case 't'://detatch camera or offset it
		TankBody->frame.moveBy(upVec);
		Turrent->frame.moveBy(upVec);
		Wheel1->frame.moveBy(upVec);
		Wheel3->frame.moveBy(upVec);
		Wheel2->frame.moveBy(upVec);

		isMoving = true;
	case 'g'://detatch camera or offset it
		TankBody->frame.moveBy(-upVec);
		Turrent->frame.moveBy(-upVec);
		Wheel1->frame.moveBy(-upVec);
		Wheel3->frame.moveBy(-upVec);
		Wheel2->frame.moveBy(-upVec);

		isMoving = true;
	case 'a':    // object/camera moves left
		//TankBody->setPosition(vec3(playerPosition.x += WHEEL_SPEED, 0, 0));
		TankBody->frame.moveBy(-rightVec);
		Turrent->frame.moveBy(-rightVec);
		Wheel1->frame.moveBy(-rightVec);
		Wheel3->frame.moveBy(-rightVec);
		Wheel2->frame.moveBy(-rightVec);

		isMoving = true;
		//playerControlled->moveBy(-rightVec);
		break;
	case 's':    // object/camera moves back
		//TankBody->setPosition(vec3(0, 0, playerPosition.z -= WHEEL_SPEED));
		TankBody->frame.moveBy(-backVec);
		Turrent->frame.moveBy(-backVec);
		Wheel1->frame.moveBy(-backVec);
		Wheel2->frame.moveBy(-backVec);
		Wheel3->frame.moveBy(-backVec);
		//spinAngle = -spinAngle;
		//spinAngleStep = -spinAngleStep;
		isMoving = true; isReverse = true;
		//playerControlled->moveBy(backVec);
		break;
	case 'd':    // object/camera moves right
		//TankBody->setPosition(vec3(playerPosition.x -= WHEEL_SPEED, 0, 0));
		TankBody->frame.moveBy(rightVec);
		Turrent->frame.moveBy(rightVec);
		Wheel1->frame.moveBy(rightVec);
		Wheel2->frame.moveBy(rightVec);
		Wheel3->frame.moveBy(rightVec);
		isMoving = true;
		//playerControlled->moveBy(rightVec);
		break;
	case 'q':    // object/camera moves up
		if (playerControlled == &TankBody->frame) {
			//TankBody->frame.moveBy(upVec);
			Turrent->frame.moveBy(upVec);
			Wheel1->frame.moveBy(upVec);
			Wheel2->frame.moveBy(upVec);
			Wheel3->frame.moveBy(upVec);
		}
		else {
			playerControlled->moveBy(upVec);
		}
		break;
	case 'e':    // object/camera moves down
		if (playerControlled == &TankBody->frame) {//if we are detatched from the lookat
			//TankBody->frame.moveBy(-upVec);
			Turrent->frame.moveBy(-upVec);
			Wheel1->frame.moveBy(-upVec);
			Wheel2->frame.moveBy(-upVec);
			Wheel3->frame.moveBy(-upVec);
		}
		else {
			playerControlled->moveBy(-upVec);
		}
		break;
	case 'j':    // object/camera rotates around a skew axis...
		playerControlled->rotateAxisAngle(cameraAngleStep, vec3(1.0, 0.3, 0));
		break;
	case ' ':   // Toggle object spinning
		spinning = !spinning;
		break;
	case 'R':   // Reverse spin direction
		spinAngleStep = -spinAngleStep;
		break;
	case 'W':   // Toggle wireframe
		wireFrame = !wireFrame;
		break;
	case 'z':
		exit(0);
	default:
		cout << "Unknown key (" << key << ")" << endl;
		break;
	}

	TankBody->frame.applyTransform();//apply on identity matrix of current stack
	Turrent->frame.applyTransform();
	Wheel1->frame.applyTransform();
	Wheel2->frame.applyTransform();
	Wheel3->frame.applyTransform();

	//smooth transition with deltatime
	//float dt = DELTA_TIME * 0.0003; (havent fixed it)
	TankBody->frame.setPosition(TankBody->frame.getPosition());
	Turrent->frame.setPosition(Turrent->frame.getPosition());
	Wheel1->frame.setPosition(Wheel1->frame.getPosition());
	Wheel2->frame.setPosition(Wheel2->frame.getPosition());
	Wheel3->frame.setPosition(Wheel3->frame.getPosition());
	glutPostRedisplay();
}
void specialKey(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_RIGHT:    // Camera/object pans right
		playerControlled->rotateAboutV(-cameraAngleStep);
		break;
	case GLUT_KEY_LEFT:    // Camera/object pans left
		playerControlled->rotateAboutV(cameraAngleStep);
		break;
	case GLUT_KEY_UP:    // Camera/object tilts up
		playerControlled->rotateAboutU(cameraAngleStep);
		break;
	case GLUT_KEY_DOWN:    // Camera/object tilts down
		playerControlled->rotateAboutU(-cameraAngleStep);
		break;
	case GLUT_KEY_PAGE_UP: // Camera/object spins anticlockwise
		playerControlled->rotateAboutN(cameraAngleStep);
		break;
	case GLUT_KEY_PAGE_DOWN: // Camera/object spins clockwise
		playerControlled->rotateAboutN(-cameraAngleStep);
		break;
	case GLUT_KEY_HOME: // Camera or object goes home
		if (playerControlled == camera) {
			playerControlled->goHome();
			playerControlled->setPosition(homePosition);

			TankBody->frame.goHome();
			TankBody->frame.setPosition(homePosition);

			Turrent->frame.goHome();
			Turrent->frame.setPosition(homePosition);

			Wheel1->frame.goHome();
			Wheel1->frame.setPosition(homePosition);

			Wheel2->frame.goHome();
			Wheel2->frame.setPosition(homePosition);

			Wheel3->frame.goHome();
			Wheel3->frame.setPosition(homePosition);
		}
	default:
		cout << "Unknown key (" << key << ")" << endl;
		break;
	}
	glutPostRedisplay();
}
void exitFunction(void)
{
	delete cube;
	delete axes;
	delete grid;

	delete TankBody;
	delete Turrent;

	delete Wheel1;
	delete Wheel2;
	delete Wheel3;

	delete launchEffect;

	delete Boulder1;
	delete Boulder2;
	delete Boulder3;
	delete Boulder4;
	delete Boulder5;
	delete Boulder6;
	delete Boulder7;
	delete Boulder8;

	delete BuildingS1;
	delete BuildingS2;
	delete BuildingS3;
	delete BuildingL1;
	delete BuildingL2;

	delete axes;
	delete grid;
	delete camera;
	//gluDeleteQuadric(quadric1);
	//  gluDeleteQuadric(quadric2);
}
void update(int value) {
	//_angle += 1.0f;
	//if (_angle > 360) {
	//	_angle -= 360;
	//}
	timeForNextFrame = true;
	//glutDisplayFunc(display);
	glutTimerFunc(timerMSecs, update, 0);
	glutPostRedisplay();
	timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
}
void mouseClickHandler(int button, int state, int x, int y) {
	cout << "btn " << button << "state " << state << "x " << x << "y " << y << "mouse clicked!" << endl;
	launchEffect->active = true;
	launchEffect->activeticks = 50;
}

//mainloop
int main(int argc, char * argv[])
{
	//init configuration
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//GL_STENCIL for shadows

	glutInitWindowSize(750, 750);
	glutInitWindowPosition(100, 75);
	glutCreateWindow("Aiden Faulconer VIC assignment");

	initialise();

	//opengl version printout
	OGL_VER = glGetString(GL_VERSION);
	cout << "OpenGL version: " << OGL_VER << endl;

	// Define GLUT callbacks...
	glutMouseFunc(mouseClickHandler);
	glutPassiveMotionFunc(mouseHandler);//mouse input
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(asciiKey);
	glutReshapeFunc(resizeWindow);
	glutSpecialFunc(specialKey);

	glutTimerFunc(timerMSecs, update, 0);//display in here

	glutMainLoop();
	return 0;
}