#ifndef _COMMON_H
#define	_COMMON_H

#include "vec3.h"

#define vDOT(A,B)		(A.x*B.x+A.y*B.y+A.z*B.z)
#define vNORMALIZE(A)	{float l=1/sqrt(A.x*A.x+A.y*A.y+A.z*A.z);A.x*=l;A.y*=l;A.z*=l;}
#define vLENGTH(A)		(sqrt(A.x*A.x+A.y*A.y+A.z*A.z))
#define vSQRLENGTH(A)	(A.x*A.x+A.y*A.y+A.z*A.z)
#define vSQRDISTANCE(A,B) ((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y)+(A.z-B.z)*(A.z-B.z))

typedef Vec3 RGB;

static RGB _RGBf(float r, float g, float b)
{
	RGB c;
	c.x = r;
	c.y = g;
	c.z = b;

	return c;
}

static RGB _RGBi(int r, int g, int b)
{
	RGB c;
	c.x = ((float) r) / 255;
	c.y = ((float) g) / 255;
	c.z = ((float) b) / 255;
	
	return c;
}

static RGB _RGBv(Vec3 v)
{
	RGB c;
	c.x = v.x;
	c.y = v.y;
	c.z = v.z;

	return c;
}

#endif