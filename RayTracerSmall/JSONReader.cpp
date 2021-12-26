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
}

JSONSphere::~JSONSphere()
{
	delete(spheres);
	delete(endPositions);
	delete(movement);
	delete(endColours);
	delete(colourChange);
}

void JSONSphere::CalculateMovement()
{
	float multiplier = 0.01f;// 1 / frameCount;
	for (int i = 0; i < sphereCount; i++)
	{
		Vec3f dif = endPositions[i] - spheres[i].center; // get the vector from the sphere current position to the end position
		dif = dif.operator*(multiplier); // divide by the number of frames
		movement[i] = dif;
	}
}

void JSONSphere::CalculateColourChange()
{
	float multiplier = 0.01f;// 1 / frameCount;
	for (int i = 0; i < sphereCount; i++)
	{
		Vec3f dif = endColours[i] - spheres[i].surfaceColor; // get the vector from the sphere current position to the end position
		dif = dif.operator*(multiplier); // divide by the number of frames
		colourChange[i] = dif;
	}
}

JSONSphere* JSONReader::LoadSphere(const char* path)
{
	std::fstream file(path);
	std::stringstream msg;
	if (!file.good())
	{
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
		msg << "JSONReader: File does not contain 'sphereCount'" << std::endl;
		std::cout << msg.str();
		return nullptr;
	}

	if (j.contains("frameCount"))
	{
		frameCount = j["frameCount"];
	}
	else
	{
		msg << "JSONReader: File does not contain 'frameCount'" << std::endl;
		std::cout << msg.str();
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

		/*if (sphere.contains("surfaceColor"))
		{
			std::vector<float> surfaceCol = sphere["surfaceColor"];
			sphereInfo->spheres[i].surfaceColor = Vec3f(surfaceCol[0], surfaceCol[1], surfaceCol[2]);
		}
		else
			failed = true;*/

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

		if (sphere.contains("startColour"))
		{
			std::vector<float> startColour = sphere["startColour"];
			sphereInfo->spheres[i].surfaceColor = Vec3f(startColour[0], startColour[1], startColour[2]);
			//sphereInfo->startColours[i] = Vec3f(startColour[0], startColour[1], startColour[2]);
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

		if (failed)
		{
			msg << "JSONReader: Values missing from file!" << std::endl;
			std::cout << msg.str();
			return nullptr;
		}
	}

	sphereInfo->CalculateMovement();
	sphereInfo->CalculateColourChange();
	return sphereInfo;
}
