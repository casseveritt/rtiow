#pragma once

#include "hitable.h"

struct Material
{
	typedef r3::Vec3f V;
	virtual bool Scatter( const Ray& incident, const Hit& hit, V& attenuation, Ray& scattered ) const = 0;
};