#include "JSONReader.h"

JSONSphere::JSONSphere(int count, int frames)
{
	sphereCount = count;
	frameCount = frames;
	spheres = new Sphere[sphereCount];
	endPositions = new Vec3f[sphereCount];
	movement = new Vec3f[sphereCount];
}

JSONSphere::~JSONSphere()
{
	delete spheres;
	delete endPositions;
	delete movement;
}

void JSONSphere::CalculateMovement()
{
	float multiplier = 1 / frameCount;
	for (int i = 0; i < sphereCount; i++)
	{
		Vec3f dif = endPositions[i] - spheres[i].center; // get the vector from the sphere current position to the end position
		dif.operator*(multiplier); // divide by the number of frames
		movement[i] = dif;
	}
}

JSONSphere* JSONReader::LoadSphere(const char* path)
{
	std::fstream file(path);
	if (!file.good())
	{
		std::cout << "Unable to load file: " << path << std::endl;
		return nullptr;
	}
	json j;
	file >> j;
	int sphereCount = 0;
	int frameCount = 0;
	if (j.contains("sphereCount"))
	{
		sphereCount = j["sphereCount"];
	}
	else
	{
		std::cout << "JSONReader: File does not contain 'sphereCount'" << std::endl;
		return nullptr;
	}

	if (j.contains("frameCount"))
	{
		frameCount = j["frameCount"];
	}
	else
	{
		std::cout << "JSONReader: File does not contain 'frameCount'" << std::endl;
		return nullptr;
	}
	
	JSONSphere* sphereInfo = new JSONSphere(sphereCount, frameCount);
	json spheres = j["spheres"];
	for (int i = 0; i < sphereCount; i++)
	{
		json sphere = spheres[i];
		bool failed = false;
		if (sphere.contains("startPos"))
		{
			std::vector<float> start = sphere["startPos"];
			sphereInfo->spheres[i].center = Vec3f(start[0], start[1], start[2]);
		}
		else
			failed = true;

		if (sphere.contains("endPos"))
		{
			std::vector<float> end = sphere["endPos"];
			sphereInfo->endPositions[i] = Vec3f(end[0], end[1], end[2]);
		}
		else
			failed = true;

		if (sphere.contains("surfaceColor"))
		{
			std::vector<float> surfaceCol = sphere["surfaceColor"];
			sphereInfo->spheres[i].surfaceColor = Vec3f(surfaceCol[0], surfaceCol[1], surfaceCol[2]);
		}
		else
			failed = true;

		if (sphere.contains("radius"))
		{
			float radius = sphere["radius"];
			sphereInfo->spheres[i].radius = radius;
			sphereInfo->spheres[i].radius2 = radius * radius;
		}
		else
			failed = true;

		if (sphere.contains("reflection"))
		{
			float reflection = sphere["reflection"];
			sphereInfo->spheres[i].reflection = reflection;
		}
		else
			failed = true;

		if (sphere.contains("transparency"))
		{
			float transparency = sphere["transparency"];
			sphereInfo->spheres[i].transparency = transparency;
		}
		else
			failed = true;

		if (failed)
		{
			std::cout << "JSONReader: Values missing from file!" << std::endl;
			return nullptr;
		}
	}

	sphereInfo->CalculateMovement();
	return sphereInfo;
}
