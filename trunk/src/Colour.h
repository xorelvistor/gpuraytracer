#pragma once

class Colour
{
public:

	Colour();
	Colour(double red, double green, double blue);        
	Colour(float red, float green, float blue);
	Colour(int red, int green, int blue);
	~Colour(){};

	float getRed() const { return red; }
	void setRed(float val) { red = val; }

	float getGreen() const { return green; }
	void setGreen(float val) { red = val; }

	float getBlue() const { return blue; }
	void setBlue(float val) { red = val; }

	void setColour(double red, double green, double blue);        
	void setColour(float red, float green, float blue);
	void setColour(int red, int green, int blue);

protected:

private:
	float red;
	float green;
	float blue;

};