#include "OrthoFrame.h"
#define GLUT_DISABLE_ATEXIT_HACK
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>

//for handling rotations and the camera
OrthoFrame::OrthoFrame(const vec3& pos)
{
	goHome();
	position = pos;
}

OrthoFrame::OrthoFrame(const vec3& pos, const vec3& uu, const vec3& vv, const vec3& nn)
{
	// This constructor trusts caller to provide a right-handed
	// orthonormal frame in [uu, vv, nn]
	position = pos;
	u = uu;
	v = vv;
	n = nn;
}

//~OrthoFrame();  // Default destructor is okay, no dynamic data.

// Set methods...
void OrthoFrame::goHome()
{
	position.setZero();
	u = vec3(1.0, 0.0, 0.0);
	v = vec3(0.0, 1.0, 0.0);
	n = vec3(0.0, 0.0, 1.0);
}

vec3 OrthoFrame::getPreviousPosition() {
	return previousPosition;
}

void OrthoFrame::setPosition(const vec3& newPosition)
{
	previousPosition = position - 2;//now the current position is a previous one 3 places backwards...
	position = newPosition;
}

void OrthoFrame::setOrientation(const vec3& newU, const vec3& newV, const vec3& newN)
{
	// This set method trusts caller to provide a right-handed
	// orthonormal frame [U, V, N]
	u = newU;
	v = newV;
	n = newN;
}

// Get methods...
vec3 OrthoFrame::getPosition()
{
	return position;
}

vec3 OrthoFrame::getU()
{
	return u;
}

vec3 OrthoFrame::getV()
{
	return v;
}

vec3 OrthoFrame::getLookAtPosition()
{
	return n;
}

float* OrthoFrame::getMatrix(float matrix[16])
{
	// Populate 4x4 camera transformation matrix
	// Let R = [u v n] be rotation
	// and let M = [R a]
	//             [0 1]  be affine tfm x' = R*x + a
	// This takes us from camera coords to world.
	// This is the opposite of what's needed in the pipeline,
	// but may be needed sometimes.
	// Return value is the input pointer.
	matrix[0] = u.x;
	matrix[1] = u.y;
	matrix[2] = u.z;
	matrix[3] = 0.0;
	matrix[4] = v.x;
	matrix[5] = v.y;
	matrix[6] = v.z;
	matrix[7] = 0.0;
	matrix[8] = n.x;
	matrix[9] = n.y;
	matrix[10] = n.z;
	matrix[11] = 0.0;
	matrix[12] = position.x;
	matrix[13] = position.y;
	matrix[14] = position.z;
	matrix[15] = 1.0;
	return matrix;
}

float* OrthoFrame::getInverseMatrix(float matrix[16])
{
	// Populate 4x4 camera transformation matrix
	// Let R = [u v n] be rotation
	// and let M = [R a]
	//             [0 1]  be affine tfm x' = R*x + a
	// But this takes us from camera coords to world.
	// We want the inverse of this (world to camera).
	// This is [R^T  -R^T*a]
	//         [ 0      1  ]
	// where R^T = [ u^T ]
	//             [ v^T ]
	//             [ n^T ]
	// and -R^T*a = [-dot(u,a)]
	//              [-dot(v,a)]
	//              [-dot(n,a)]
	// Return value is the input pointer.
	matrix[0] = u.x;
	matrix[1] = v.x;
	matrix[2] = n.x;
	matrix[3] = 0.0;
	matrix[4] = u.y;
	matrix[5] = v.y;
	matrix[6] = n.y;
	matrix[7] = 0.0;
	matrix[8] = u.z;
	matrix[9] = v.z;
	matrix[10] = n.z;
	matrix[11] = 0.0;
	matrix[12] = -dot(u, position);
	matrix[13] = -dot(v, position);
	matrix[14] = -dot(n, position);
	matrix[15] = 1.0;
	return matrix;
}

void OrthoFrame::ToggleAttatchement() {
	detatched = !detatched;
}

void OrthoFrame::applyTransform()
{
	float matrix[16];
	if (detatched) {//isolate transformation independent of worlds
		glPushMatrix();
		glMultMatrixf(getMatrix(matrix));
		glPopMatrix();
	}
	glMultMatrixf(getMatrix(matrix));//causes stuttering
}

void OrthoFrame::setLookAtPosition(const vec3& newPosition) {
	n = newPosition;
}

void OrthoFrame::cameraTransform()
{
	vec3 lookAtPoint(n);  // n points back, so camera looks forward
	vec3 newpos = lookAtPoint + position;//we must be a consistent distance from lookat
	gluLookAt(newpos.x, newpos.y, newpos.z,
		lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
		v.x, v.y, v.z);
}

// Update methods...
void OrthoFrame::moveBy(const vec3& delta)
{
	// Translation interpreted as expressed in camera coords...
	position += delta.x * u + delta.y * v + delta.z * n;
}

void OrthoFrame::rotateAboutV(const float dTheta)
{
	// rotate about v
	float c = cos(dTheta), s = sin(dTheta);
	vec3 nPrime(c*n + s * u);
	u = -s * n + c * u;
	n = nPrime;
}

void OrthoFrame::rotateAboutU(const float dTheta)
{
	// rotate about u
	float c = cos(dTheta), s = sin(dTheta);
	vec3 vPrime(c*v + s * n);
	n = -s * v + c * n;
	v = vPrime;
}

void OrthoFrame::rotateAboutN(const float dTheta)
{
	// rotate about n
	float c = cos(dTheta), s = sin(dTheta);
	vec3 uPrime(c*u + s * v);
	v = -s * u + c * v;
	u = uPrime;
}

void OrthoFrame::rotateAxisAngle(const float dTheta, const vec3& axis)
{
	// Rogrigues formula for axis-angle rotation.
	// This is
	// R = cos(dTheta) * I + (1-cos(dTheta)) * a * a^T + sin(dTheta) * [ a X ]
	// where [a X ] is the "a cross" skew-symmetric matrix
	// such that [ a X ][w] = a X w (cross product).
	float rot[3][3];
	float c = cos(dTheta), s = sin(dTheta);
	vec3 a(normalize(axis));   // Now a is a *unit* vector for the axis.
	// Initialise to cos(dTheta) * (IdentityMatrix)...
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			rot[i][j] = 0.0f;
		rot[i][i] = c;
	}
	// Now add in the (1-cos(dTheta)) * a * a^T
	float oneMinusC = 1.0f - c;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			rot[i][j] += oneMinusC * a[i] * a[j];
	}
	// Now rotMatrix = cos(dTheta) * I + (1-cos(dTheta)) * a * a^T
	// That's the symmetric part.
	// Now add in the skew-symmetric part;
	// IS THIS THE RIGHT WAY ROUND?  MAY NEED TO REVERSE SIGNS
	vec3 b(s*a);  // Now b contains sin(dTheta) * unit axis
	rot[0][1] -= b[2];
	rot[1][0] += b[2];
	rot[0][2] += b[1];
	rot[2][0] -= b[1];
	rot[1][2] -= b[0];
	rot[2][1] += b[0];

	// Now transform u, v, n...
	// This is [ u' v' n' ] = [ u v n] [ R00  R01  R02]
	//                                 [ R10  R11  R12]
	//                                 [ R20  R21  R22]

	vec3 newU = rot[0][0] * u + rot[1][0] * v + rot[2][0] * n;
	vec3 newV = rot[0][1] * u + rot[1][1] * v + rot[2][1] * n;
	vec3 newN = rot[0][2] * u + rot[1][2] * v + rot[2][2] * n;

	u = newU;
	v = newV;
	n = newN;
}