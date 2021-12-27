#pragma once

#include "Global.h"
#include "json.hpp"
#include "Sphere.h"
#include "Vec3.h"
#include <fstream>

using nlohmann::json;

class JSONSphere
{
public:
	JSONSphere(int count, int frames);
	~JSONSphere();
	void CalculateMovement();
	void CalculateColourChange();
	void CalculateRadiusChange();

	int sphereCount;
	int frameCount;
	Sphere* spheres;
	Vec3f* endPositions;
	Vec3f* movement;
	Vec3f* endColours;
	Vec3f* colourChange;
	float* endRadii;
	float* radiusChange;
};

class JSONReader
{
public:
	static JSONSphere* LoadSphere(const char* path);
};