#pragma once

#include "hitable.h"
#include "quadratic.h"

struct Sphere : public Hitable
{
	typedef r3::Vec3f V;
	Sphere( const V& center, const float radius, Material* material ) : c( center ), r( radius ), mat( material ) {}

	bool Hits( const Ray& ray, float t_min, Hit& hit )
	{
		Quadraticf quadr( Dot( ray.dir, ray.dir ), 2 * Dot( ray.dir, ray.o - c ), Dot( ray.o - c, ray.o - c ) - r * r );
		float discr = quadr.Discriminant();
		if ( discr >= 0 )
		{
			float t = quadr.ClosestForwardSolution( discr, t_min );
			if ( t < t_min )
			{
				return false;
			}
			else
			{
				hit.t = t;
				hit.p = ray.At( t );
				hit.n = ( hit.p - c ) / r;
				hit.mat = mat;
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	V c;
	float r;
	const Material* mat;
};
