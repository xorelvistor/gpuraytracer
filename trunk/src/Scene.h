#pragma once

#include "Common.h"
#include "Camera.h"

class Material
{
	public:
		Material() : colour(0.5f, 0.5f, 0.5f), reflection(0), diffuse(0.2f){};

		void		setColour(Colour colour){ this->colour = colour; };
		void		setDiffuse(float diffuse){ this->diffuse = diffuse; };
		void		setReflection(float reflection){ this->reflection = reflection; };

		Colour		getColour(){ return colour; };
		float		getDiffuse(){ return diffuse; };
		float		getReflection(){ return reflection; };

	protected:

	private:
		Colour		colour;
		float		reflection;
		float		diffuse;
};

class Primitive
{
	public:
		Primitive() : name(0), light(false) {};
		
		Material*	getMaterial(){return &material;};
		void		setMaterial(Material m){this->material = m;};

		void		setLight(bool l){this->light = l;};
		bool		isLight(){return light;};

		void		setName(char* name){this->name = name;};
		char*		getName(){return name;};

virtual int			intersect(Ray& aRay, float& aDistance) = 0;
virtual Vector3		getNormal(Vector3& position) = 0;

	protected:
		bool		light;
		Material	material;
		char*		name;

	private:

};

class Sphere : public Primitive
{
	public:
		Sphere(Vector3& c, float r) : centre(c), radius(r), sqRadius(r*r), rRadius(1.0f/r){};

		Vector3		getCentre(){ return centre; };
		float		getSqRadius(){ return sqRadius; };
		int			intersect(Ray& aRay, float& aDistance);
		Vector3		getNormal(Vector3& position){ return (position - centre) * rRadius; };

	protected:

	private:
		Vector3		centre;
		float		sqRadius, radius, rRadius;
		
};

class Plane : public Primitive
{
	public:
		Plane() : normal(0,0,0), distance(0){};
		Plane(Vector3& n, float d) : normal(n), distance(d){};

		float		getDistance(){ return distance; };
		Vector3		getNormal(){ return normal; };
		int			intersect(Ray& aRay, float& aDistance);
		Vector3		getNormal(Vector3& position){ return normal; };

	protected:

	private:
		float distance;
		Vector3 normal;
};

class Scene
{
	public:
		Scene(GLint w, GLint h);
		~Scene();

		Camera*		getCamera(){ return camera; };
		int			getPrimitivesCount(){ return countPrimitives; };
		Primitive*	getPrimitive(int index){ return primitives[index]; };
		void		InitScene();

	protected:

	private:
		Camera*		camera;
		int			countPrimitives;
		Primitive**	primitives;

};