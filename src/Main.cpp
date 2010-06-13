#include "Common.h"
#include "Scene.h"

using namespace std;

//*****************************************************************************
// INSTANCE VARIABLES
//*****************************************************************************

// Tracer variables
bool			running = true;
GLuint			maxDepth = 1;
GLuint			pboId;
GLuint			screenWidth = 1024;
GLuint			screenHeight = 768;
GLuint			screenBitDepth = 32;
Scene*			scene = NULL;

// OpenGL variables
GLuint			textures;
GLubyte*		buffer;

// SDL variables
SDL_Event		event;
SDL_Surface*	screen;

// FPS variables
int				fpsUpdateRate = 100; // FPS update rate (ms)
double			fpsCurrent = 0.0;
double			fpsAverage = 0.0;
int				framesSinceStart = 0;	
int				framesSinceLast = 0;
int				timeStart = 0;
int				timeLast = 0;

//*****************************************************************************
// METHODS
//*****************************************************************************

//************************************
// Method:		startup - initialize OpenGL
//************************************
bool initializeOpenGL() 
{
	// Initialise GLEW
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(status));
	}

	int glError = GL_NO_ERROR;

	// OpenGL Settings
	glClearColor(0, 0, 0, 1);
	glShadeModel(GL_FLAT);
	glEnable(GL_TEXTURE_2D);
	glError = glGetError();

	// Setup PBO
	glGenBuffers(1, &pboId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glError = glGetError();

	// No borders  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);  
	glError = glGetError();

	// No bilinear filtering of texture  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  
	glError = glGetError();

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId); // Bind PBO
	glBufferData(GL_PIXEL_UNPACK_BUFFER, screenWidth * screenHeight * 3, 0, GL_STREAM_DRAW); // Initialise PBO
	glError = glGetError();

	return (glGetError() == GL_NO_ERROR);
}

//************************************
// Method:		startup - initialize stuff
// Returns:		true on success, false otherwise
//************************************
bool startup()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		fprintf(stderr, "ERROR: SDL initialization failed: %s\n",SDL_GetError());
		return false;
	}
	
	// Set up the screen
	screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBitDepth, SDL_OPENGL);

	// Set the window caption
	SDL_WM_SetCaption("GPU Distributed Ray Tracer", NULL);

	if ( screen == NULL )
	{
		fprintf(stderr, "ERROR: Could NOT set (%d x %d x %d) video mode: %s\n", screenWidth, screenHeight, screenBitDepth, SDL_GetError());
		return false;
	}

	if (!initializeOpenGL())
	{
		fprintf(stderr, "ERROR: OpenGL initialization failed\n");
	}
	
	printf("INFO: SDL & OpenGL initialization complete\n");
	return true;
	
}

//************************************
// Method:		handleKeyDown - handle the key which has been pressed
// Parameter:	SDLKey - the key to be handled
//************************************
void handleKeyDown(SDLKey key) 
{
	switch ( key )
	{
	case SDLK_ESCAPE:
		running = false;
	}
}

//************************************
// Method:		handleEvent - handle the given event
// Parameter:	SDL_Event *event - the event to be handled
//************************************
void handleEvent(SDL_Event* event) 
{
	switch( event->type )
	{
	case SDL_QUIT:
		running = false;
		printf("EVENT: SDL_QUIT\n");
		break;

	case SDL_KEYDOWN:
		printf("EVENT: SDL_KEYDOWN\n");
		handleKeyDown(event->key.keysym.sym);
		break;
	}
}

//************************************
// Method:		shutdown - clean up before exit
//************************************
void shutdown()
{
	// OpenGL cleanup
	glDeleteTextures(1, &textures);

	// Shutdown SDL
	SDL_Quit();
}

//************************************
// Method:		waitForEnter - wait for the user to press enter
//************************************
void waitForEnter() 
{
	printf("\n\nINFO: Press [Enter] to continue...\n");
	std::cin.get();
}

//************************************
// Method:		updateStats - update window caption to display statistics
//************************************
void updateStats()
{
	framesSinceLast++;
	framesSinceStart++;

	double timeSinceLast = SDL_GetTicks() - timeLast;
	double timeSinceStart = SDL_GetTicks() - timeStart;
	if (timeSinceLast > fpsUpdateRate)
	{
		timeLast = SDL_GetTicks();
		fpsCurrent = framesSinceLast / (timeSinceLast/1000);
		fpsAverage = framesSinceStart / (timeSinceStart/1000);
		framesSinceLast = 0;
	}

	char captionBuffer[200];
	sprintf_s(captionBuffer, "GPU Distributed Ray Tracer | %dx%dx%d | Current FPS: %.1f | Average FPS: %.1f", screenWidth, screenHeight, screenBitDepth, fpsCurrent, fpsAverage);
	SDL_WM_SetCaption(captionBuffer, NULL);
}

//************************************
// Method:		trace - fire a ray
// Returns:		
// Parameter:	Ray r - the ray to be traced
// Parameter:	GLuint depth - the current depth of the trace
//************************************
Primitive* trace (Ray& aRay, Colour& aColour, GLuint depth, float aRefractionIndex, float& aDistance)
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

		for(int l = 0; l < scene->getPrimitivesCount(); l++)
		{
			Primitive* p = scene->getPrimitive(l);

			if(p->isLight())
			{
				Primitive* light = p;

				//***********************************
				// SHADOWS
				//***********************************
				float shade = 1.0f;
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

//************************************
// Method:    rayTrace - ray trace
//************************************
void rayTrace(GLubyte* buffer)
{
	Camera* camera = scene->getCamera();
	Vector3 origin = camera->position;

	float x = origin.x;
	float y = origin.y;
	float z = origin.z;

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

			// Convert from float to int
			int iRed = (int) (colour.red * 256);
			int iGreen = (int) (colour.green * 256);
			int iBlue = (int) (colour.blue * 256);
			if (iRed > 255) iRed = 255;
			if (iGreen > 255) iGreen = 255;
			if (iBlue > 255) iBlue = 255;

			buffer[3 * (y * screenWidth + x)] = iRed;
			buffer[3 * (y * screenWidth + x) + 1] = iGreen;
			buffer[3 * (y * screenWidth + x) + 2] = iBlue;
		}
	}
}

//************************************
// Method:		render - render the pixels given
//************************************
void render() 
{
	// Bind PBO
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId);

	// Lock Buffer
	buffer = (GLubyte *) glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

	// Trace the scene
	rayTrace(buffer);

	// Unlock Buffer
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Output Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
	glEnd();

	// Swap OpenGL frame buffers/Update Display 
	SDL_GL_SwapBuffers();	
}

//************************************
// Method:		main - program entry point
// Returns:		0 for success 1 otherwise
// Parameter:	int argc
// Parameter:	char * argv
//************************************
int main( int argc, char* argv[] )
{
	if(!startup())
	{
		shutdown();
		waitForEnter();
		return -1;
	}

	// Save the start time for FPS calculations
	timeStart = timeLast = SDL_GetTicks();

	// Create Scene
	scene = new Scene(screenWidth, screenHeight);
	scene->InitSceneCornellBox();
	//scene->InitScene2();
	///scene->InitScene1();
	scene->getCamera()->position.z = -5;

	// Main Loop
	while (running)
	{
		// Events
		while (SDL_PollEvent( &event ))
			handleEvent(&event);

		// Update Stats
		updateStats();

		// Rendering
		render();
	}

	shutdown();

	return 0;
}