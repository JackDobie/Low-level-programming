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

	Sphere* spheres;
	int sphereCount;
	Vec3f* endPositions;
	Vec3f* movement;
	Vec3f* colourChange;
	Vec3f* startColours;
	Vec3f* endColours;
	int frameCount;
};

class JSONReader
{
public:
	static JSONSphere* LoadSphere(const char* path);
};