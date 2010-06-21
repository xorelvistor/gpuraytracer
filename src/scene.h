#pragma once

#include "Common.h"
#include "Camera.h"

class Material
{
	public:
		Material() :colour(0.5f, 0.5f, 0.5f),
					reflection(0),
					diffuse(0.2f),
					absorptionCoefficient(0),
					refraction(0),
					refractionIndex(1.0f){};

		void		setAbsorptionCoefficient(float absorptionCoefficient){ this->absorptionCoefficient = absorptionCoefficient; };
		void		setColour(Colour colour){ this->colour = colour; };
		void		setDiffuse(float diffuse){ this->diffuse = diffuse; };
		void		setReflection(float reflection){ this->reflection = reflection; };
		void		setRefraction(float refraction){ this->refraction = refraction; };
		void		setRefractiveIndex(float refractionIndex){ this->refractionIndex = refractionIndex; };
		void		setSpecular(float specular){ this->specular = specular; };

		float		getAbsorptionCoefficient(){ return absorptionCoefficient; };
		Colour		getColour(){ return colour; };
		float		getDiffuse(){ return diffuse; };
		float		getReflection(){ return reflection; };
		float		getRefraction(){ return refraction; };
		float		getRefractiveIndex(){ return refractionIndex; };
		float		getSpecular(){ return specular; };

	protected:

	private:
		float		absorptionCoefficient;
		Colour		colour;
		float		diffuse;
		float		reflection;
		float		refraction;
		float		refractionIndex;
		float		specular;
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
virtual int			getType() = 0;

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
		int			getType(){ return SPHERE; };

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
		int			getType(){ return PLANE; };

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
		void		initScene1();
		void		initScene2();
		void		initSceneCornellBox();
		void		initSimple();

	protected:

	private:
		Camera*		camera;
		int			countPrimitives;
		Primitive**	primitives;

};