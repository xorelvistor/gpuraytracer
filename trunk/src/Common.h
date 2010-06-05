#pragma once

#include <cmath>
#include <math.h>
#include <iostream>
#include <SDL.h>
#include <glew.h>

#include "Vector3.h"

//*****************************************************************************
// Enumerators
//*****************************************************************************
enum TracerType
{
	CPUONLY,
	OPENCLCPU,
	OPENCLGPU
};

enum Material
{
	mDIFFUSE,
	mREFLECTIVE,
	mTRANSPARENT,
	mTRANSLUCENT,
	mLIGHT
};

enum ScreenPlane
{
	xLeft,
	xRight,
	yTop,
	yBottom
};

//*****************************************************************************
// STRUCTURES
//*****************************************************************************
struct Colour
{
	GLuint red, green, blue;
	Colour():red(0), green(0), blue(0) {}
	Colour(GLuint r, GLuint g, GLuint b):red(r), green(g), blue(b) {}
};

struct Ray
{
	Vector3 pos, dir;
	Ray(Vector3 p, Vector3 d):pos(p),dir(d) {}
};

