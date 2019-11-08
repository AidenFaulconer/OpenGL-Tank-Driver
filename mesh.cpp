#include <iostream>
#include "lib3ds.h"
#include "mesh.h"

// Construct an empty mesh with the given name
Mesh::Mesh(string meshName) : RenderableParent()
{
	name = meshName;
	nVertices = 0;
	nPolygons = 0;
	isTextured = false;
	setPolygonType(3);  // Default to triangles.
	vertices = NULL;
	textureCoords = NULL;
	vertexNormals = NULL;
	polygons = NULL;
	polygonNormals = NULL;
}

Mesh::~Mesh()
{
	// A destructor is more complicated than it looks.
	// It is possible & desirable that more than one mesh
	// point at the same polygon data, and that more than one
	// parent mesh point at the same child mesh.
	// Hence a destructor cannot just blithely delete
	// the memory pointed at in the mesh -- if it does then
	// it may leave dangling pointers in other meshes.
	// One way to handle this is to implement
	// reference counting in mesh constructors & destructors.
	// We just duck the issue... effectively this class
	// assumes a static collection of meshes that are created,
	// populated and drawn -- BUT WHICH NEVER NEED TO BE DESTROYED.
	// If you don't destroy them, then there is no memory leaking.
	// However, if the destructor is called, then it is safe (?) to
	// delete the memory holding normals, since that is allocated
	// by findVertexNormals(); and should be 'local' to this mesh.
	delete[] vertexNormals;
	delete[] polygonNormals;
}

void Mesh::setPolygonType(int nnSides)
{
	switch (nnSides)
	{
	case 3:
		nSides = 3;
		polygonType = GL_TRIANGLES;
		break;
	case 4:
		nSides = 4;
		polygonType = GL_QUADS;
		break;
	default:
		return;  // only know about triangles and quads.
	}
}

void Mesh::printStuff(int i, bool recursive, int indent)
{
	string leader(indent, ' ');
	// Print summary of Mesh object properties.
	cout << leader << "Mesh[" << i << "]  '" << name << "'" << endl;
	cout << leader << "  Vertices:   " << nVertices << endl;
	cout << leader << "  Polygons:   " << nPolygons << endl;
	cout << leader << "  Sides:      " << nSides << endl;
	//findBoundingBox();
	cout << leader << "  Sub-meshes: " << childMeshes.size() << endl;
	if (!recursive)
		return;
	for (GLuint j = 0; j < childMeshes.size(); j++)
		childMeshes[j]->printStuff(j, recursive, indent + 2);
	return;
}

void Mesh::printProperties(bool recursive)
{
	printStuff(0, recursive, 0);
}

void Mesh::printProperties()
{
	printStuff(0, false, 0);
}

bool Mesh::read3dsFile(char *fileName)
{
	// Read a .3ds file, extract mesh data from it.
	Lib3dsFile*     file;
	Lib3dsMesh*     mesh3d;
	Lib3dsMaterial* material = NULL;
	Mesh*           mesh;

	file = lib3ds_file_open(fileName);
	if (file == NULL)
		return false;

	// Not sure what the next does, but I *think* it constructs tfm matrices
	// for the 'nodes' in the 3ds files for frame 0 of any animation that
	// might be in the file?
	lib3ds_file_eval(file, 0);

	// OK, got the 3ds file, so start poking the data into Mesh...
	name = file->name;
	setPolygonType(3); // .3ds files always have triangles.
	// Copy the mesh data...
	for (int i = 0; i < file->nmeshes; i++)
	{
		mesh3d = file->meshes[i];
		mesh = new Mesh();
		childMeshes.push_back(mesh);

		// Copy 3ds vertex data for child mesh...
		mesh->nVertices = mesh3d->nvertices;
		mesh->vertices = new vec3[mesh->nVertices];
		for (int j = 0; j < mesh->nVertices; j++)
			mesh->vertices[j] = vec3(mesh3d->vertices[j]); // ptr-to-float constructor

			  // Copy 3ds texture coord data (if available) for child mesh
		if (mesh3d->texcos != NULL)
		{
			mesh->isTextured = true;
			mesh->textureCoords = new vec2[mesh->nVertices];
			for (int j = 0; j < mesh->nVertices; j++)
			{
				mesh->textureCoords[j][0] = mesh3d->texcos[j][0];
				mesh->textureCoords[j][1] = mesh3d->texcos[j][1];
			}
		}

		// Copy 3ds polygon data for child mesh...
		mesh->nPolygons = mesh3d->nfaces;
		mesh->polygons = new GLuint[3 * mesh->nPolygons];  // Because they're triangles.
		GLuint* polygons = mesh->polygons;               // convenient short name.
		Lib3dsFace* faces = mesh3d->faces;                // convenient short name.
		for (int j = 0; j < mesh->nPolygons; j++)
		{
			polygons[3 * j] = faces[j].index[0];
			polygons[3 * j + 1] = faces[j].index[1];
			polygons[3 * j + 2] = faces[j].index[2];
		}
		mesh->findPolygonNormals();
		mesh->findVertexNormals();
	}
	lib3ds_file_free(file);
	return true;
}

bool Mesh::findPolygonNormals()
{
	// Skip it if there are no polygons,
	// or if the normals have already been calculated.
	if (nPolygons == 0 || polygonNormals != NULL)
		return false;
	polygonNormals = new vec3[nPolygons];
	for (int j = 0; j < nPolygons; j++)
	{
		// Use first 3 polygon vertices to find a polygon normal...
		GLuint v0 = polygons[j*nSides + 0];
		GLuint v1 = polygons[j*nSides + 1];
		GLuint v2 = polygons[j*nSides + 2];
		polygonNormals[j] = polygonNormal(vertices[v0], vertices[v1], vertices[v2]);
	}
	return true;
}

bool Mesh::findVertexNormals()
{
	findPolygonNormals();
	// Skip it if there are no vertices,
	// or if vertex normals appear to have already been calculated...
	if (nVertices == 0 || vertexNormals != NULL)
		return false;
	vertexNormals = new vec3[nVertices]; // These should be initialised to [0,0,0]
	// Loop over polygons, accumulating normals
	// at each vertex of the poly...
	for (int i = 0; i < nPolygons; i++)
	{
		for (int k = 0; k < nSides; k++)
		{
			GLuint vNum = polygons[i*nSides + k];
			vertexNormals[vNum] += polygonNormals[i];
		}
	}
	for (int j = 0; j < nVertices; j++)
		vertexNormals[j] = normalize(vertexNormals[j]);
	return true;
}

int Mesh::render()
{
	int rv, returnValue = 0;
	//glPushAttrib(GL_ALL_ATTRIB_BITS);  // Push the GL attribute stack
	if (isTextured)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	doTransform();
	if (nVertices != 0)
		drawPolygons();
	for (unsigned int j = 0; j < childMeshes.size(); j++)
	{
		rv = childMeshes[j]->render();
		returnValue = returnValue || rv;
	}
	glPopMatrix();
	//glPopAttrib();
	return returnValue;
}

void Mesh::drawPolygons()
{
	// Draw a polygon mesh object.

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	if (vertexNormals != NULL)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, vertexNormals);
	}
	if (isTextured && textureCoords != NULL)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, textureCoords);
	}
	glDrawElements(polygonType, nSides*nPolygons,
		GL_UNSIGNED_INT, polygons);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

//vec3 vmin(vec3 a, vec3 b)
//{
//  vec3 m(a);
//  if (b.x<a.x)
//    m.x = b.x;
//  if (b.y<a.y)
//    m.y = b.y;
//  if (b.z<a.z)
//    m.z = b.z;
//  return m;
//}
//
//vec3 vmax(vec3 a, vec3 b)
//{
//  vec3 m(a);
//  if (b.x>a.x)
//    m.x = b.x;
//  if (b.y>a.y)
//    m.y = b.y;
//  if (b.z>a.z)
//    m.z = b.z;
//  return m;
//}
//
//void Mesh::findBoundingBox()
//{
//  vec3 lower(1000000), upper(-1000000);
//
//  if (nVertices == 0)
//    return;
//  for (int j=0; j<nVertices; j++)
//  {
//     lower = vmin(lower, vertices[j]);
//     upper = vmax(upper, vertices[j]);
//  }
//  cout << "Bounding box:" << endl;
//  cout << "  x: [ " << lower.x << ", " << upper.x << " ]" << endl;
//  cout << "  y: [ " << lower.y << ", " << upper.y << " ]" << endl;
//  cout << "  z: [ " << lower.z << ", " << upper.z << " ]" << endl;
//  cout << endl;
//}

// Define Cube constructor.
// This is a foolishly complicated way to draw a cube,
// but means it can serve as a simple Mesh "test object".

Cube::Cube(float size) : Mesh("Cube")
{
	const float sz = 0.5;

	nVertices = 8;
	nPolygons = 6;
	setPolygonType(4); // Cube is made of quads.

	setScale(size, size, size);

	vertices = new vec3[nVertices];
	polygons = new GLuint[4 * nPolygons];

	// Here is the cube data, arranged in a way suitable for
	// vertex array drawing.  It's barely different to before.
	vertices[0] = vec3(-sz, sz, -sz);
	vertices[1] = vec3(sz, sz, -sz);
	vertices[2] = vec3(sz, -sz, -sz);
	vertices[3] = vec3(-sz, -sz, -sz);
	vertices[4] = vec3(-sz, sz, sz);
	vertices[5] = vec3(sz, sz, sz);
	vertices[6] = vec3(sz, -sz, sz);
	vertices[7] = vec3(-sz, -sz, sz);

	// Build C array -- easy to initialise...
	GLuint cubePolygonSequence[] = {
	  4,  7,  6,  5,
	  1,  5,  6,  2,
	  0,  3,  7,  4,
	  0,  4,  5,  1,
	  2,  6,  7,  3,
	  0,  1,  2,  3
	};
	// Copy the data across...
	for (int j = 0; j < 4 * nPolygons; j++)
		polygons[j] = cubePolygonSequence[j];

	findVertexNormals();  // This should average the 3 face normals at each vertex.
}

MoveableMesh::MoveableMesh() : Mesh()
{
}

//void Mesh::setRotRadian(const float& radian) {
//	rotRadian = radian;
//}
//
//
//float Mesh::getRotRadian() {
//	return rotRadian;
//}

void MoveableMesh::moveBy(const vec3& delta) {
	// Translation interpreted as expressed in camera coords...
	position += delta.x * u + delta.y * v + delta.z * n;
}

int MoveableMesh::render()
{
	int rv, returnValue = 0;
	//glPushAttrib(GL_ALL_ATTRIB_BITS);  // Push the GL attribute stack
	if (isTextured)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glFrontFace(MoveableMesh::vertexOrder);
	frame.applyTransform();
	doTransform();
	if (nVertices != 0)
		drawPolygons();
	for (unsigned int j = 0; j < childMeshes.size(); j++)
	{
		rv = childMeshes[j]->render();
		returnValue = returnValue || rv;
	}
	glPopMatrix();
	//glPopAttrib();
	return returnValue;
}