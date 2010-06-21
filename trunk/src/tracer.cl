// #pragma OPENCL EXTENSION cl_amd_printf : enable
// #pragma OPENCL EXTENSION cl_amd_fp64 : enable

#include "ball.h"
#include "light.h"
#include "triangle.h"
#include "cam.h"
#include "material.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#define EPSILON			0.0001f
#define INITIALDEPTH	1
#define MAXTRACEDEPTH	6
#define PI				3.141592653589793238462f
#define vDOT(A,B)		(A.x*B.x+A.y*B.y+A.z*B.z)
#define ABSORPTION		0.15f

#define INSIDE			-1
#define MISS			0
#define HIT				1

#define HITNOTHING		0
#define HITBALL			1
#define HITLIGHT		2
#define HITTRIANGLE		3

#define SQR(x)			((x)*(x))

////////////////////////////////////////////////////////////////////////////////////////////////////

#define float3			float4
#define _float3(x,y,z)	((float4){(x),(y),(z),(0.0f)})
#define _float3v(v)		((float4){(v).x,(v).y,(v).z,(0.0f)})

float3  cross3(float3  b, float3  c)
{
	return _float3(mad(b.y, c.z,  -b.z * c.y),
				   mad(b.z, c.x,  -b.x * c.z),
				   mad(b.x, c.y,  -b.y * c.x));

}

float3 normalize3(float3 v)
{
	return v * half_rsqrt(dot(v, v));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
	float3 position, direction;
} Ray;

Ray _Ray (float3 p, float3 d){Ray r; r.position = p; r.direction = d; return r;}

////////////////////////////////////////////////////////////////////////////////////////////////////

static float3 getBallNormal(float3 intersectPoint, __global Ball* ball)
{
	return ((intersectPoint - _float3v(ball->centre)) * ball->rRadius);
}

static float3 getTriangleNormal(float3 intersectPoint, __global Triangle* triangle)
{
	return _float3(0.0f, 0.0f, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static int getIntersectionType(float det, float b, float* aDistance)
{
	int retValue = MISS;
	if ( det > 0 )
	{
		det = half_sqrt( det );
		float i1 = b - det;
		float i2 = b + det;
		if ( i2 > 0 )
		{
			if ( i1 < 0 )
			{
				if ( i2 < *aDistance )
				{
					*aDistance = i2;
					retValue = INSIDE;
				}
			}
			else
			{
				if ( i1 < *aDistance )
				{
					*aDistance = i1;
					retValue = HIT;
				}
			}
		}
	}

	return retValue;
}

static int intersectPointLight(Ray *aRay, float* aDistance, __global Light* light)
{
	float3 v = aRay->position - _float3v(light->position);
	float b = -dot(v, aRay->direction);
	float det = (b*b) -dot(v, v) + SQR(POINTLIGHTRADIUS);
	return getIntersectionType(det, b, aDistance);
}

static int intersectBall(Ray *aRay, float* aDistance, __global Ball* ball)
{
	float3 v = aRay->position - _float3v(ball->centre);
	float b = -dot(v, aRay->direction);
	float det = (b*b) -dot(v, v) + ball->sqRadius;
	return getIntersectionType(det, b, aDistance);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static __global Mat* getMaterial(int mIndex, __global Mat* materials, const int materialCount)
{
	for(int m = 0; m < materialCount; m++)
	{
		if(materials[m].materialIndex == mIndex)
		{
			return &materials[m];
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static float calculateShade(__global Light* light,
							float3 intersectPoint,
							__global Ball* balls,
							const int ballCount,
							__global Triangle* triangles,
							const int triangleCount)
{
	float shade = 1.0f;

	// Point Lights
	if(light->type == POINTLIGHT)
	{
		float3 L;
		L = light->position - intersectPoint;
		float distanceFromLight = vLENGTH(L);
		L = (1.0f/distanceFromLight) * L;

		float3 pos;
		pos = intersectPoint + (L * EPSILON);

		Ray r = _Ray(pos, L);
		for(int b = 0; b < ballCount; b++)
		{
			__global Ball* ball = &balls[b];
			if(intersectBall(&r, &distanceFromLight, ball) != MISS)
			{
				shade = 0;
				break;
			}
		}
	}


	return shade;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static void rayTrace(Ray* aRay,
					 float3* aColour,
					 unsigned int depth,
					 float* aDistance,
					 float aRefractiveIndex,
				     __global Ball* balls,
				     const int ballCount,
				     __global Light* lights,
				     const int lightCount,
				     __global Triangle* triangles,
				     const int triangleCount,
					 __global Mat* materials,
					 const int materialCount)
{
	// Don't recurse too far
	if (depth > MAXTRACEDEPTH) return;

	// Initiate
	*aDistance = 1000000.0f;
	float3 intersectPoint;
	int intersectionType = HITNOTHING;
	int intersectionResult = MISS;
	__global Light* lastLight = 0;
	__global Ball* lastBall = 0;
	__global Triangle* lastTriangle = 0;


	// Find Nearest Sphere Intersection
	for (int b = 0; b < ballCount; b++)
	{
		__global Ball* ball = &balls[b];
		int r = intersectBall(aRay, aDistance, ball);
		if ( r != MISS )
		{
			intersectionType = HITBALL;
			lastBall = ball;
			intersectionResult = r;
		}
	}
	// Find Nearest Light Intersection
	for (int l = 0; l < lightCount; l++)
	{
		__global Light* light = &lights[l];
		if ( light->type == POINTLIGHT )
		{
			int r = intersectPointLight(aRay, aDistance, light);
			if ( r != MISS )
			{
				intersectionType = HITLIGHT;
				lastLight = light;
				intersectionResult = r;
			}
		}
	}

	// No hit, terminate ray
	if (intersectionType == HITNOTHING) return;

	// Hit light
	if(intersectionType == HITLIGHT)
	{
		*aColour = _float3v(lastLight->colour);
	}
	else
	{
		// Calculate intersection point
		intersectPoint = aRay->position + aRay->direction * *(aDistance);

		float3 N;
		__global Mat* material;
		if(intersectionType == HITBALL)
		{
			N = getBallNormal(intersectPoint, lastBall);
			material = getMaterial(lastBall->materialIndex, materials, materialCount);
		}
		else
		{
			N = getTriangleNormal(intersectPoint, lastTriangle);
			material = getMaterial(lastTriangle->materialIndex, materials, materialCount);
		}
		// *aColour = _float3v(material->colour);
		//*aColour = N;

		for(int l = 0; l < lightCount; l++)
		{
			__global Light* light = &lights[l];

			//----------------------------------------
			// SHADOWS
			//----------------------------------------
			float shade = calculateShade(light, intersectPoint, balls, ballCount, triangles, triangleCount);

			if (shade > 0)
			{
				float3 L = _float3v(light->position) - intersectPoint;
				L = normalize3(L);

				//----------------------------------------
				// DIFFUSE SHADING
				//----------------------------------------
				if (material->diffuse > 0)
				{
					float dot = vDOT(L, N);
					if (dot > 0)
					{
						float diffuse = dot * material->diffuse * shade;
						*aColour += diffuse * _float3v(material->colour) * _float3v(light->colour);
					}
				}

				//----------------------------------------
				// SPECULAR SHADING
				//----------------------------------------
				if (material->specular > 0)
				{
					float3 V = aRay->direction;
					float3 R = L - 2.0f * vDOT(L,N) * N;
					float dot = vDOT(V,R);
					if (dot > 0)
					{
						float specular = pow(dot, 20) * material->specular * shade;
						*aColour += specular * _float3v(light->colour);
					}
				}
			}
		}
		//----------------------------------------
		// REFLECTION
		//----------------------------------------
		float reflection  = material->reflection;
		if ( reflection > 0.0f && depth < MAXTRACEDEPTH )
		{
			float3 R = aRay->direction -2.0f * vDOT(aRay->direction, N) * N;
			float3 reflectionColour = _float3(0.0f, 0.0f, 0.0f);
			float dist;
			Ray ray = _Ray(intersectPoint + R * EPSILON, R);
			rayTrace(&ray, &reflectionColour, depth + 1, &dist, aRefractiveIndex, balls, ballCount, lights, lightCount, triangles, triangleCount, materials, materialCount);
			*aColour += reflection * reflectionColour * _float3v(material->colour);
		}

		//----------------------------------------
		// REFRACTION
		//----------------------------------------
		float refraction = prim->getMaterial()->getRefraction();
		if (refraction > 0 && depth < MAXTRACEDEPTH)
		{
			float refractionIndex = prim->getMaterial()->getRefractiveIndex();
			float refractionRatio = aRefractionIndex / refractionIndex;
			float3 N = prim->getNormal(intersectionPoint) * (float) intersectionResult;
			float cosI = -DOT(N, aRay.direction);
			float cosT2 = 1.0f - refractionRatio * refractionRatio * (1.0f - cosI * cosI);
			if(cosT2 > 0.0f)
			{
				float3 T = (refractionRatio * aRay.direction) + (refractionRatio * cosI - sqrtf(cosT2)) * N;
				Colour refractionColour(0, 0, 0);
				float refractionDistance;
				trace(Ray(intersectionPoint + T * EPSILON, T), refractionColour, depth + 1, refractionIndex, refractionDistance);

				//***********************************
				// Beer-Lambert Law
				//***********************************
				Colour absorbance = prim->getMaterial()->getColour() * prim->getMaterial()->getAbsorptionCoefficient() * -refractionDistance;
				Colour transparency = Colour( expf(absorbance.red),
											  expf(absorbance.green),
											  expf(absorbance.blue) );
				*aColour += refractionColour * transparency;
			}
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static void generatePrimaryRay(__global Cam* camera,
							   const int x,
							   const int y,
							   const int screenWidth,
							   const int screenHeight,
							   Ray *ray)
{

	float3 origin = _float3(camera->position.x, camera->position.y, camera->position.z);
	float left = camera->spXLeft;
	float right = camera->spXRight;
	float top = camera->spYTop;
	float bottom = camera->spYBottom;

	float dX = (right - left) / screenWidth;
	float dY = (top - bottom) / screenHeight;

	float3 spPos = _float3(left + x * dX, bottom + y * dY, 0);

	float3 direction = spPos - origin;
	direction = normalize3(direction);

	ray->position = origin;
	ray->direction = direction;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

__kernel void RayTracer(const int screenWidth,
						const int screenHeight,
						__global float* buffer,
						__global Cam* camera,
						__global Ball* balls,
						const int ballCount,
						__global Light* lights,
						const int lightCount,
						__global Triangle* triangles,
						const int triangleCount,
						__global Mat* materials,
						const int materialCount)
{
	const int globalId = get_global_id(0);
	const int x = globalId % screenWidth;
	const int y = globalId / screenWidth;

	Ray ray;
	generatePrimaryRay(camera, x, y, screenWidth, screenHeight, &ray);
	float distance = 0.0f;
	float refractiveIndex = 1.0f;
	float3 colour = _float3(0.0f, 0.0f, 0.0f);
	rayTrace(&ray, &colour, INITIALDEPTH, &distance, refractiveIndex, balls, ballCount, lights, lightCount, triangles, triangleCount, materials, materialCount);

	buffer[4 * (y * screenWidth + x)] = colour.x;
	buffer[4 * (y * screenWidth + x) + 1] = colour.y;
	buffer[4 * (y * screenWidth + x) + 2] = colour.z;
	buffer[4 * (y * screenWidth + x) + 3] = 1.0f; // Alpha

	//buffer[4 * (y * screenWidth + x)] = 0.5f;
	//buffer[4 * (y * screenWidth + x) + 1] = 0.5f;
	//buffer[4 * (y * screenWidth + x) + 2] = 0.5f;
	//buffer[4 * (y * screenWidth + x) + 3] = 1.0f; // Alpha
}

////////////////////////////////////////////////////////////////////////////////////////////////////