#include "JSONReader.h"
#include <sstream>

JSONSphere::JSONSphere(int count, int frames)
{
	sphereCount = count;
	frameCount = frames;
	spheres = new Sphere[sphereCount];
	endPositions = new Vec3f[sphereCount];
	movement = new Vec3f[sphereCount];
	endColours = new Vec3f[sphereCount];
	colourChange = new Vec3f[sphereCount];
	endRadii = new float[sphereCount];
	radiusChange = new float[sphereCount];
}

JSONSphere::~JSONSphere()
{
	delete(spheres);
	delete(endPositions);
	delete(movement);
	delete(endColours);
	delete(colourChange);
	delete(endRadii);
	delete(radiusChange);
}

void JSONSphere::CalculateMovement()
{
	float multiplier = 1 / (float)frameCount;
	for (int i = 0; i < sphereCount; i++)
	{
		Vec3f dif = endPositions[i] - spheres[i].center; // get the vector from the sphere current position to the end position
		dif = dif.operator*(multiplier); // divide by the number of frames
		movement[i] = dif;
	}
}

void JSONSphere::CalculateColourChange()
{
	float multiplier = 1 / (float)frameCount;
	for (int i = 0; i < sphereCount; i++)
	{
		Vec3f dif = endColours[i] - spheres[i].surfaceColor;
		dif = dif.operator*(multiplier);
		colourChange[i] = dif;
	}
}

void JSONSphere::CalculateRadiusChange()
{
	float multiplier = 1 / (float)frameCount;
	for (int i = 0; i < sphereCount; i++)
	{
		float dif = endRadii[i] - spheres[i].radius;
		dif = dif * multiplier;
		radiusChange[i] = dif;
	}
}

JSONSphere* JSONReader::LoadSphere(const char* path)
{
	std::fstream file(path);
	if (!file.good())
	{
		std::stringstream msg;
		msg << "Unable to load file: " << path << std::endl;
		std::cout << msg.str();
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
		std::cout << "JSONReader: File does not contain 'sphereCount'\n";
		return nullptr;
	}

	if (j.contains("frameCount"))
	{
		frameCount = j["frameCount"];
	}
	else
	{
		std::cout << "JSONReader: File does not contain 'frameCount'\n";
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

		if (sphere.contains("startRadius"))
		{
			float radius = sphere["startRadius"];
			sphereInfo->spheres[i].radius = radius;
			sphereInfo->spheres[i].radius2 = radius * radius;
		}
		else
			failed = true;

		if (sphere.contains("endRadius"))
		{
			float radius = sphere["endRadius"];
			sphereInfo->endRadii[i] = radius;
		}
		else
		{
			sphereInfo->endRadii[i] = sphereInfo->spheres[i].radius;
		}

		if (sphere.contains("reflection"))
		{
			float reflection = sphere["reflection"];
			sphereInfo->spheres[i].reflection = reflection;
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

		if (sphere.contains("endColour"))
		{
			std::vector<float> endColour = sphere["endColour"];
			sphereInfo->endColours[i] = Vec3f(endColour[0], endColour[1], endColour[2]);
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
			std::cout << "JSONReader: Values missing from file!\n";
			delete sphereInfo;
			return nullptr;
		}
	}

	sphereInfo->CalculateMovement();
	sphereInfo->CalculateColourChange();
	sphereInfo->CalculateRadiusChange();
	return sphereInfo;
}
