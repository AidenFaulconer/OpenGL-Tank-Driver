#ifndef VEC3_H
#define VEC3_H
#include <cmath>
#include <iostream>
#include "glut.h"
using namespace std;

/* ======  Declaration of vec3 class ==============================*/
struct vec3
{
	// The vec3 struct is designed to be stored internally as just x,y,z values.

public:
	// Data.  Warning! these are public!!
	GLfloat x;
	GLfloat y;
	GLfloat z;

	/* ----- Constructors & destructors... -----------------*/
	vec3();	// 0-argument constructor makes a zero-vector.
	vec3(const GLfloat a); // 1-argument constructor makes vector [a,a,a]
	vec3(const GLfloat xval, const GLfloat yval, const GLfloat zval); // 3-argument constructor copies x,y,z values into a vec3...
	vec3(GLfloat const *pf); // 1-arg constructor transcribes data from an array of GLfloats
	// No copy constructor: default is okay.
	// No destructor: default is okay.

	// No set and get methods, since data are public.
	inline GLfloat* fv(); // Method to return a ptr-to-GLfloat.

	/* ------ Other methods... ----------------------- */
	vec3 setZero();
	vec3 setValue(GLfloat xx, GLfloat yy, GLfloat zz);

	// Overload C++ operators for some basic arithmetic...
	GLfloat operator[](const int k) const;
	vec3 operator+=(const vec3& v);
	vec3 operator+=(const GLfloat a);
	vec3 operator-=(const vec3& v);
	vec3 operator-=(const GLfloat a);
	vec3 operator*=(const GLfloat a);
	vec3 operator/=(const GLfloat a);
};  // End of vec3 class

// =========== Top-level function declarations =============
// ----------- C++ operators ---------------------
inline ostream& operator<<(ostream& os, const vec3& v);
bool operator==(const vec3& u, const vec3& v);
bool operator!=(const vec3& u, const vec3& v);
inline vec3 operator-(const vec3& u);
inline vec3 operator+(const vec3& u, const vec3& v);
inline vec3 operator-(const vec3& u, const vec3& v);
inline vec3 operator*(const GLfloat a, const vec3& u);
inline vec3 operator*(const vec3& u, const GLfloat a);
inline vec3 operator/(const vec3& u, const GLfloat a);

// -------- Other functions using vec3 ------------------------
inline GLfloat length(const vec3 &u);
inline vec3 normalize(const vec3& u);
inline vec3& normalize(vec3& w, vec3 &u);

inline GLfloat dist(const vec3& u, const vec3& v);
inline vec3 mix(const vec3& u, const vec3& v, const GLfloat t);
inline vec3&  mix(vec3& w, vec3& u, vec3& v, const GLfloat t);
inline GLfloat  dot(const vec3& u, const vec3& v);
inline vec3 cross(const vec3& u, const vec3& v);
inline vec3& cross(vec3& w, vec3& u, vec3& v);

inline vec3 polygonNormal(const vec3& P, const vec3& Q, const vec3& R);
inline vec3& polygonNormal(vec3& n, const vec3& P, const vec3& Q, const vec3& R);

inline vec3& negate(vec3& w, vec3& u);
inline vec3& add(vec3& w, vec3& u, vec3& v);
inline vec3& subtract(vec3& w, vec3& u, vec3& v);
inline vec3& scalarMult(vec3 &w, GLfloat a, vec3& u);
/* =========== Methods for vec3 struct ======================= */
// 0-argument constructor makes vector [0,0,0]...
inline vec3::vec3() : x(0.0f), y(0.0f), z(0.0f)
{
}

// 1-argument constructor makes vector [a,a,a]...
inline vec3::vec3(const GLfloat a) : x(a), y(a), z(a)
{
}

// 3-argument constructor copies x,y,z values into a vec3...
inline vec3::vec3(const GLfloat xval, const GLfloat yval, const GLfloat zval) : x(xval), y(yval), z(zval)
{
}

// 1-argument constructor -- accepts ptr-to-GLfloat
// and copies array components to a vec3
inline vec3::vec3(GLfloat const *pf) : x(pf[0]), y(pf[1]), z(pf[2])
{
}

// This dangerous method provides a pointer-to-GLfloat that can be passed
// when OpenGL expects an array of 3 elements.
// E.g. if "vec3 u = vec3(0.7, 0.2, 0.2);"
// then it is valid to call   "glColor3fv(u.fv());"
// "Dangerous" because a pointer to an object is escaping.
// If you delete the underlying vec3 or it goes out of scope,
// then you know what you're in for if you follow this dangling pointer.
inline GLfloat* vec3::fv()
{
	return (GLfloat*)this;
}

// Method to set a vector to [0,0,0]...
inline vec3 vec3::setZero()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	return *this;
}

// Method to set a vector to [xx,yy,zz]...
inline vec3 vec3::setValue(GLfloat xx, GLfloat yy, GLfloat zz)
{
	x = xx;
	y = yy;
	z = zz;
	return *this;
}

// C++ operators...
inline GLfloat vec3::operator[](const int k) const
{
	return ((GLfloat*)this)[k];
}

inline vec3 vec3::operator+=(const vec3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline vec3 vec3::operator+=(const GLfloat a)
{
	x += a;
	y += a;
	z += a;
	return *this;
}

inline vec3 vec3::operator-=(const vec3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline vec3 vec3::operator-=(const GLfloat a)
{
	x -= a;
	y -= a;
	z -= a;
	return *this;
}

inline vec3 vec3::operator*=(const GLfloat a)
{
	x *= a;
	y *= a;
	z *= a;
	return *this;
}

inline vec3 vec3::operator/=(const GLfloat a)
{
	GLfloat invA = 1.0f / a;
	x *= invA;
	y *= invA;
	z *= invA;
	return *this;
}

// ========= End of methods for struct vec3 ==================

// ======= Top-level function definitions ===========

// ----------- C++ operators ---------------------

//  Stream output...
inline ostream& operator<<(ostream& os, const vec3& v)
{
	os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
	return os;
}

// Comparison...
// Beware! (GLfloating point comparisons)
inline bool operator==(const vec3& u, const vec3& v)
{
	return (u.x == v.x && u.y == v.y && u.z == v.z);
}

// Comparison...
// Beware! (GLfloating point comparisons)
inline bool operator!=(const vec3& u, const vec3& v)
{
	return (u.x != v.x || u.y != v.y || u.z != v.z);
}

// Vector negation...
// Warning! vec3 '-' allocates memory! Use 'negate' if this matters.
inline vec3 operator-(const vec3& u)
{
	return vec3(-u.x, -u.y, -u.z);
}

// Vector addition...
// Warning! vec3 '+' allocates memory!  Use 'add' if this matters.
inline vec3 operator+(const vec3& u, const vec3& v)
{
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

// Vector subtraction...
// Warning! vec3 '-' allocates memory!  Use 'subtract' if this matters.
inline vec3 operator-(const vec3& u, const vec3& v)
{
	return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

// Scalar mult a*u.
// Warning: vec3 '*' allocates memory!  Use 'scalarMult' if this matters.
inline vec3 operator*(const GLfloat a, const vec3& u)
{
	return vec3(a * u.x, a * u.y, a * u.z);
}

// Scalar mult u*a.
// Warning: vec3 '*' allocates memory!  Use 'scalarMult' if this matters.
inline vec3 operator*(const vec3& u, const GLfloat a)
{
	return vec3(a * u.x, a * u.y, a * u.z);
}

// Scalar 'division' u/a, which is really u * (1/a).
// Warning: vec3 '*' allocates memory!  Use 'scalarMult' if this matters.
inline vec3 operator/(const vec3& u, const GLfloat a)
{
	GLfloat invA = 1.0f / a;
	return vec3(invA * u.x, invA * u.y, invA * u.z);
}

// -------- Other functions using vec3 ------------------------

// Euclidean length of vector...
inline GLfloat length(const vec3 &u)
{
	// This is not overflow-safe, nor is it underflow-safe
	// So don't let x,y,z get too big, or too small.
	return sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
}

// Normalization of a vector...
// Warning!  allocates memory
inline vec3 normalize(const vec3& u)
{
	GLfloat len, invLength;
	len = length(u);
	if (len == 0.0)  // Can't normalize a 0-vector, but don't throw an exception.
		return vec3(0.0f, 0.0f, 0.0f);
	invLength = 1.0f / len;  // Only need one division.
	return vec3(u.x*invLength, u.y*invLength, u.z*invLength);
}

// Two-argument form, no memory allocated...
inline vec3& normalize(vec3& w, vec3 &u)
{
	GLfloat len, invLength;
	len = length(u);
	if (len == 0.0)  // Can't normalize a 0-vector, but don't throw an exception.
	{
		w.x = 0.0;
		w.y = 0.0;
		w.z = 0.0;
		return w;
	}
	// OK, the vector is nonzero...
	invLength = 1.0f / len;  // Only need one division.
	w.x = invLength * u.x;
	w.y = invLength * u.y;
	w.z = invLength * u.z;
	return w;
}

// Euclidean distance between points
// This should be called distance, but that fights with some C++ iterator function?
inline GLfloat dist(const vec3& u, const vec3& v)
{
	GLfloat deltax = v.x - u.x,
		deltay = v.y - u.y,
		deltaz = v.z - u.z;
	return sqrt(deltax * deltax + deltay * deltay + deltaz * deltaz);
}

// Linear blend or mix of vectors.
// Warning!  Allocates memory!  Use 4-argument form if this matters.
inline vec3 mix(const vec3& u, const vec3& v, const GLfloat t)  // Linear blend of u,v
{
	return t * u + (1 - t)*v;
}

// Linear blend or mix of vectors.  Effectively w = t*u + (1-t)*v
inline vec3&  mix(vec3& w, vec3& u, vec3& v, const GLfloat t)  // Linear blend of u,v
{
	w.x = t * u.x + (1 - t)*v.x;
	w.y = t * u.y + (1 - t)*v.y;
	w.z = t * u.z + (1 - t)*v.z;
	return w;
}

inline GLfloat  dot(const vec3& u, const vec3& v) // Dot product of two vectors. will be zero if vectors are orthogonal
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

// Cross product of two vectors...
inline vec3 cross(const vec3& u, const vec3& v)
{
	return vec3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline vec3& cross(vec3& w, vec3& u, vec3& v)
{
	// This code is carefully written to work for calls like cross (u,u,v)
	// Why not just use the other version?
	GLfloat tmpx, tmpy, tmpz;
	tmpx = u.y * v.z - u.z * v.y;
	tmpy = u.z * v.x - u.x * v.z;
	tmpz = u.x * v.y - u.y * v.x;
	w.x = tmpx;
	w.y = tmpy;
	w.z = tmpz;
	return w;
}

//find angle between two vectors
inline GLfloat findAngle(const vec3& v1, const vec3& v2)
{
	// You might leave these Unitize calls out if you know
	// you're always passing in Unitized vectors. However
	// I recommend having two versions of this function,
	// one that takes unit vectors, and one that will unitize
	// them for you.
	// Copies are required because we're passing in 'v1'
	// and 'v2' by const reference, which allows the
	// user to pass in vectors and not worry about them
	// unintentionally getting unitized.

	//vec3 v1Copy = v1;
	//v1Copy.Unitize();

	//vec3 v2Copy = v2;
	//v2Copy.Unitize();

	//dotProduct / magProduct is cos theta... arc cos of theta is the angle
	const GLfloat dotProduct = fabsf(dot(v1, v2));
	const GLfloat magProduct = length(v1) * length(v2);// |a|*|b|

	cout << "mag product" << magProduct << endl;
	cout << "dot product " << dotProduct << endl;
	cout << "angle is... " << acosf(dotProduct / magProduct) << endl;

	// A range check here is a good idea for safe code, I'll
	// leave it out for this sample though.

	return acosf(dotProduct / magProduct);
}

// Unit normal of a polygon...
// Warning! 'polygonNormal' allocates memory!
inline vec3 polygonNormal(const vec3& P, const vec3& Q, const vec3& R)
{
	vec3 PQ = Q - P, PR = R - P, n = cross(PQ, PR);
	return normalize(n);
}

// Unit normal of a polygon...
// Version using less memory allocation.
inline vec3& polygonNormal(vec3& n, const vec3& P, const vec3& Q, const vec3& R)
{
	vec3 PQ = Q - P, PR = R - P;
	cross(n, PQ, PR);
	normalize(n, n);
	return n;
}

// Unary minus that doesn't allocate memory...
inline vec3& negate(vec3& w, vec3& u)
{
	w.x = -u.x;
	w.y = -u.y;
	w.z = -u.z;
	return w;
}

// Vector addition that doesn't allocate memory...
inline vec3& add(vec3& w, vec3& u, vec3& v)  // Can't declare const e.g. what if add(u,u,u)?
{
	// Effectively w = u + v
	w.x = u.x + v.x;
	w.y = u.y + v.y;
	w.z = u.z + v.z;
	return w;
}

// Vector subtraction that doesn't allocate memory...
inline vec3& subtract(vec3& w, vec3& u, vec3& v) // Can't declare const e.g. what if subtract(u,u,u)?
{
	// Effectively w = u - v
	w.x = u.x - v.x;
	w.y = u.y - v.y;
	w.z = u.z - v.z;
	return w;
}

// Scalar mult that doesn't allocate memory...
inline vec3& scalarMult(vec3 &w, GLfloat a, vec3& u) // Can't declare const e.g. what if scalarMult(u,a,u)?
{
	// Effectively v = a * u
	w.x = a * u.x;
	w.y = a * u.y;
	w.z = a * u.z;
	return w;
}

#endif  // VEC3_H