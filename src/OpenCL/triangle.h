#ifndef _TRIANGLE_H
#define	_TRIANGLE_H

#include "vertex.h"

typedef struct {
	Vertex		vertex[3];
	Vec3		normal;
	float		u, v;
	float		nu, nv, nd;
	int			k;
	float		bnu, bnv;
	float		cnu, cnv;
	int			materialIndex;
} Triangle;

/*static Triangle _Triangle(Vertex av1, Vertex av2, Vertex av3)
{
	Triangle t;
	t.materialIndex = 0;
	t.vertex[0] = av1;
	t.vertex[1] = av2;
	t.vertex[2] = av3;

	Vec3 A = t.vertex[0].position;
	Vec3 B = t.vertex[1].position;
	Vec3 C = t.vertex[2].position;

	// Calculate normal
	Vec3 c; vsub(c, B, A);
	Vec3 b; vsub(b, C, A);
	vxcross(t.normal, b, c);

	// Determine the dominant axis
	if(fabs(t.normal.x) > fabs(t.normal.y))
	{
		t.k = (fabs(t.normal.x) > fabs(t.normal.z)) ? 0 : 2;
	}
	else
	{
		t.k = (fabs(t.normal.y) > fabs(t.normal.z)) ? 1 : 2;
	}
	// Determine project plane axis
	t.u = (int) (t.k+1) % 3;
	t.v = (int) (t.k+2) % 3;

	// Precompute some values to speed up intersection checks
	// float krec = 1.0f / 
	//nu = m_N.cell[u] * krec;
	//nv = m_N.cell[v] * krec;
	//nd = m_N.Dot( A ) * krec;
	//// first line equation
	//real reci = 1.0f / (b.cell[u] * c.cell[v] - b.cell[v] * c.cell[u]);
	//bnu = b.cell[u] * reci;
	//bnv = -b.cell[v] * reci;
	//// second line equation
	//cnu = c.cell[v] * reci;
	//cnv = -c.cell[u] * reci;
	//// finalize normal
	//m_N.Normalize();
	//m_Vertex[0]->SetNormal( m_N );
	//m_Vertex[1]->SetNormal( m_N );
	//m_Vertex[2]->SetNormal( m_N );
	return t;

}*/

#endif