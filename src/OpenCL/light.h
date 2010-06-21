#ifndef _LIGHT_H
#define	_LIGHT_H

#include "common.h"

#define POINTLIGHTRADIUS 0.05f

#define POINTLIGHT 1
#define AREALIGHT 2

typedef struct {
	Vec3	position;
	RGB		colour;
	int		type;
} Light;

static Light _Light(Vec3 p, RGB r, int t)
{
	Light l;
	l.position = p;
	l.colour = r;
	l.type = t;
	
	return l;
}

#endif