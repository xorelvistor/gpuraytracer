#include "RayTracerCPU.h"

//*****************************************************************************
// CONSTRUCTOR + DECONSTRUCTOR
//*****************************************************************************
RayTracerCPU::RayTracerCPU()
{
	fprintf(stderr, "INFO: CPU Tracer Created\n");
}

RayTracerCPU::~RayTracerCPU()
{
	
}

//*****************************************************************************
// PRIVATE METHODS
//*****************************************************************************

//************************************
// Method:    calculateShade - calculates the shade value at a given intersection point
// Returns:   float - returns whether or not the point is in shade, 1/visible -> 0/occluded
// Parameter: Primitive * light - the light source to check against 
// Parameter: Vector3 intersectionPoint - the intersection point to check from
//************************************
float RayTracerCPU::calculateShade( Primitive* light, Vector3 intersectionPoint ) 
{
	float shade = 1.0f;

	// Point Lights
	if (light->getType() == SPHERE)
	{
		Vector3 L = ((Sphere*) light)->getCentre() - intersectionPoint;
		float distanceFromLight = LENGTH(L);
		L *= (1.0f/distanceFromLight);
		Ray r = Ray(intersectionPoint + L * EPSILON, L);

		for(int s = 0; s < scene->getPrimitivesCount(); s++)
		{
			Primitive* p = scene->getPrimitive(s);
			if( (p != light) && (p->intersect(r, distanceFromLight) != MISS))
			{
				shade = 0;
				break;
			}
		}
	}
	else
	{
		return shade;
	}
	return shade;
}
//************************************
// Method:		trace - fire a ray
// Returns:		Primitive* - a pointer to the first primitive intersected by the ray
// Parameter:	Ray r - the ray to be traced
// Parameter:	GLuint depth - the current depth of the trace
//************************************
Primitive* RayTracerCPU::trace (Ray& aRay, Colour& aColour, GLuint depth, float aRefractionIndex, float& aDistance)
{
	// Stop tracing after maximum trace depth is hit
	if (depth > MAXTRACEDEPTH) return 0;

	// Trace primary ray
	aDistance = 1000000.0f;
	Vector3 intersectionPoint;
	Primitive* prim = 0;
	int intersectionResult = MISS;

	// Find nearest intersection
	for (int s = 0; s < scene->getPrimitivesCount(); s++)
	{
		Primitive* p = scene->getPrimitive(s);
		int r = p->intersect(aRay, aDistance);
		if (r != MISS)
		{
			prim = p;
			intersectionResult = r;
		}
	}

	// No hit, terminate ray
	if (!prim) return 0;

	// Handle intersection
	if (prim->isLight())
	{
		aColour = Colour(1.0f, 1.0f, 1.0f);
	}
	else
	{
		// Calculate intersection point
		intersectionPoint = aRay.position + aRay.direction * aDistance;
		//aColour = prim->getNormal(intersectionPoint);

		for(int l = 0; l < scene->getPrimitivesCount(); l++)
		{
			Primitive* p = scene->getPrimitive(l);

			if(p->isLight())
			{
				Primitive* light = p;

				//***********************************
				// SHADOWS
				//***********************************
				float shade = calculateShade(light, intersectionPoint);

				if (shade > 0)
				{
					Vector3 L = ((Sphere*)light)->getCentre() - intersectionPoint;
					NORMALIZE(L);
					Vector3 N = prim->getNormal(intersectionPoint);

					//***********************************
					// DIFFUSE SHADING
					//***********************************
					if (prim->getMaterial()->getDiffuse() > 0)
					{
						float dot = DOT(L, N);
						if (dot > 0)
						{
							float diffuse = dot * prim->getMaterial()->getDiffuse() * shade;
							aColour += diffuse * prim->getMaterial()->getColour() * light->getMaterial()->getColour();
						}
					}

					//***********************************
					// SPECULAR SHADING
					//***********************************
					if (prim->getMaterial()->getSpecular() > 0)
					{
						Vector3 V = aRay.direction;
						Vector3 R = L - 2.0f * DOT(L, N) * N;
						float dot = DOT(V, R);
						if (dot > 0)
						{
							float specular = powf(dot, 20) * prim->getMaterial()->getSpecular() * shade;
							aColour += specular * light->getMaterial()->getColour();
						}
					}
				}
			}
		}

		//***********************************
		// REFLECTION
		//***********************************
		float reflection = prim->getMaterial()->getReflection();
		if(reflection > 0.0f && depth < MAXTRACEDEPTH)
		{
			Vector3 N = prim->getNormal(intersectionPoint);
			Vector3 R = aRay.direction - 2.0f * DOT(aRay.direction, N) * N;
			Colour reflectionColor(0, 0, 0);
			float dist;
			trace(Ray(intersectionPoint + R * EPSILON, R), reflectionColor, depth + 1, aRefractionIndex, dist);
			aColour += reflection * reflectionColor * prim->getMaterial()->getColour();
		}

		//***********************************
		// REFRACTION
		//***********************************
		float refraction = prim->getMaterial()->getRefraction();
		if (refraction > 0 && depth < MAXTRACEDEPTH)
		{
			float refractionIndex = prim->getMaterial()->getRefractiveIndex();
			float refractionRatio = aRefractionIndex / refractionIndex;
			Vector3 N = prim->getNormal(intersectionPoint) * (float) intersectionResult;
			float cosI = -DOT(N, aRay.direction);
			float cosT2 = 1.0f - refractionRatio * refractionRatio * (1.0f - cosI * cosI);
			if(cosT2 > 0.0f)
			{
				Vector3 T = (refractionRatio * aRay.direction) + (refractionRatio * cosI - sqrtf(cosT2)) * N;
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
				aColour += refractionColour * transparency;
			}
		}
	}

	return prim;
}

//*****************************************************************************
// PUBLIC METHODS
//*****************************************************************************
bool RayTracerCPU::initialize()
{
	fprintf(stderr, "INFO: Successfully Initialized CPU RayTracer\n");
	return true;
}

void RayTracerCPU::rayTrace(float* buffer)
{
	Camera* camera = scene->getCamera();
	Vector3 origin = camera->position;

	float left = camera->spXLeft;
	float right = camera->spXRight;
	float top = camera->spYTop;
	float bottom = camera->spYBottom;

	float deltaX = (right - left) / screenWidth;
	float deltaY = (top - bottom) / screenHeight;

	for(GLuint y = 0; y < screenHeight; y++)
	{
		for(GLuint x = 0; x < screenWidth; x++)
		{
			float xpos = left + x * deltaX;
			float ypos = bottom + y * deltaY;
			float zpos = 0;

			Vector3 direction = Vector3(xpos, ypos, zpos) - origin;
			direction.Normalize();

			// Set up variables which will be used during the trace
			Ray ray(origin, direction);
			Colour colour(0, 0, 0);
			float distance;
			trace(ray, colour, 1, 1.0f, distance);

			buffer[4 * (y * screenWidth + x)] = colour.red;
			buffer[4 * (y * screenWidth + x) + 1] = colour.green;
			buffer[4 * (y * screenWidth + x) + 2] = colour.blue;
			buffer[4 * (y * screenWidth + x) + 3] = 1.0f; // Alpha
		}
	}	
}
