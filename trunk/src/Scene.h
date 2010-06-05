#pragma once

#include "Common.h"
#include "Camera.h"

class Scene
{
	public:
		Scene(GLint w, GLint h);
		~Scene();

		Camera* camera;

	protected:

	private:

};