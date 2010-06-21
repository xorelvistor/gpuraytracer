#ifndef _VERTEX_H
#define	_VERTEX_H

#include "common.h"

typedef struct {
	Vec3	position;
	Vec3	normal;
	float	u, v; // UV co-ords for texture mapping
} Vertex ;

#endif