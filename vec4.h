#ifndef _VEC4m_
#define _VEC4m_

#include <math.h>

#include "vec3.h"

#define pi 3.14159265359
#define degrees pi/180.0
#define radians 180.0/pi

/* Type definitions...  */
typedef float vec2[2];
typedef float vec4[4];

// -----------  Vec3 print  ----------------------------------------
// Print a vec3.
// If u is the vec3 {0.0400, 1.0000, 0.6800} , then the call
//   v3Print(u, "6.4f", "Displacement vec"
// should print something like
//   Displacement vec:  [0.0400, 1.0000, 0.6800]
void v3Print(const vec3 u, char fformat[], char stringLabel[]);

// -----------  Vec3 creation  -------------------------------------
// Build a vector u from 3 scalars...
void v3FloatToVec(const float a, const float b, const float c, vec3& u);
void v3FloatToVecM(vec3& u, const float a, const float b, const float c);

// Set u to zero vector...
void v3Zero(vec3& u);

// Copy a vector: effectively  v = u...
void v3Copy(const vec3& u, vec3& v);
void v3CopyM(vec3& v, const vec3& u);

// -----------  Arithmetic Operations - In place  ------------------

// Change sign of vec: effectively  u = -u...
void v3ChangeSign(vec3& u);

// In-place scalar mult: effectively  u = a*u...
void v3Scale(const float a, vec3& u);

// In-place addition: effectively  v = v + u...
void v3AddToVec(const vec3& u, vec3& v);
void v3AddToVecM(vec3& v, const vec3& u);

// In-place subtraction: effectively  v = v - u...
void v3SubtractFromVec(const vec3& u, vec3& v);

// In-place vec3 min: effectively  v = min(u,v)...
void v3UpdateMin(const vec3& u, vec3& v);

// In-place vec3 max: effectively  v = max(u,v)...
void v3UpdateMax(const vec3& u, vec3& v);

// -----------  Routines beolow Defined for Softwear engineering purposes -----------------------------

#define X 0
#define Y 1
#define Z 2
#define W 3

#define R 0
#define G 1
#define B 2
#define A 3

// returns true if any element of u is less than its corresponding element in v
bool v3isLess(const vec3& u, const vec3& v);

// returns true if any element of u is greater than its corresponding element in v
bool v3isGreater(const vec3& u, const vec3& v);

// clips values in u to a maximum of v
void v3clipMaxM(vec3& u, const vec3& v);

// clips values in u to a minimum of v
void v3clipMinM(vec3& u, const vec3& v);

// clips absoloute values in u to a minimum/maximum of v
void v3clipAbsM(vec3& u, const vec3& v);

// damp movement move u towards zero by amount v
void v3dampM(vec3& u, const vec3& v);

// ------------ worse still vec4 stuff here ----------------

// set a vec 4 to a value
void v4FloatToVecM(vec4 u, const float a, const float b, const float c, const float d);

// adds u into v
void v4AddToVecM(vec4 v, const vec4 u);

// clips values in u to a maximum of v
void v4ClipMaxM(vec4 u, const vec4 v);

// copy vector
void v4CopyM(vec4 v, const vec4 u);

#endif  // _VEC3m_
