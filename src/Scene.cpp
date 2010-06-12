#include "Scene.h"

Scene::Scene(GLint screenWidth, GLint screenHeight)
{
	this->camera = new Camera(screenWidth, screenHeight);
	camera->spXLeft = -4;
	camera->spXRight = 4;
	camera->spYTop = 3;
	camera->spYBottom = -3;

	this->countPrimitives = 0;
	this->primitives = new Primitive* [100];
}

Scene::~Scene()
{

}

int Plane::intersect(Ray& aRay, float& aDistance)
{
	float d = DOT(normal, aRay.direction);
	if (d != 0)
	{
		float dist = -( DOT(normal, aRay.position) + this->distance ) / d;
		if (dist > 0)
		{
			if (dist < aDistance)
			{
				aDistance = dist;
				return HIT;
			}
		}
	}
	return MISS;
}

int Sphere::intersect(Ray& aRay, float& aDistance)
{
	Vector3 v = aRay.position - centre;

	float b = -DOT(v, aRay.direction);
	float det = (b * b) - DOT(v, v) + sqRadius;
	int returnValue = MISS;

	if (det > 0)
	{
		det = sqrtf( det );
		float i1 = b - det;
		float i2 = b + det;
		if (i2 > 0)
		{
			if (i1 < 0)
			{
				if (i2 < aDistance)
				{
					aDistance = i2;
					returnValue = INSIDE;
				}
			}
			else
			{
				if (i1 < aDistance)
				{
					aDistance = i1;
					returnValue = HIT;
				}
			}
		}
	}

	return returnValue;
}
void Scene::InitScene()
{
	// Ground Plane
	primitives[0] = new Plane( Vector3(0, 1, 0), 4.4f );
	primitives[0]->setName( "Ground Plane" );
	primitives[0]->getMaterial()->setReflection( 0 );
	primitives[0]->getMaterial()->setDiffuse( 1.0f );
	primitives[0]->getMaterial()->setColour( Colour( 0.4f, 0.3f, 0.3f ) );

	// Big Sphere
	primitives[1] = new Sphere( Vector3( 1, -0.8f, 3 ), 2.5f );
	primitives[1]->setName( "Big Sphere" );
	primitives[1]->getMaterial()->setReflection( 0.6f );
	primitives[1]->getMaterial()->setColour( Colour( 0.7f, 0.7f, 0.7f ) );

	// Small Sphere
	primitives[2] = new Sphere( Vector3( -5.5f, -0.5, 7 ), 2 );
	primitives[2]->setName( "Small Sphere" );
	primitives[2]->getMaterial()->setReflection( 1.0f );
	primitives[2]->getMaterial()->setDiffuse( 0.1f );
	primitives[2]->getMaterial()->setColour( Colour( 0.7f, 0.7f, 1.0f ) );

	// Light 1
	primitives[3] = new Sphere( Vector3( 0, 5, 5 ), 0.1f );
	primitives[3]->setName( "Light 1" );
	primitives[3]->setLight( true );
	primitives[3]->getMaterial()->setColour( Colour( 0.6f, 0.6f, 0.6f ) );

	// Light 2
	primitives[4] = new Sphere( Vector3( 2, 5, 1 ), 0.1f );
	primitives[4]->setName( "Light 2" );
	primitives[4]->setLight( true );
	primitives[4]->getMaterial()->setColour( Colour( 0.7f, 0.7f, 0.9f ) );

	// Set Number of Primitives
	countPrimitives = 5;

}