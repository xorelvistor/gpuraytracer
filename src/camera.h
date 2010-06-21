#pragma once

#include "Common.h"

class Camera
{
	public:
		Camera() : screenWidth(0), screenHeight(0){};
		Camera(GLint w, GLint h) : screenWidth(w), screenHeight(h){};
		~Camera(){};
		Vector3		position;
		GLint		screenWidth;
		GLint		screenHeight;
		float		spXLeft, spXRight, spYTop, spYBottom;

	protected:

	private:

};