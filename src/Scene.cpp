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
void Scene::initScene1()
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
	primitives[1]->getMaterial()->setSpecular( 0.4f );
	primitives[1]->getMaterial()->setColour( Colour( 0.7f, 0.7f, 0.7f ) );

	// Small Sphere
	primitives[2] = new Sphere( Vector3( -5.5f, -0.5, 7 ), 2 );
	primitives[2]->setName( "Small Sphere" );
	primitives[2]->getMaterial()->setReflection( 1.0f );
	primitives[2]->getMaterial()->setDiffuse( 0.1f );
	primitives[2]->getMaterial()->setSpecular( 0.9f );
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

	// set Number of Primitives
	countPrimitives = 5;
}

void Scene::initScene2()
{
	primitives[0] = new Plane( Vector3( 0, 1, 0 ), 4.4f );
	primitives[0]->setName( "Ground Plane" );
	primitives[0]->getMaterial()->setReflection( 0.0f );
	primitives[0]->getMaterial()->setRefraction( 0.0f );
	primitives[0]->getMaterial()->setDiffuse( 1.0f );
	primitives[0]->getMaterial()->setColour( Colour( 0.4f, 0.3f, 0.3f ) );

	primitives[1] = new Sphere( Vector3( 2, 0.8f, 3 ), 2.5f );
	primitives[1]->setName( "big sphere" );
	primitives[1]->getMaterial()->setReflection( 0.2f );
	primitives[1]->getMaterial()->setRefraction( 0.8f );
	primitives[1]->getMaterial()->setRefractiveIndex( 1.3f );
	primitives[1]->getMaterial()->setColour( Colour( 0.7f, 0.7f, 1.0f ) );

	primitives[2] = new Sphere( Vector3( -5.5f, -0.5, 7 ), 2 );
	primitives[2]->setName( "small sphere" );
	primitives[2]->getMaterial()->setReflection( 0.5f );
	primitives[2]->getMaterial()->setRefraction( 0.0f );
	primitives[2]->getMaterial()->setRefractiveIndex( 1.3f );
	primitives[2]->getMaterial()->setDiffuse( 0.1f );
	primitives[2]->getMaterial()->setColour( Colour( 0.7f, 0.7f, 1.0f ) );

	primitives[3] = new Sphere( Vector3( 0, 5, 5 ), 0.1f );
	primitives[3]->setLight( true );
	primitives[3]->getMaterial()->setColour( Colour( 0.4f, 0.4f, 0.4f ) );

	primitives[4] = new Sphere( Vector3( -3, 5, 1 ), 0.1f );
	primitives[4]->setLight( true );
	primitives[4]->getMaterial()->setColour( Colour( 0.6f, 0.6f, 0.8f ) );

	primitives[5] = new Sphere( Vector3( -1.5f, -3.8f, 1 ), 1.5f );
	primitives[5]->setName( "extra sphere" );
	primitives[5]->getMaterial()->setReflection( 0.0f );
	primitives[5]->getMaterial()->setRefraction( 0.8f );
	primitives[5]->getMaterial()->setColour( Colour( 1.0f, 0.4f, 0.4f ) );

	primitives[6] = new Plane( Vector3( 0.4f, 0, -1 ), 12 );
	primitives[6]->setName( "Back Plane" );
	primitives[6]->getMaterial()->setReflection( 0.0f );
	primitives[6]->getMaterial()->setRefraction( 0.0f );
	primitives[6]->getMaterial()->setSpecular( 0 );
	primitives[6]->getMaterial()->setDiffuse( 0.6f );
	primitives[6]->getMaterial()->setColour( Colour( 0.5f, 0.3f, 0.5f ) );

	primitives[7] = new Plane( Vector3( 0, -1, 0 ), 7.4f );
	primitives[7]->setName( "Back Plane" );
	primitives[7]->getMaterial()->setReflection( 0.0f );
	primitives[7]->getMaterial()->setRefraction( 0.0f );
	primitives[7]->getMaterial()->setSpecular( 0 );
	primitives[7]->getMaterial()->setDiffuse( 0.5f );
	primitives[7]->getMaterial()->setColour( Colour( 0.4f, 0.7f, 0.7f ) );

	int prim = 8;
	for ( int x = 0; x < 8; x++ ) for ( int y = 0; y < 7; y++ )
	{
		primitives[prim] = new Sphere( Vector3( -4.5f + x * 1.5f, -4.3f + y * 1.5f, 10 ), 0.3f );
		primitives[prim]->setName( "grid sphere" );
		primitives[prim]->getMaterial()->setReflection( 0 );
		primitives[prim]->getMaterial()->setRefraction( 0 );
		primitives[prim]->getMaterial()->setSpecular( 0.6f );
		primitives[prim]->getMaterial()->setDiffuse( 0.6f );
		primitives[prim]->getMaterial()->setColour( Colour( 0.3f, 1.0f, 0.4f ) );
		prim++;
	}
	countPrimitives = prim;
};

void Scene::initSceneCornellBox()
{
	primitives[0] = new Plane( Vector3(0, 1, 0), 4.4f );
	primitives[0]->setName( "Ground Plane" );
	primitives[0]->getMaterial()->setReflection( 0 );
	primitives[0]->getMaterial()->setDiffuse( 0.2f );
	primitives[0]->getMaterial()->setSpecular( 0.5f );
	primitives[0]->getMaterial()->setColour( Colour( 1.0f, 1.0f, 1.0f ) );

	primitives[1] = new Plane( Vector3(0, -1, 0), 4.4f );
	primitives[1]->setName( "Ceiling Plane" );
	primitives[1]->getMaterial()->setReflection( 0 );
	primitives[1]->getMaterial()->setDiffuse( 0.2f );
	primitives[1]->getMaterial()->setSpecular( 0.5f );
	primitives[1]->getMaterial()->setColour( Colour( 1.0f, 1.0f, 1.0f ) );

	primitives[2] = new Plane( Vector3(0, 0, -1), 7.4f );
	primitives[2]->setName( "Back Plane" );
	primitives[2]->getMaterial()->setReflection( 0 );
	primitives[2]->getMaterial()->setDiffuse( 0.2f );
	primitives[2]->getMaterial()->setSpecular( 0.5f );
	primitives[2]->getMaterial()->setColour( Colour( 1.0f, 1.0f, 1.0f ) );

	primitives[3] = new Plane( Vector3(0, 0, 1), 7.4f );
	primitives[3]->setName( "Front Plane" );
	primitives[3]->getMaterial()->setReflection( 0 );
	primitives[3]->getMaterial()->setDiffuse( 0.2f );
	primitives[3]->getMaterial()->setSpecular( 0.5f );
	primitives[3]->getMaterial()->setColour( Colour( 0.0f, 0.0f, 0.0f ) );

	primitives[4] = new Plane( Vector3(1, 0, 0), 4.4f );
	primitives[4]->setName( "Left Plane" );
	primitives[4]->getMaterial()->setReflection( 0 );
	primitives[4]->getMaterial()->setDiffuse( 0.5f );
	primitives[4]->getMaterial()->setSpecular( 1.0f );
	primitives[4]->getMaterial()->setColour( Colour( 1.0f, 0.0f, 0.0f ) );

	primitives[5] = new Plane( Vector3(-1, 0, 0), 4.4f );
	primitives[5]->setName( "Right Plane" );
	primitives[5]->getMaterial()->setReflection( 0 );
	primitives[5]->getMaterial()->setDiffuse( 1.0f );
	primitives[5]->getMaterial()->setSpecular( 0.5f );
	primitives[5]->getMaterial()->setColour( Colour( 0.0f, 0.0f, 1.0f ) );

	primitives[6] = new Sphere( Vector3(-2, -2.8f, 4), 1.25f);
	primitives[6]->setName( "Mirror Sphere");
	primitives[6]->getMaterial()->setReflection( 0.5f );
	primitives[6]->getMaterial()->setSpecular( 0.7f );
	primitives[6]->getMaterial()->setDiffuse( 0.5f );
	primitives[6]->getMaterial()->setColour( Colour( 0.75f, 0.75f, 0.75f ) );
	
	primitives[7] = new Sphere( Vector3(2, -2.8f, 3), 1.25f);
	primitives[7]->setName( "Diffuse Sphere");
	primitives[7]->getMaterial()->setReflection( 0.0f );
	primitives[7]->getMaterial()->setRefraction( 0.0f );
	primitives[7]->getMaterial()->setRefractiveIndex( 1.0f );
	primitives[7]->getMaterial()->setDiffuse( 1.0f );
	primitives[7]->getMaterial()->setColour( Colour( 0.0f, 1.0f, 0.0f ) );

	// Light Source
	primitives[8] = new Sphere( Vector3( 0, 2.2f, 0 ), 0.05f );
	primitives[8]->setLight( true );
	primitives[8]->getMaterial()->setColour( Colour( 1.0f, 1.0f, 1.0f ) );

	countPrimitives = 9;
}

void Scene::initSimple()
{
	primitives[0] = new Sphere( Vector3(-2, -2.8f, 4), 1.25f);
	primitives[0]->setName( "Mirror Sphere");
	primitives[0]->getMaterial()->setSpecular( 0.7f );
	primitives[0]->getMaterial()->setReflection( 0.4f );
	primitives[0]->getMaterial()->setRefraction( 0.5f );
	primitives[0]->getMaterial()->setRefractiveIndex( 1.33f );
	primitives[0]->getMaterial()->setAbsorptionCoefficient( 0.15f );
	primitives[0]->getMaterial()->setColour( Colour( 0.0f, 1.0f, 0.0f ) );

	primitives[1] = new Sphere( Vector3(2, -2.8f, 3), 1.25f);
	primitives[1]->setName( "Mirror Sphere");
	primitives[1]->getMaterial()->setSpecular( 0.5f );
	primitives[1]->getMaterial()->setReflection( 0.4f );
	primitives[1]->getMaterial()->setRefraction( 0.5f );
	primitives[1]->getMaterial()->setRefractiveIndex( 1.33f );
	primitives[1]->getMaterial()->setAbsorptionCoefficient( 0.15f );
	primitives[1]->getMaterial()->setColour( Colour( 0.0f, 1.0f, 1.0f ) );

	// Light Source
	primitives[2] = new Sphere( Vector3( 0, 2.2f, 0 ), 0.05f );
	primitives[2]->setLight( true );
	primitives[2]->getMaterial()->setColour( Colour( 1.0f, 1.0f, 1.0f ) );

	countPrimitives = 3;
}