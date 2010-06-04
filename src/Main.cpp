#include <cmath>
#include <math.h>
#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>

using namespace std;

//*****************************************************************************
// STRUCTURES
//*****************************************************************************
struct Colour
{
	float r, g, b;
	Colour():r(0),g(0),b(0) {}
};

struct Vector3
{
	float x, y, z;

	Vector3():x(0),y(0),z(0) {}
	Vector3(float _x, float _y, float _z):x(_x),y(_y),z(_z) {}

	Vector3 operator+(const Vector3& v) const {return Vector3(x+v.x, y+v.y, z+v.z);}
	Vector3 operator-(const Vector3& v) const {return Vector3(x-v.x, y-v.y, z-v.z);}
	Vector3 operator*(const Vector3& v) const {return Vector3(x*v.x, y*v.y, z*v.z);}
	Vector3 operator+(const float v) const {return Vector3(x+v, y+v, z+v);}
	Vector3 operator-(const float v) const {return Vector3(x-v, y-v, z-v);}
	Vector3 operator*(const float v) const {return Vector3(x*v, y*v, z*v);}
	Vector3 operator/(const float v) const {return Vector3(x/v, y/v, z/v);}

	float   Normalize()
	{
		float l = sqrt(x*x+y*y+z*z);
		float il = 1.0f / l;
		x*=il; y*=il; z*=il;
		return l;
	}
	float   Dot(const Vector3& v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}
};

Vector3   Normalize(const Vector3& v)
{
	float l = 1.0f / sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	return Vector3(v.x*l,v.y*l,v.z*l);
}

struct Ray
{
	Vector3 pos, dir;
	Ray(Vector3 p, Vector3 d):pos(p),dir(d) {}
};

//*****************************************************************************
// INSTANCE VARIABLES
//*****************************************************************************

// Tracer variables
bool		running = true;
GLint		screenWidth = 1024;
GLint		screenHeight = 768;
GLint		screenBitDepth = 32;

// OpenGL variables
GLuint		textures;
Colour		*pixels;

// SDL variables
SDL_Event	event;
SDL_Surface	*screen;

// FPS variables
int			fpsUpdateRate = 100; // FPS update rate (ms)
double		fpsCurrent = 0.0;
double		fpsAverage = 0.0;
int			framesSinceStart = 0;	
int			framesSinceLast = 0;
int			timeStart = 0;
int			timeLast = 0;

//*****************************************************************************
// METHODS
//*****************************************************************************

//************************************
// Method:    startup - initialize OpenGL
//************************************
bool initializeOpenGL() 
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	glClearColor(0,0,0,0);
	glGenTextures(1, &textures);
	glEnable(GL_TEXTURE_2D);

	return (glGetError() != GL_NO_ERROR);
}

//************************************
// Method:    startup - initialize stuff
//************************************
bool startup()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		fprintf(stderr,"SDL initialization failed: %s\n",SDL_GetError());
		return false;
	}
	
	// Set up the screen
	screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBitDepth, SDL_OPENGL);

	// Set the window caption
	SDL_WM_SetCaption("GPU Distributed Ray Tracer", NULL);

	if ( screen == NULL )
	{
		fprintf(stderr,"Could NOT set (%d x %d x %d) video mode: %s\n", screenWidth, screenHeight, screenBitDepth, SDL_GetError());
		return false;
	}

	if (!initializeOpenGL())
	{

	}
	
	printf("SDL & OpenGL initialization complete\n");
	return true;
	
}

//************************************
// Method:    handleKeyDown - handle the key which has been pressed
// Parameter: SDLKey - the key to be handled
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
// Method:    handleEvent - handle the given event
// Parameter: SDL_Event *event - the event to be handled
//************************************
void handleEvent(SDL_Event *event) 
{
	switch( event->type )
	{
	case SDL_QUIT:
		running = false;
		printf("Event! - SDL_QUIT\n");
		break;

	case SDL_KEYDOWN:
		printf("Event! - SDL_KEYDOWN\n");
		handleKeyDown(event->key.keysym.sym);
		break;
	}
}

//************************************
// Method:    shutdown - clean up before exit
//************************************
void shutdown()
{
	// OpenGL cleanup
	glDeleteTextures(1, &textures);

	// Shutdown SDL
	SDL_Quit();
}

//************************************
// Method:    waitForEnter - wait for the user to press enter
//************************************
void waitForEnter() 
{
	printf("\n\nPress [Enter] to continue...");
	std::cin.get();
}

//************************************
// Method:    updateStats - update window caption to display statistics
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
// Method:    rayTrace - ray trace
//************************************
void rayTrace()
{
	for(int y = 0; y < screenHeight; y++)
	{
		for(int x = 0; x < screenWidth; x++)
		{
			Colour * pixel = &(pixels[y * screenWidth + x]);
			pixel->r = fmod(pixel->r + .01f,1);
			pixel->g = fmod(pixel->g + .02f,1);
			pixel->b = fmod(pixel->b + .03f,1);
		}
	}
}

//************************************
// Method:    render - render the pixels given
//************************************
void render() 
{
	rayTrace();

	glBindTexture(GL_TEXTURE_2D, textures);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); glVertex2f(-1.0f,-1.0f);
		glTexCoord2f(1.0f,0.0f); glVertex2f( 1.0f,-1.0f);
		glTexCoord2f(1.0f,1.0f); glVertex2f( 1.0f, 1.0f);
		glTexCoord2f(0.0f,1.0f); glVertex2f(-1.0f, 1.0f);
	glEnd();

	// Swap OpenGL frame buffers/Update Display 
	SDL_GL_SwapBuffers();
}

//************************************
// Method:    main - program entry point
// Returns:   0 for success 1 otherwise
// Parameter: int argc
// Parameter: char * argv
//************************************
int main( int argc, char * argv[] )
{
	if(!startup())
	{
		shutdown();
		waitForEnter();
		return -1;
	}

	// Save the start time for FPS calculations
	timeStart = timeLast = SDL_GetTicks();

	pixels = new Colour[screenWidth * screenHeight];

	// Main Loop
	while (running)
	{
		// Events
		while (SDL_PollEvent( &event ))
		{
			handleEvent(&event);
		}

		// Update Stats
		updateStats();

		// Rendering
		render();
	}

	shutdown();

	return 0;
}
