#pragma once

#include "Common.h"
#include "IRayTracer.h"
#include "OpenCL/ball.h"
#include "OpenCL/cam.h"
#include "OpenCL/material.h"
#include "OpenCL/light.h"
#include "OpenCL/triangle.h"

class RayTracerOpenCL : public IRayTracer
{
	public:
		RayTracerOpenCL();
		~RayTracerOpenCL();

		bool				initialize();
		void				rayTrace(float* buffer);

		bool				getUseGPU(){ return useGPU; };
		void				setUseGPU(bool use){ this->useGPU = use; };

	protected:

	private:
		bool				initializeOpenCL();
		bool				clPrintDeviceInfo( cl_device_id& aDevice, cl_uint i, bool selected, cl_device_type& aType );
		bool				clSelectDevice( cl_platform_id platform, cl_device_id& selectedDevice);
		bool				clSelectPlatform(cl_platform_id& platform);
		bool				clSetupBuffers();
		bool				clSetupCommandQueue(cl_device_id& aDevice);
		bool				clSetupContext( cl_platform_id platform, cl_device_id selectedDevice, cl_device_id* aDevices );
		bool				clSetupKernel( char* fileName, cl_device_id* aDevices );
		char*				readSourceFile( char* fileName );
		void				setupScene();

		// OpenCL variables
		bool				useGPU;
		cl_uint				clWorkGroupSize;
		cl_uint				clForceWorkGroupSize;
		cl_command_queue	clCommandQueue;
		cl_context			clContext;
		cl_kernel			clKernel;
		cl_program			clProgram;
		cl_mem				clCameraBuffer;
		cl_mem				clLightBuffer;
		cl_mem				clMaterialBuffer;
		cl_mem				clPixelBuffer;
		cl_mem				clTriangleBuffer;
		cl_mem				clBallBuffer;
		cl_int				clError;

		// Tracer variables
		Cam					tracerCam;
		
		cl_uint				ballCount;
		Ball*				balls;
		
		cl_uint				lightCount;
		Light*				lights;

		cl_uint				triangleCount;
		Triangle*			triangles;

		cl_uint				materialCount;
		Mat*				materials;

};