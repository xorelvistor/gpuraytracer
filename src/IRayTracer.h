#pragma once

#include "Common.h"
#include "Scene.h"

// Intersection values
#define HIT 1
#define MISS 0
#define INSIDE -1

class IRayTracer
{
	public:
		void setScene(Scene* scene){ this->scene = scene; };
		void setScreenSize(GLuint w, GLuint h){ this->screenWidth = w; this->screenHeight = h; };
		virtual bool initialize() = 0;
		virtual void rayTrace(float* buffer){};

	protected:
		IRayTracer(){};
		~IRayTracer(){};

		Scene*	scene;
		GLuint screenWidth;
		GLuint screenHeight;

	private:
};