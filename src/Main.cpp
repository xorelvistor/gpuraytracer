#include <fstream>
#include <iostream>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <CL/cl.hpp>

#include "scene.h"
#include "camera.h"

using namespace std;

//*****************************************************************************
// INSTANCE VARIABLES
//*****************************************************************************

// Tracer variables
bool				running = true;
GLuint				maxDepth = 1;
GLuint				pboId;
GLuint				screenWidth = 800;
GLuint				screenHeight = 600;
GLuint				screenBitDepth = 32;

// OpenGL variables
GLuint				textures;
float*				buffer;

// SDL variables
SDL_Event			event;
SDL_Surface*		screen;

// FPS variables
int					fpsUpdateRate = 100; // FPS update rate (ms)
double				fpsCurrent = 0.0;
double				fpsAverage = 0.0;
int					framesSinceStart = 0;
int					framesSinceLast = 0;
int					timeStart = 0;
int					timeLast = 0;

//*****************************************************************************
// METHODS
//*****************************************************************************

//************************************
// Method:		initializeOpenGL - initialize OpenGL
// Returns:		true on success, false otherwise
//************************************
bool initializeOpenGL()
{
    // Initialize GLEW
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, 0);
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
    glBufferData(GL_PIXEL_UNPACK_BUFFER, screenWidth * screenHeight * 4 * sizeof(float), 0, GL_STREAM_DRAW); // Initialise PBO
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
        fprintf(stderr, "ERROR: Failed to initialize SDL: %s\n",SDL_GetError());
        return false;
    }

    // Set up the screen
    screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBitDepth, SDL_OPENGL);

    // Set the window caption
    SDL_WM_SetCaption("GPU Distributed Ray Tracer", NULL);

    if (screen == NULL)
    {
        fprintf(stderr, "ERROR: Could NOT set (%d x %d x %d) video mode: %s\n", screenWidth, screenHeight, screenBitDepth, SDL_GetError());
        return false;
    }

    fprintf(stderr, "INFO: SDL Initialization Complete\n");

    if (!initializeOpenGL())
    {
        fprintf(stderr, "ERROR: Failed to initialize OpenGL\n");
        return false;
    }

    fprintf(stderr, "INFO: OpenGL Initialization Complete\n");
    return true;

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
    fprintf(stderr, "\nINFO: Press [Enter] to continue...\n");
    std::cin.get();
}

//************************************
// Method:		handleKeyDown - handle the key which has been pressed
// Parameter:	SDLKey - the key to be handled
//************************************
void handleKeyDown(SDLKey key)
{
    switch (key)
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
    switch (event->type)
    {
    case SDL_QUIT:
        running = false;
        fprintf(stderr, "EVENT: SDL_QUIT\n");
        break;

    case SDL_KEYDOWN:
        fprintf(stderr, "EVENT: SDL_KEYDOWN\n");
        handleKeyDown(event->key.keysym.sym);
        break;
    }
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
// Method:		rayTrace - trace the scene and store the result in the specified buffer
// Parameter:	float* buffer - where to store the result of the trace
//************************************
void rayTrace(float* buffer)
{

}

//************************************
// Method:		initilizeTestScene - setup the Cornell Box test scene
// Returns:		a pointer to the scene
//************************************
Scene* initilizeTestScene()
{
	return NULL;
}

//************************************
// Method:		initializeTracer - setup the rayTracer
// Parameter:	screenWidth, screenHeight - screen dimensions
// Parameter:	Scene* scene - the scene to trace
//************************************
bool initializeTracer( GLuint screenWidth, GLuint screenHeight, Scene* scene ) 
{
	return true;
}

//************************************
// Method:		render - render the pixels given
//************************************
void render()
{
    // Bind PBO
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId);

    // Lock Buffer
    buffer = (float *) glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

    // Trace the scene
    rayTrace(buffer);

    // Unlock Buffer
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screenWidth, screenHeight, GL_RGBA, GL_FLOAT, 0);

    // Output Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);    glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);    glVertex2f( 1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f);    glVertex2f( 1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // Swap OpenGL frame buffers/Update Display
    SDL_GL_SwapBuffers();
}

//************************************
// Method:		main - program entry point
// Returns:		0 for success 1 otherwise
// Parameter:	int argc
// Parameter:	char* argv
//************************************
int main(int argc, char* argv[])
{
    if (!startup())
    {
        shutdown();
        waitForEnter();
        return -1;
    }

    // Save the start time for FPS calculations
    timeStart = timeLast = SDL_GetTicks();

    // Create Scene
    Scene* scene = initilizeTestScene();

    // tracer = new RayTracerCPU();

    if (initializeTracer(screenWidth, screenHeight, scene))
    {
        shutdown();
        waitForEnter();
        return -1;
    }

    // Main Loop
    while (running)
    {
        // Events
        while (SDL_PollEvent(&event))
            handleEvent(&event);

        // Update Stats
        updateStats();

        // Rendering
        render();
    }

    // waitForEnter();

    shutdown();

    return 0;
}