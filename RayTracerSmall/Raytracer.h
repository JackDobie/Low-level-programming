#pragma once

#include "Global.h"
#include "Sphere.h"
#include "Vec3.h"
#include "JSONReader.h"
#include <string>
#include <chrono>
#include <mutex>
#include <thread>

using std::string;

#if defined __linux__ || defined __APPLE__
// "Compiled for Linux
#else
// Windows doesn't define these values by default, Linux does
#define M_PI 3.141592653589793
#define INFINITY 1e8
#endif

// This variable controls the maximum recursion depth
#define MAX_RAY_DEPTH 5

class Raytracer
{
public:
	Raytracer();
	Raytracer(const char* jsonpath);
	~Raytracer();
	float mix(const float& a, const float& b, const float& mix);
	Vec3f Trace(const Vec3f& rayorig, const Vec3f& raydir, const std::vector<Sphere>& spheres, const int& depth);
	void Render(const std::vector<Sphere>& spheres, int iteration);
	void BasicRender();
	void SimpleShrinking();
	void SmoothScaling(int r);
	void SmoothScalingThreaded();
	void JSONRender(int iteration);
	void JSONRenderThreaded();

	JSONSphere* GetJSON() { return json; }
	void SetJSON(JSONSphere* j) { json = j; }
private:
	JSONSphere* json;
	std::mutex* wait;

	unsigned width;
	unsigned height;
	float invWidth;
	float invHeight;
	float fov;
	float aspectratio;
	float angle;
};