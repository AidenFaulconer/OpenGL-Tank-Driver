#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include "renderable.h"
#include "axes.h"
#include "OrthoFrame.h"

using namespace std;

typedef float vec2[2];

// Class for a basic polygon mesh.
// This version only contains the bare basics:
//    * number of vertices + the vertices themselves
//    * number of polygons + the polygons themselves
// It does not contain a material, nor any instructions
// for how to draw the polygons (wireframe vs. filled,
// culled or no, smooth vs. flat shading, ...)
// It also has no provision for storing texture coordinates,
// nor texturing information (e.g. texture handle to use).
// Note that the mesh can contain both:
//  * polygons, vertices and the like.
//  * a vector of ptr-to-Mesh, so that a scene graph
//    can be encoded.
// Most of the data is public (haven't been fanatical about
// set-and-get) but it would be unwise to change any of
// the fundamental geometry (vertices, normals, polygons,...)
// -- these should be read in / calculated once then
// left alone) .

class Mesh : public RenderableParent {
public:

	//Data...
	string  name;
	Axes*	meshAxes;
	int     nVertices;
	int     nPolygons;
	bool    isTextured;
	vec3*   vertices;
	vec3*   vertexNormals;
	vec2*   textureCoords;
	GLuint* polygons;
	vec3*   polygonNormals;
	GLenum vertexOrder = GL_CCW;
	vector<Mesh*> childMeshes;

	OrthoFrame frame;//all meshes have rotation handling (not amazingly memory efficient but this isnt battlefield 300 and 1

	vec3 position;  // position, in world coords
	float rotRadian; //rotation set currently for this mesh (in radians)

	//set methods
	/*void setPosition(const vec3& newPosition);
	void setRotRadian(const float& radian);*/

	//get methods
	/*vec3 getPosition();
	float getRotRadian();*/

	// Methods...
	Mesh(string meshName = "Unknown mesh");
	~Mesh();

	void setPolygonType(int nnSides);

	// Print mesh properties.
	// If recursive is true, then the whole DAG is printed.
	void printProperties();
	void printProperties(bool recursive);

	// Read a '.3ds' file and stow it into a Mesh that already exists.
	// Usually a 3ds file will create one 'parent' mesh with no polygons,
	// and one or more child meshes containing polygons.
	bool read3dsFile(char* fileName);

	int render();    // Render a Mesh.

	// Service methods for finding normals.
	// Usually findVertexNormals is enough -- it calls the other one if needed.
	bool findPolygonNormals();
	bool findVertexNormals();
	//void findBoundingBox();

protected:
	void drawPolygons();
	void printStuff(int j, bool recursive, int indent);

private:
	// Data...
	int     nSides;   // This should be 3 or 4
	GLuint polygonType; // This should be GL_TRIANGLES or GL_QUADS
};

// Define a cube as a special kind of polygon mesh.
// Just provide it with a special constructor.
class Cube : public Mesh
{
public:
	Cube(float size = 1.0f);
};

// Define a movable mesh as a special kind of polygon mesh.
// Basically it is a polygon mesh with
// an orthonormal frame attached to it.
class MoveableMesh : public Mesh
{
public:
	//Data...
	//GLenum vertexOrder = GL_CCW;

	// Update methods...
	void moveBy(const vec3& delta);

	MoveableMesh();
	int render();
protected:
	vec3 u;  // u vector, expressed in world coords
	vec3 v;  // v vector, expressed in world coords
	vec3 n;  // n vector, expressed in world coords
};

#endif  // MESH_H
