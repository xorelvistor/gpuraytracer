#ifndef _VEC3_H
#define	_VEC3_H

typedef struct {
	float x, y, z;
} Vec3;

#define vinit(v, a, b, c) { (v).x = a; (v).y = b; (v).z = c; }
#define vassign(a, b) vinit(a, (b).x, (b).y, (b).z)
#define vclr(v) vinit(v, 0.f, 0.f, 0.f)
#define vadd(v, a, b) vinit(v, (a).x + (b).x, (a).y + (b).y, (a).z + (b).z)
#define vsub(v, a, b) vinit(v, (a).x - (b).x, (a).y - (b).y, (a).z - (b).z)
#define vsadd(v, a, b) { float k = (a); vinit(v, (b).x + k, (b).y + k, (b).z + k) }
#define vssub(v, a, b) { float k = (a); vinit(v, (b).x - k, (b).y - k, (b).z - k) }
#define vmul(v, a, b) vinit(v, (a).x * (b).x, (a).y * (b).y, (a).z * (b).z)
#define vsmul(v, a, b) { float k = (a); vinit(v, k * (b).x, k * (b).y, k * (b).z) }
#define vdot(a, b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)
#define vnorm(v) { float l = 1.f / sqrt(vdot(v, v)); vsmul(v, l, v); }
#define vxcross(v, a, b) vinit(v, (a).y * (b).z - (a).z * (b).y, (a).z * (b).x - (a).x * (b).z, (a).x * (b).y - (a).y * (b).x)
#define vfilter(v) ((v).x > (v).y && (v).x > (v).z ? (v).x : (v).y > (v).z ? (v).y : (v).z)
#define viszero(v) (((v).x == 0.f) && ((v).x == 0.f) && ((v).z == 0.f))

#define clamp(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
#define vmax(x, y) ( (x) > (y) ? (x) : (y))
#define vmin(x, y) ( (x) < (y) ? (x) : (y))
#define sign(x) ((x) > 0 ? 1 : -1)

#define toInt(x) ((int)(pow(clamp(x, 0.f, 1.f), 1.f / 2.2f) * 255.f + .5f))

static Vec3 _Vec3(float x, float y, float z)
{
	Vec3 v;
	v.x = x;
	v.y = y;
	v.z = z;

	return v;
}

#endif

