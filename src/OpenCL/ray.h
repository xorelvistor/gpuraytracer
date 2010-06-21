#ifndef _RAY_H
#define	_RAY_H

#include "common.h"

typedef struct {
	Vec3		position, direction;
} Ray;

#define rayInit(r, p, d) { vassign((r).position, p); vassign((r).direction, d); }

static Ray _Ray(Vec3 p, Vec3 d)
{
	Ray r;
	r.position = p;
	r.direction = d;

	return r;
}
#endif

