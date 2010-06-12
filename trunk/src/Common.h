#pragma once

#include <cmath>
#include <math.h>
#include <iostream>
#include <SDL.h>
#include <glew.h>

#include "Vector3.h"

//*****************************************************************************
// Definitions
//*****************************************************************************
#define DOT(A,B)		(A.x*B.x+A.y*B.y+A.z*B.z)
#define NORMALIZE(A)	{float l=1/sqrtf(A.x*A.x+A.y*A.y+A.z*A.z);A.x*=l;A.y*=l;A.z*=l;}
#define LENGTH(A)		(sqrtf(A.x*A.x+A.y*A.y+A.z*A.z))
#define SQRLENGTH(A)	(A.x*A.x+A.y*A.y+A.z*A.z)
#define SQRDISTANCE(A,B) ((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y)+(A.z-B.z)*(A.z-B.z))

#define EPSILON			0.0001f
#define MAXTRACEDEPTH		6

#define PI				3.141592653589793238462f

//*****************************************************************************
// Enumerators
//*****************************************************************************
enum TracerType
{
	CPUONLY,
	OPENCLCPU,
	OPENCLGPU
};

enum ScreenPlane
{
	xLeft,
	xRight,
	yTop,
	yBottom
};

enum PrimitiveType
{
	SPHERE,
	PLANE,
	POLYGON
};

enum IntersectionType
{
	HIT,
	MISS,
	INSIDE
};

//*****************************************************************************
// STRUCTURES
//*****************************************************************************

struct Ray
{
	Vector3 position, direction;
	Ray() : position(0, 0, 0), direction(0, 0, 0) {}
	Ray(Vector3 p, Vector3 d) : position(p), direction(d) {}
};


typedef Vector3 Colour;