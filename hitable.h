#pragma once

#include "linear.h"
#include "ray.h"

#include <vector>

struct Material;

struct Hit
{
	typedef r3::Vec3f V;
	float t;
	V p;
	V n;
	const Material* mat;
};

struct Hitable
{
	virtual bool Hits( const Ray& ray, Hit& hit ) = 0;
};

struct HitableCollection : public Hitable
{
	std::vector<Hitable*> hitables;

	bool Hits( const Ray& ray, Hit& hit )
	{
		bool has_hit = false;
		Hit closest_hit;
		for ( auto hitable : hitables )
		{
			Hit h;
			if ( hitable->Hits( ray, h ) )
			{
				if ( has_hit == false )
				{
					closest_hit = h;
					has_hit = true;
					continue;
				}
				if ( h.t < closest_hit.t )
				{
					closest_hit = h;
				}
			}
		}
		if ( has_hit )
		{
			hit = closest_hit;
		}
		return has_hit;
	}
};