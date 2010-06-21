#pragma once

#include "Common.h"
#include "IRayTracer.h"

class RayTracerCPU : public IRayTracer
{
	public:
		RayTracerCPU();
		~RayTracerCPU();

		bool initialize();
		void rayTrace(float* buffer);

	protected:

	private:
		float calculateShade( Primitive* light, Vector3 intersectionPoint );
		Primitive* trace (Ray& aRay, Colour& aColour, GLuint depth, float aRefractionIndex, float& aDistance);
};