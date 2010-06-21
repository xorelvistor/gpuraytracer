#ifndef _BALL_H
#define	_BALL_H

#include "common.h"

typedef struct {
	Vec3	centre;
	float	sqRadius, radius, rRadius;
	int		materialIndex;
} Ball;

static Ball _Ball(Vec3 c, float r)
{
	Ball b;
	b.centre = c;
	b.radius = r;
	b.sqRadius = r*r;
	b.rRadius = (1.0f/r);

	return b;
}

#endif