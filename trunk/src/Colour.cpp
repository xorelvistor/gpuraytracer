#include "Colour.h"

Colour::Colour()
{
	this->red = static_cast<float>(0);
	this->green = static_cast<float>(0);
	this->blue = static_cast<float>(0);
}


Colour::Colour(double red, double green, double blue)
{
	setColour(red/255, green/255, blue/255);
}

Colour::Colour(float red, float green, float blue)
{
	setColour(red, green, blue);
}

Colour::Colour(int red, int green, int blue)
{
	setColour(red/255, green/255, blue/255);
}

void Colour::setColour(double red, double green, double blue)
{
	this->red = static_cast<float>(red);
	this->green = static_cast<float>(green);
	this->blue = static_cast<float>(blue);
}

void Colour::setColour(float red, float green, float blue)
{
	this->red = static_cast<float>(red);
	this->green = static_cast<float>(green);
	this->blue = static_cast<float>(blue);
}

void Colour::setColour(int red, int green, int blue)
{
	this->red = static_cast<float>(red);
	this->green = static_cast<float>(green);
	this->blue = static_cast<float>(blue);
}
