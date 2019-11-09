// terrain.cpp

#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "terrain.h"
#include "fileOperations.h"

// -----------------------------------------------------------------------------------

// X22_0112.TXT

//GLfloat xsize;
//GLfloat zsize;
//int xsteps;
//int zsteps;

//MaterialClass* mat;
//MaterialClass* matLines;

void terrain::setUp(GLfloat xsizeZ, GLfloat zsizeZ, int xstepsZ, int zstepsZ)
{
	xsize = xsizeZ;
	zsize = zsizeZ;
	xsteps = xstepsZ;
	zsteps = zstepsZ;
}

void terrain::setMaterialHeight(GLfloat height)
{
	//	MaterialClass* matHigh;
	//MaterialClass* matBeach;
	//MaterialClass* matLand;

	//GLfloat highHeight;
	//GLfloat grassHeight;
	if (height >= highHeight)
		matHigh->doMaterial();
	else
	{
		if (height > grassHeight)
			matLand->doMaterial();
		else
			matBeach->doMaterial();
	}
}

//read in height field data
void readHeightFieldData(string fileName) {
	char str[999];
	char str2[999];
	char *tok;
	char *cont;
	char *strr;
	FILE* file;
	int count = 0;
	errno_t rc = fopen_s(&file, fileName.c_str(), "r");
	//file = fopen( "testmap.txt", "r");

	if (rc == 0)
	{
		strr = fgets(str, 998, file);
		while (strr != NULL)
		{
			printf("%s\n", str);
			strcpy_s(str2, 999, str);
			tok = strtok_s(str2, ",", &cont);
			while (tok != NULL)
			{
				printf("token=%s\n", tok);
				tok = strtok_s(NULL, ",", &cont);
			}
			strr = fgets(str, 998, file);
			count++;
		}

		fclose(file);
	}
}

int terrain::render()
{
	glPushMatrix();

	GLfloat xdelta = xsize / xsteps;
	GLfloat zdelta = zsize / zsteps;

	for (int x = 0; x < xsteps - 1; x++)
		for (int z = 0; z < zsteps - 1; z++)
		{
			//set points for this triangle
			glBegin(GL_TRIANGLES);
			glNormal3fv(polygonNormal(vec3(xdelta*x, map[x][z], zdelta*z), vec3(xdelta*x + xdelta, map[x + 1][z], zdelta*z), vec3(xdelta*x, map[x][z + 1], zdelta*z + zdelta)).fv());
			setMaterialHeight(map[x][z]);

			//thisTex->setVertex(z, xdelta*x, map[x][z], zdelta*z);
			glVertex3f(xdelta*x, map[x][z], zdelta*z);

			setMaterialHeight(map[x + 1][z]);

			//thisTex->setVertex(z, xdelta*x, map[x][z], zdelta*z);
			glVertex3f(xdelta*x + xdelta, map[x + 1][z], zdelta*z);
			setMaterialHeight(map[x][z + 1]);

			//thisTex->setVertex(z, xdelta*x, map[x][z], zdelta*z);
			glVertex3f(xdelta*x, map[x][z + 1], zdelta*z + zdelta);

			glEnd();

			//set face for this triangle
			glBegin(GL_TRIANGLES);
			glNormal3fv(polygonNormal(vec3(xdelta*x + xdelta, map[x + 1][z + 1], zdelta*z + zdelta), vec3(xdelta*x, map[x][z + 1], zdelta*z + zdelta), vec3(xdelta*x + xdelta, map[x + 1][z], zdelta*z)).fv());
			setMaterialHeight(map[x + 1][z + 1]);

			//thisTex->setVertex(z, xdelta*x, map[x][z], zdelta*z);
			glVertex3f(xdelta*x + xdelta, map[x + 1][z + 1], zdelta*z + zdelta);
			setMaterialHeight(map[x][z + 1]);

			//thisTex->setVertex(z, xdelta*x, map[x][z], zdelta*z);
			glVertex3f(xdelta*x, map[x][z + 1], zdelta*z + zdelta);
			setMaterialHeight(map[x + 1][z]);

			//thisTex->setVertex(z, xdelta*x, map[x][z], zdelta*z);
			glVertex3f(xdelta*x + xdelta, map[x + 1][z], zdelta*z);

			glEnd();
		}

	//mat->setMaterialBlue();
	//mat->doMaterial();
	//for (int x=0; x<xsteps-1; x++)
	//	for (int z=0; z<zsteps-1; z++)
	//	{
	//	glVertex3f(xdelta*x,0.1,zdelta*z);
	//	glVertex3f(xdelta*x+xdelta,0.1,zdelta*z);
	//	glVertex3f(xdelta*x,0.1,zdelta*z+zdelta);
	//
	//	glVertex3f(xdelta*x+xdelta,0.1,zdelta*z+zdelta);
	//	glVertex3f(xdelta*x,0.1,zdelta*z+zdelta);
	//	glVertex3f(xdelta*x+xdelta,0.1,zdelta*z);
	//	}

	glPopMatrix();
	return true;
}

int terrain::renderLines()
{
	glPushMatrix();

	matLines->doMaterial();

	GLfloat xdelta = xsize / xsteps;
	GLfloat zdelta = zsize / zsteps;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//debug normals
	for (int x = 0; x < xsteps - 1; x++)
		for (int z = 0; z < zsteps - 1; z++)
		{
			displayNormal3(
				vec3(xdelta*x + xdelta, map[x + 1][z + 1], zdelta*z + zdelta),
				vec3(xdelta*x, map[x][z + 1], zdelta*z + zdelta),
				vec3(xdelta*x + xdelta, map[x + 1][z], zdelta*z),
				vec3(x, map[x][z], z)
			);
		}

	glBegin(GL_TRIANGLES);
	for (int x = 0; x < xsteps - 1; x++)
		for (int z = 0; z < zsteps - 1; z++)
		{
			glNormal3f(x, map[x][z], z); // yes veronica all normals point up (which is wrong) (students need to fix this)

			glVertex3f(xdelta*x, 0.01 + map[x][z], zdelta*z);
			glVertex3f(xdelta*x + xdelta, 0.01 + map[x + 1][z], zdelta*z);
			glVertex3f(xdelta*x, 0.01 + map[x][z + 1], zdelta*z + zdelta);

			glVertex3f(xdelta*x + xdelta, 0.01 + map[x + 1][z + 1], zdelta*z + zdelta);
			glVertex3f(xdelta*x, 0.01 + map[x][z + 1], zdelta*z + zdelta);
			glVertex3f(xdelta*x + xdelta, 0.01 + map[x + 1][z], zdelta*z);
		}

	glEnd();

	glPopMatrix();

	return true;
}

//was in fileoperations, however it makes more sense for it to be here imo
int terrain::readHeightField(const char *filename, int *width, int *depth, int heights[][MAXDEPTH])
// readHeightField reads in height field data from a file
// return value is 0 if successful, 1 if fail.
// Width, depth & heights are returned as a call-by-reference.
#define giveUp(message) {fclose(inputFile); printf("%s\n", message); return GL_FALSE;}
{
	FILE   *inputFile;
	int    nRead, i, j;

	// Open height field data file for input, barf on error...
	printf("Opening height field file \"%s\" for input...\n", filename);
	inputFile = fopen(filename, "r");
	if (inputFile == NULL)
	{
		printf("  Unable to open file for input\n");
		return GL_FALSE;
	}

	// Read first line, it should be width & depth...
	nRead = fscanf(inputFile, "%d %d", width, depth);
	if (nRead != 2)
		giveUp("  Couldn't read width & height from file.");
	printf("  width, depth:  %d x %d\n", *width, *depth);
	if (*width <= 0 || *depth <= 0)
		giveUp("  Invalid value for width or depth..");

	// Now loop over lines containing height data...
	printf("  Reading heights...\n");
	for (j = 0; j < *depth; j++)
	{
		for (i = 0; i < *width; i++)
		{
			nRead = fscanf_s(inputFile, "%d", &heights[i][j]);
			if (nRead != 1)
				giveUp("  Bad or incomplete height data.");
		}
	}
	fclose(inputFile);
	printf("  Height field done.\n");
	return GL_TRUE;
}                         // End of readHeightField

terrain::terrain(char* fileName, int width, int height)
{
	mat = new MaterialClass();
	matLines = new MaterialClass();
	mat->setMaterialJade();
	matLines->setMaterialBlackRubber();

	matHigh = new MaterialClass();
	matBeach = new MaterialClass();
	matLand = new MaterialClass();

	matHigh->setMaterialWhiteStuff();
	matBeach->setMaterialYellowSand();
	matLand->setMaterialGreenGrass();

	mapWidth = width;
	mapHeight = height;

	setUp(mapWidth, mapHeight, mapWidth, mapHeight);
	readHeightField(findFile(fileName), &mapWidth, &mapHeight, mapInt);
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			map[x][y] = ((float)mapInt[x][y]) / 200;
		}
	}


	highHeight = 2.8;
	grassHeight = 0.5;
}

float terrain::getHeight(const vec3& pos) {
	if (pos.x < 0 || pos.z < 0)
		return 0;//if we get a location outside the heightfeild return nothing
	else
		return map[int(pos.x)][int(pos.z)];
}

terrain::~terrain()
{
	delete mat;
	delete matLines;
	delete matBeach;
	delete matHigh;
	delete matLand;
	delete thisTex;
}

// end