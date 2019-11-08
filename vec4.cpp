#include "vec4.h"



// -----------  Vec3 print  ----------------------------------------
void v3Print(const vec3 u, char fformat[], char stringLabel[])
// Print vec3 something like "Displacement vec: [0.0400, 1.0000, 0.6800]" 
// according to given floating point format
{
  char s[200];
  sprintf_s(s, "%s:  [%%%s, %%%s, %%%s]\n", stringLabel, fformat, fformat, fformat);
  printf(s, u[0], u[1], u[2]);
}

// -----------  Vec3 creation  -------------------------------------

void v3FloatToVec(const float a, const float b, const float c, vec3& u)
// Build a vector u from 3 scalars...
{
  u.x=a; 
  u.y=b; 
  u.z=c;
}

void v3FloatToVecM( vec3& u, const float a, const float b, const float c)
// Build a vector u from 3 scalars...
{
  u.x=a; 
  u.y=b; 
  u.z=c;
}

void v3Zero(vec3& u)    
// Effectively:  u = 0
{
  u.x=0.0; 
  u.y=0.0; 
  u.z=0.0;
}

void v3Copy(const vec3& u, vec3& v)
// Effectively: v = u
{
  v.x = u.x; 
  v.y = u.y; 
  v.y = u.y;
}

void v3CopyM(vec3& v, const vec3& u)
// Effectively: v = u
{
  v.x = u.x; 
  v.y = u.y; 
  v.z = u.z;
}


// -----------  Arithmetic Operations - In place  ------------------

void v3ChangeSign (vec3& u)
// Change sign of vec: effectively  u = -u
{
  u.x = -u.x;
  u.y = -u.y;
  u.z = -u.z;
}

void v3Scale (const float a, vec3& u)
// In-place scalar mult: effectively  u = a*u
{
  u.x *= a;
  u.y *= a;
  u.z *= a;
}

void v3AddToVec(const vec3& u, vec3& v)
// In-place addition: effectively  v = v + u;
{
  v.x += u.x; 
  v.y += u.y; 
  v.z += u.z;
}

void v3AddToVecM(vec3& v, const vec3& u)
// In-place addition: effectively  v = v + u;
{
  v.x += u.x; 
  v.y += u.y; 
  v.z += u.z;
}

void v3SubtractFromVec (const vec3& u, vec3& v)
// In-place subtraction: effectively  v = v - u;
{
  v.x -= u.x; 
  v.y -= u.y; 
  v.z -= u.z;
}

void v3UpdateMin(const vec3& u, vec3& v)
// In-place vec3 min: effectively  v = min(u,v)
{
   if (u.x<v.x)  v.x = u.x;
   if (u.y<v.y)  v.y = u.y;
   if (u.z<v.z)  v.z = u.z;
}

void v3UpdateMax(const vec3& u, vec3& v)
// In-place vec3 max: effectively  v = max(u,v)
{
   if (u.x>v.x)  v.x = u.x;
   if (u.y>v.y)  v.y = u.y;
   if (u.z>v.z)  v.z = u.z;
}

// -----------  Routines beolow Defined for Softwear engineering purposes -----------------------------

// returns true if any element of u is less than its corresponding element in v 
bool v3isLess(const vec3& u, const vec3& v)
{
	if (u.x < v.x) return true;
	if (u.y < v.y) return true;
	if (u.z < v.z) return true;
return false;
}

// returns true if any element of u is greater than its corresponding element in v 
bool v3isGreater(const vec3& u, const vec3& v)
{
	if (u.x > v.x) return true;
	if (u.y > v.y) return true;
	if (u.z > v.z) return true;
return false;
}

// clips values in u to a maximum of v
void v3clipMaxM(vec3& u, const vec3& v)
{	
	if (u.x > v.x) u.x = v.x;
	if (u.y > v.y) u.y = v.y;
	if (u.z > v.z) u.z = v.z;
}

// clips values in u to a minimum of v
void v3clipMinM(vec3& u, const vec3& v)
{
if (u.x < v.x) u.x = v.x;
if (u.y < v.y) u.y = v.y;
if (u.z < v.z) u.z = v.z;
}

// clips absoloute values in u to a minimum/maximum of v
// note it assumes v is positive for maximum speed
void v3clipAbsM(vec3& u, const vec3& v)
{
	if (u.x > v.x) u.x = v.x;
	if (u.y > v.y) u.y = v.y;
	if (u.z > v.z) u.z = v.z;

	if (u.x < -v.x) u.x = -v.x;
	if (u.y < -v.y) u.y = -v.y;
	if (u.z < -v.z) u.z = -v.z;
}

// damp movement move u towards zero by amount v 
void v3dampM(vec3& u, const vec3& v)
{
	if (u.x < 0) u.x = u.x + v.x;
	if (u.y < 0) u.y = u.y + v.y;
	if (u.z < 0) u.z = u.z + v.z;

	if (u.x > 0) u.x = u.x - v.x;
	if (u.y > 0) u.y = u.y - v.y;
	if (u.z > 0) u.z = u.z - v.z;
}

// -------------- vec4 stuff below ------------------------------------

void v4FloatToVecM(vec4 u,const float a, const float b, const float c, const float d)
{
	u[0]=a;
	u[1]=b;
	u[2]=c;
	u[3]=d;
}

void v4AddToVecM(vec4 v, const vec4 u)
{
  v[0] = v[0]+ u[0]; 
  v[1] = v[1]+ u[1]; 
  v[2] = v[2]+ u[2]; 
  v[3] = v[3]+ u[3];
}

//clips u to v
void v4ClipMaxM(vec4 u, const vec4 v)
{
	if (u[0] > v[0]) u[0] = v[0];
	if (u[1] > v[1]) u[1] = v[1];
	if (u[2] > v[2]) u[2] = v[2];
	if (u[3] > v[3]) u[3] = v[3];
}

void v4CopyM(vec4 v, const vec4 u)
// Effectively: v = u
{
  v[0] = u[0]; 
  v[1] = u[1]; 
  v[2] = u[2];
  v[3] = u[3];
}
// end
