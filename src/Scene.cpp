#include "Scene.h"

Scene::Scene(GLint screenWidth, GLint screenHeight)
{
	this->camera = new Camera(screenWidth, screenHeight);
	camera->spXLeft = -4;
	camera->spXRight = 4;
	camera->spYTop = 3;
	camera->spYBottom = -3;
}

Scene::~Scene()
{

}