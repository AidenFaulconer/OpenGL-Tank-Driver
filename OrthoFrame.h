#ifndef ORTHOFRAME_H
#define ORTHOFRAME_H

#include "vec3.h"

// Manage the configuration state of a Euclidean frame,
// i.e. object whose configuration is given by a position
// and an orientation.
// Position means in terms of world coordinates (x,y,z).
// The frame here is orthonormal [u,v,n], expressing
// the frame's local axes in terms of world coordinates [x,y,z].

class OrthoFrame {
public:
	OrthoFrame(const vec3& pos = vec3(0.0f, 0.0f, 0.0f));
	OrthoFrame(const vec3& pos, const vec3& uu, const vec3& vv, const vec3& nn);
	//~OrthoFrame();  // Default destructor is okay, no dynamic data.

	// Set methods...
	virtual void goHome();

	void ToggleAttatchement();

	void setPosition(const vec3& newPosition);//also sets previous position
	void setLookAtPosition(const vec3& newPosition);

	void setOrientation(const vec3& newU, const vec3& newV, const vec3& newN);

	// Get methods...
	vec3 getPosition();
	vec3 getPreviousPosition();//used for angle calculations
	vec3 getLookAtPosition();
	vec3 getU();
	vec3 getV();
	virtual float *getMatrix(float matrix[16]);
	virtual float *getInverseMatrix(float matrix[16]);

	// Update methods...
	void moveBy(const vec3& delta);
	void rotateAboutU(const float dTheta);  // rotate about u
	void rotateAboutV(const float dTheta);  // rotate about v
	void rotateAboutN(const float dTheta);  // rotate about n
	void rotateAxisAngle(const float dTheta, const vec3& axis);  // rotate about axis

	// Methods with graphics side effects...
	void applyTransform();
	void cameraTransform();

protected:
	// Data...
	bool detatched = false;
	vec3 position;  // position, in world coords
	vec3 previousPosition;//used for movement calculations
	vec3 u;  // u vector, expressed in world coords
	vec3 v;  // v vector, expressed in world coords
	vec3 n;  // n vector, expressed in world coords
};

#endif // ORTHOFRAME_H