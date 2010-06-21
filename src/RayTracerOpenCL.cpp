#include "RayTracerOpenCL.h"

//*****************************************************************************
// CONSTRUCTOR + DECONSTRUCTOR
//*****************************************************************************
RayTracerOpenCL::RayTracerOpenCL()
{
	useGPU = false;
	clWorkGroupSize = 1;
	clForceWorkGroupSize = 0;

	fprintf(stderr, "INFO: OpenCL Tracer Created\n");
}

RayTracerOpenCL::~RayTracerOpenCL()
{

}

//*****************************************************************************
// PRIVATE METHODS
//*****************************************************************************

//************************************
// Method:    clPrintDeviceInfo - print info for the given device
// Returns:   true on success, false otherwise
// Parameter: cl_device_id & device - the specified device
// Parameter: cl_uint i - the devices numeric index
// Parameter: bool selected - whether or not the device has been selected
//************************************
bool RayTracerOpenCL::clPrintDeviceInfo( cl_device_id& aDevice, cl_uint i, bool selected, cl_device_type& aType ) 
{
	char deviceNameBuffer[256];
	char* stringSelected = selected ? "[SELECTED] " : "";

	// Device Type
	clError = clGetDeviceInfo(aDevice, CL_DEVICE_TYPE, sizeof(cl_device_type), &aType, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to get OpenCL Device[%d] TYPE\n", i);
		return false;
	}
	char* stringType;
	switch(aType)
	{
	case CL_DEVICE_TYPE_ALL:
		stringType = "TYPE_ALL";
		break;
	case CL_DEVICE_TYPE_DEFAULT:
		stringType = "TYPE_DEFAULT";
		break;
	case CL_DEVICE_TYPE_CPU:
		stringType = "TYPE_CPU";
		break;
	case CL_DEVICE_TYPE_GPU:
		stringType = "TYPE_GPU";
		break;
	default:
		stringType = "TYPE_UNKNOWN";
		break;
	}
	fprintf(stderr, "INFO: %sOpenCL Device[%d] Type: %s\n", stringSelected, i, stringType);

	// Device Name
	clError = clGetDeviceInfo(aDevice, CL_DEVICE_NAME, sizeof(char[256]), &deviceNameBuffer, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to get OpenCL Device[%d] NAME\n");
		return false;
	}
	fprintf(stderr, "INFO: %sOpenCL Device[%d] Name: %s\n", stringSelected, i, deviceNameBuffer);

	// Device's Compute Units
	cl_uint numComputeUnits = 0;
	clError = clGetDeviceInfo(aDevice, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &numComputeUnits, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to get OpenCL Device[%d] MAX_COMPUTE_UNITS\n");
		return false;
	}
	fprintf(stderr, "INFO: %sOpenCL Device[%d] Compute Units: %d\n", stringSelected, i, numComputeUnits);

	// Device's Max. Work Group Size
	size_t workGroupSize = 0;
	clError = clGetDeviceInfo(aDevice, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(cl_uint), &workGroupSize, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to get OpenCL Device[%d] MAX_WORK_GROUP_SIZE\n");
		return false;
	}
	fprintf(stderr, "INFO: %sOpenCL Device[%d] Max. Work Group Size: %d\n", stringSelected, i, (cl_uint) workGroupSize);

	return true;
}

//************************************
// Method:    clSelectDevice - select a device on the given platform
// Returns:   true on success, false otherwise
// Parameter: cl_platform_id platform - the given platform
// Parameter: cl_device_id & selectedDevice - where to store the selected device
// Parameter: cl_device_id * devices - 
//************************************
bool RayTracerOpenCL::clSelectDevice( cl_platform_id platform, cl_device_id& selectedDevice) 
{
	cl_device_id devices[32];
	cl_uint numDevices;

	clError = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 32, devices, &numDevices);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to get OpenCL Device IDs\n");
		return false;
	}

	bool deviceFound = false;

	for(cl_uint i = 0; i < numDevices; i++)
	{
		cl_device_type type = 0;
		cl_device_id device = devices[i];
		if( !clPrintDeviceInfo(device, i, false, type) ) return false;

		if (!deviceFound)
		{
			if (useGPU && type == CL_DEVICE_TYPE_GPU)
			{
				deviceFound = true;
				selectedDevice = device;
			}

			if(!useGPU && type == CL_DEVICE_TYPE_CPU)
			{
				deviceFound = true;
				selectedDevice = device;
			}
		}
	}

	if(!deviceFound)
	{
		fprintf(stderr, "ERROR: Unable to select an appropriate OpenCL device\n");
		return false;
	}

	return true;
}

//************************************
// Method:    clSelectPlatform - Select the first platform available
// Returns:   true on success, false otherwise
// Parameter: cl_platform_id & platform - where to store the selected platform
//************************************
bool RayTracerOpenCL::clSelectPlatform(cl_platform_id& platform) 
{
	cl_uint numPlatforms;
	platform = NULL;
	clError = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (clError != CL_SUCCESS || numPlatforms == 0)
	{
		fprintf(stderr, "ERROR: Failed to get OpenCL platforms\n");
		return false;
	}

	if (numPlatforms > 0)
	{
		cl_platform_id *platforms = (cl_platform_id *) malloc(sizeof(cl_platform_id) * numPlatforms);
		clError = clGetPlatformIDs(numPlatforms, platforms, NULL);
		if (clError != CL_SUCCESS)
		{
			fprintf(stderr, "ERROR: Failed to get OpenCL platform IDs\n");
			return false;
		}

		for(cl_uint i = 0; i < numPlatforms; i++)
		{
			char platformBuffer[100];
			clError = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platformBuffer), platformBuffer, NULL);

			fprintf(stderr, "INFO: OpenCL Platform[%d]: %s\n", i, platformBuffer);
		}

		platform = platforms[0];
		free(platforms);
	}
	return true;
}

bool RayTracerOpenCL::clSetupBuffers()
{
	cl_uint pixelBufferSize = sizeof(float) * screenHeight * screenWidth * 4;
	clPixelBuffer = clCreateBuffer(clContext, CL_MEM_READ_WRITE, pixelBufferSize, NULL, &clError);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to create OpenCL pixel buffer: %d\n", clError);
		return false;
	}

	clBallBuffer = clCreateBuffer(clContext, CL_MEM_READ_ONLY, sizeof(Ball) * ballCount, NULL, &clError);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to create OpenCL ball buffer: %d\n", clError);
		return false;
	}

	clLightBuffer = clCreateBuffer(clContext, CL_MEM_READ_ONLY, sizeof(Light) * lightCount, NULL, &clError);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to create OpenCL light buffer: %d\n", clError);
		return false;
	}

	clMaterialBuffer = clCreateBuffer(clContext, CL_MEM_READ_ONLY, sizeof(Mat) * materialCount, NULL, &clError);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to create OpenCL light buffer: %d\n", clError);
		return false;
	}

	clTriangleBuffer = clCreateBuffer(clContext, CL_MEM_READ_ONLY, sizeof(Triangle) * triangleCount, NULL, &clError);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to create OpenCL triangle buffer: %d\n", clError);
		return false;
	}

	clCameraBuffer = clCreateBuffer(clContext, CL_MEM_READ_ONLY, sizeof(Cam), NULL, &clError);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to create OpenCL camera buffer: %d\n", clError);
		return false;
	}

	fprintf(stderr, "INFO: Successfully Created OpenCL Buffers\n");
	return true;
}


//************************************
// Method:    clSetupCommandQueue - create an OpenCL command queue
// Returns:   true on success, false otherwise
// Parameter: cl_device_id & aDevice - the device associated with the context
//************************************
bool RayTracerOpenCL::clSetupCommandQueue(cl_device_id& aDevice)
{
	cl_command_queue_properties commandQueueProperty = 0;
	clCommandQueue = clCreateCommandQueue(clContext, aDevice, commandQueueProperty, &clError);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to create OpenCL Command Queue: %d\n", clError);
		return false;
	}

	fprintf(stderr, "INFO: Successfully Created OpenCL Command Queue\n");
	return true;
}

//************************************
// Method:    clSetupContext - setup an OpenCL context from the given parameters
// Returns:   true on success, false otherwise
// Parameter: cl_platform_id platform - the platform to use
// Parameter: cl_device_id selectedDevice - the device to use
//************************************
bool RayTracerOpenCL::clSetupContext( cl_platform_id platform, cl_device_id selectedDevice, cl_device_id* aDevices ) 
{
	cl_context_properties contextProperty[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties) platform, 0};
	cl_context_properties* contextProperties = (platform == NULL) ? NULL : contextProperty;

	clContext = clCreateContext(contextProperties, 1, &selectedDevice, NULL, NULL, &clError);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to create OpenCL context\n");
		return false;
	}

	// Get Device List from Context
	size_t deviceListSize;
	clError = clGetContextInfo(clContext, CL_CONTEXT_DEVICES, 32, aDevices, &deviceListSize);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to get OpenCL Context Info\n");
		return false;
	}

	// Print devices list
	for(cl_uint i = 0; i < deviceListSize / sizeof(cl_device_id); i++)
	{
		cl_device_type type = 0;
		cl_device_id device = aDevices[i];
		if( !clPrintDeviceInfo(device, i, true, type) ) return false;
	}

	fprintf(stderr, "INFO: Successfully Created OpenCL Context\n");
	return true;
}

//************************************
// Method:    clSetupKernel - setup a kernel from the specified source file
// Returns:   true on success, false otherwise
// Parameter: char * fileName - the source file from which to generate a kernel
//************************************
bool RayTracerOpenCL::clSetupKernel(char* fileName, cl_device_id* aDevices)
{
	const char* programSource = readSourceFile(fileName);

	if ( !programSource ) return false;

	// Create program
	clProgram = clCreateProgramWithSource(clContext, 1, &programSource, NULL, &clError);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to create OpenCL program from source file %s\n", fileName);
		return false;
	}

	// Build program
	clError = clBuildProgram(clProgram, 1, aDevices, "-I. ", NULL, NULL);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to build OpenCL kernel: %d\n", clError);
		
		// Print out Build Log on failure
		size_t returnValueSize;
		clError = clGetProgramBuildInfo(clProgram, aDevices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &returnValueSize);
		if ( clError != CL_SUCCESS )
		{
			fprintf(stderr, "ERROR: Failed to get OpenCL Program Build Log Size: %d", clError);
			return false;
		}

		char* programBuildLog = (char *) malloc(returnValueSize + 1);
		clError = clGetProgramBuildInfo(clProgram, aDevices[0], CL_PROGRAM_BUILD_LOG, returnValueSize, programBuildLog, NULL);
		if ( clError != CL_SUCCESS )
		{
			fprintf(stderr, "ERROR: Failed to get OpenCL Program Build Log: %d", clError);
			return false;
		}
		programBuildLog[returnValueSize] = '\0';

		fprintf(stderr, "ERROR: OpenCL Program Build Log: \n\n%s\n\n", programBuildLog);
		free(programBuildLog);
		return false;
	}

	// Create kernel
	clKernel = clCreateKernel(clProgram, "RayTracer", &clError);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to create OpenCL kernel: %d\n", clError);
		return false;
	}

	// LordCRC's patch for more accurate work group size
	size_t groupSize = 0;
	clError = clGetKernelWorkGroupInfo(clKernel, aDevices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &groupSize, NULL);
	if ( clError != CL_SUCCESS )
	{
		fprintf(stderr, "ERROR: Failed to get OpenCL kernel work group info: %d\n", clError);
		return false;
	}
	clWorkGroupSize = (unsigned int) groupSize;
	fprintf(stderr, "INFO: OpenCL Device 0: Kernel Work Group Size = %d\n", clWorkGroupSize);

	if(clForceWorkGroupSize > 0)
	{
		fprintf(stderr, "INFO: OpenCL Device 0: Forced Kernel Work Group Size = %d\n", clForceWorkGroupSize);
		clWorkGroupSize = clForceWorkGroupSize;
	}

fprintf(stderr, "INFO: Successfully created OpenCL Kernel\n");
	return true;
}

//************************************
// Method:    readSourceFile - read specified source file
// Returns:   char* - file contents
// Parameter: char * fileName - the file name of the specified file
//************************************
char* RayTracerOpenCL::readSourceFile( char* fileName ) 
{
	FILE* file;
	long fileSize;
	char* fileContents;

	fopen_s(&file, fileName, "rt");

	if(!file)
	{
		fprintf(stderr, "ERROR: Failed to open file '%s'\n", fileName);
		return 0;
	}

	// Calculate file size
	if(fseek(file, 0, SEEK_END))
	{
		fprintf(stderr, "ERROR: Failed to seek to end of file\n", fileName);
		return 0;
	}
	fileSize = ftell(file);
	if(fileSize == 0)
	{
		fprintf(stderr, "ERROR: Failed to check position on file, or file is empty\n");
		return false;
	}
	rewind(file);

	// Allocate memory
	fileContents = (char*) malloc(sizeof(char) * fileSize + 1);
	if(!fileContents)
	{
		fprintf(stderr, "ERROR: Failed to allocate memory for file contents\n");
		return 0;
	}

	// Read File
	fprintf(stderr, "INFO: Reading File '%s'...\n", fileName);
	size_t result = fread(fileContents, 1, sizeof(char) * fileSize, file);
	if (result != sizeof(char) * fileSize)
	{
		fprintf(stderr, "\n\n %s \n\n", fileContents);
		fprintf(stderr, "ERROR: Failed to read file '%s', only read %ld, expected %ld\n", fileName, result, fileSize);
		return 0;
	}

	// Tidy up
	fileContents[fileSize] = '\0';
	fclose(file);

	fprintf(stderr, "INFO: File %s Read Successfully\n", fileName);

	return fileContents;
}

//************************************
// Method:		initializeOpenCL - initialize OpenCL
// Returns:		true on success, false otherwise
//************************************
bool RayTracerOpenCL::initializeOpenCL()
{
	cl_platform_id platform;
	cl_device_id selectedDevice;
	cl_device_id devices[32];

	// Select the platform
	if ( !clSelectPlatform(platform) ) return false;

	// Select the device
	if ( !clSelectDevice(platform, selectedDevice) ) return false;

	// Create OpenCL context
	if ( !clSetupContext(platform, selectedDevice, devices) ) return false;

	// Setup command queue
	if ( !clSetupCommandQueue(selectedDevice) ) return false;

	// Setup buffers
	if ( !clSetupBuffers() ) return false;

	// Setup OpenCL kernel
	if ( !clSetupKernel("tracer.cl", devices) ) return false;

	return true;
}

void RayTracerOpenCL::setupScene()
{
	Camera* camera = this->scene->getCamera();
	this->tracerCam.position.x = camera->position.x;
	this->tracerCam.position.y = camera->position.y;
	this->tracerCam.position.z = camera->position.z;
	this->tracerCam.spXLeft = camera->spXLeft;
	this->tracerCam.spXRight = camera->spXRight;
	this->tracerCam.spYBottom = camera->spYBottom;
	this->tracerCam.spYTop = camera->spYTop;

	materialCount = 2;
	materials = (Mat*) malloc(sizeof(Mat) * materialCount);
	materials[0] = _Mat(_RGBf(0.0f, 1.0f , 0.0f), 1);
	materials[0].specular = 0.7f;
	materials[0].reflection = 0.4f;
	materials[0].refraction = 0.5f;
	materials[0].refractiveIndex = 1.33f;
	materials[1] = _Mat(_RGBf(0.0f, 1.0f , 1.0f), 2);
	materials[1].specular = 0.5f;
	materials[1].reflection = 0.4f;
	materials[1].refraction = 0.5f;
	materials[1].refractiveIndex = 1.33f;

	ballCount = 2;
	balls = (Ball*) malloc(sizeof(Ball) * ballCount);
	balls[0] = _Ball(_Vec3(-2.f, -2.8f, 4.f), 1.25f);
	balls[0].materialIndex = 1;
	balls[1] = _Ball(_Vec3(2.f, -2.8f, 3.f), 1.25f);
	balls[1].materialIndex = 2;

	triangleCount = 10;
	triangles = (Triangle*) malloc(sizeof(Ball) * triangleCount);

	lightCount = 1;
	lights = (Light*) malloc(sizeof(Light) * lightCount);
	lights[0] = _Light(_Vec3(0, 2.2f, 0),_RGBf(1.0f, 1.0f, 1.0f), POINTLIGHT);
	// lights[1] = _Light(_Vec3(0, 2.2f, -5),_RGBf(1.0f, 1.0f, 1.0f), POINTLIGHT);
}

//*****************************************************************************
// PUBLIC METHODS
//*****************************************************************************

bool RayTracerOpenCL::initialize()
{
	setupScene();

	if (!initializeOpenCL())
	{
		fprintf(stderr, "ERROR: Failed to initialize OpenCL\n");
		return false;
	}

	fprintf(stderr, "INFO: Successfully Initialized OpenCL RayTracer\n");
	return true;
}


void RayTracerOpenCL::rayTrace(float* buffer)
{
	size_t globalThreads[1], localThreads[1];

	globalThreads[0] = screenWidth * screenHeight;
	if(globalThreads[0] % clWorkGroupSize != 0)
	{
		globalThreads[0] = ( globalThreads[0] / clWorkGroupSize + 1 ) * clWorkGroupSize;
	}
	localThreads[0] = 64;

	// Move necessary data to buffers
	clError = clEnqueueWriteBuffer(clCommandQueue, clCameraBuffer, CL_TRUE, 0, sizeof(Cam), &(this->tracerCam), 0, NULL, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to write the OpenCL camera buffer: %d\n", clError);
		exit(-1);
	}
	clError = clEnqueueWriteBuffer(clCommandQueue, clBallBuffer, CL_TRUE, 0, sizeof(Ball) * ballCount, balls, 0, NULL, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to write the OpenCL ball buffer: %d\n", clError);
		exit(-1);
	}
	clError = clEnqueueWriteBuffer(clCommandQueue, clLightBuffer, CL_TRUE, 0, sizeof(Light) * lightCount, lights, 0, NULL, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to write the OpenCL light buffer: %d\n", clError);
		exit(-1);
	}
	clError = clEnqueueWriteBuffer(clCommandQueue, clMaterialBuffer, CL_TRUE, 0, sizeof(Mat) * materialCount, materials, 0, NULL, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to write the OpenCL light buffer: %d\n", clError);
		exit(-1);
	}
	clError = clEnqueueWriteBuffer(clCommandQueue, clTriangleBuffer, CL_TRUE, 0, sizeof(Triangle) * triangleCount, triangles, 0, NULL, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to write the OpenCL triangle buffer: %d\n", clError);
		exit(-1);
	}

	clError = clSetKernelArg(clKernel, 0, sizeof(int),(void*) &screenWidth);
	clError = clSetKernelArg(clKernel, 1, sizeof(int), (void*) &screenHeight);
	clError = clSetKernelArg(clKernel, 2, sizeof(cl_mem), &clPixelBuffer);
	clError = clSetKernelArg(clKernel, 3, sizeof(cl_mem), &clCameraBuffer);

	clError = clSetKernelArg(clKernel, 4, sizeof(cl_mem), &clBallBuffer);
	clError = clSetKernelArg(clKernel, 5, sizeof(cl_uint),(void*) &ballCount);

	clError = clSetKernelArg(clKernel, 6, sizeof(cl_mem), &clLightBuffer);
	clError = clSetKernelArg(clKernel, 7, sizeof(cl_uint),(void*) &lightCount);

	clError = clSetKernelArg(clKernel, 8, sizeof(cl_mem), &clTriangleBuffer);
	clError = clSetKernelArg(clKernel, 9, sizeof(cl_uint),(void*) &triangleCount);

	clError = clSetKernelArg(clKernel, 10, sizeof(cl_mem), &clMaterialBuffer);
	clError = clSetKernelArg(clKernel, 11, sizeof(cl_uint),(void*) &materialCount);

	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to set kernel arguments: %d\n", clError);
	}

	// Execute Kernel
	clError = clEnqueueNDRangeKernel(clCommandQueue, clKernel, 1, NULL, globalThreads, localThreads, 0, NULL, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to enqueue OpenCL work: %d\n", clError);
		exit(-1);
	}
	clFinish(clCommandQueue);

	// Read output from buffer
	clError = clEnqueueReadBuffer(clCommandQueue, clPixelBuffer, CL_TRUE, 0, screenWidth * screenHeight * 4 * sizeof(float), buffer, 0, NULL, NULL);
	if(clError != CL_SUCCESS)
	{
		fprintf(stderr, "ERROR: Failed to read the OpenCL pixel buffer: %d\n", clError);
		exit(-1);
	}

	clFinish(clCommandQueue);

}

