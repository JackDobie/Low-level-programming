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

	Sphere* spheres;
	int sphereCount;
	Vec3f* endPositions;
	Vec3f* movement;
	int frameCount;
};

class JSONReader
{
public:
	static JSONSphere* LoadSphere(const char* path);
};