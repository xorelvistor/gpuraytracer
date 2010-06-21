#ifndef _MATERIAL_H
#define	_MATERIAL_H

#include "common.h"

typedef struct {
	int		materialIndex;
	float	diffuse;
	float	reflection;
	float	specular;
	float	refraction;
	float	refractiveIndex;
	RGB		colour;
} Mat;

static Mat _Mat(RGB r, int i)
{
	Mat m;
	m.materialIndex = i;
	m.colour = r;
	m.reflection = 0;
	m.refraction = 0;
	m.refractiveIndex = 1.0f;
	m.diffuse = 0.2f;

	return m;
}

#endif