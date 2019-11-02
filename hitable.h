#pragma once

#include "linear.h"
#include "ray.h"

#include <vector>

struct Material;

struct Hit
{
	typedef r3::Vec3f V;
	float s;
	V p;
	V n;
	const Material* mat;
};

struct Hitable
{
	virtual bool Hits( const Ray& ray, float s_min, Hit& hit ) = 0;
};

struct HitableCollection : public Hitable
{
	std::vector<Hitable*> hitables;

	bool Hits( const Ray& ray, float s_min, Hit& hit )
	{
		bool has_hit = false;
		Hit closest_hit;
		for ( auto hitable : hitables )
		{
			Hit h;
			if ( hitable->Hits( ray, s_min, h ) )
			{
				if ( has_hit == false )
				{
					closest_hit = h;
					has_hit = true;
					continue;
				}
				if ( h.s < closest_hit.s )
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