#include <SDL.h>
#include <SDL_opengl.h>

//************************************
// Method:    startup - initialise stuff
//************************************
void startup() 
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr,"SDL initialization failed: %s\n",SDL_GetError());
		SDL_Quit();
	}
	else
	{
		printf("SDL initialization complete\n");
	}
}

//************************************
// Method:    shutdown - clean up before exit
//************************************
void shutdown()
{
	// Shutdown SDL
	SDL_Quit();
}

//************************************
// Method:    main - program entry point
// Returns:   0 for success 1 otherwise
// Parameter: int argc
// Parameter: char * * arv
//************************************
int main(int argc, char ** arv)
{
	startup();
	
	system("pause");

	shutdown();

	return 0; 
}
