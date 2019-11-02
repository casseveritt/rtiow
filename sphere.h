#pragma once

#include "hitable.h"
#include "quadratic.h"

struct Sphere : public Hitable
{
	typedef r3::Vec3f V;
	Sphere( const V& center, const float radius, Material* material, const V& vel = V(), float time0 = 0 )
		: c0( center ), r( radius ), mat( material ), dcdt( vel ), t0( time0 )
	{
	}

	bool Hits( const Ray& ray, float s_min, Hit& hit )
	{
		V c = c0 + dcdt * ( ray.t - t0 );
		V oc = ray.o - c;
		Quadraticf quadr( Dot( ray.dir, ray.dir ), 2 * Dot( ray.dir, oc ), Dot( oc, oc ) - r * r );
		float discr = quadr.Discriminant();
		if ( discr >= 0 )
		{
			float s = quadr.ClosestForwardSolution( discr, s_min );
			if ( s < s_min )
			{
				return false;
			}
			else
			{
				hit.s = s;
				hit.p = ray.At( s );
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

	V c0;
	float r;
	const Material* mat;
	V dcdt;
	float t0;
};
