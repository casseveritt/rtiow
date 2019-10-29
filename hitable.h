#pragma once

#include "linear.h"
#include "ray.h"

struct Hit
{
	typedef r3::Vec3f V;
	float t;
	V p;
	V n;
};

struct Hitable
{
	virtual bool Hits( const Ray& ray, float t_min, float t_max, Hit* hit ) = 0;
};