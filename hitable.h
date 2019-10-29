#pragma once

#include "linear.h"
#include "ray.h"

#include <vector>

struct Hit
{
	typedef r3::Vec3f V;
	float t;
	V p;
	V n;
};

struct Hitable
{
	virtual bool Hits( const Ray & ray, float t_min, float t_max, Hit * hit ) = 0;
};


struct HitableCollection : public Hitable
{
	std::vector<Hitable *> hitables;

	bool Hits( const Ray & ray, float t_min, float t_max, Hit * hit )
	{
		bool has_hit = false;
		Hit closest_hit;
		for( auto hitable : hitables )
		{
			Hit h;
			if ( hitable->Hits( ray, t_min, t_max, &h ) )
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
		if ( hit && has_hit )
		{
			*hit = closest_hit;
		}
		return has_hit;
	}

};