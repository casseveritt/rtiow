#pragma once

#include "linear.h"

struct Ray
{
	typedef r3::Vec3f V;
	Ray() {}
	Ray( const V& origin, const V& direction, float fwd = 0.001f ) : dir( direction.Normalized() ), o( origin + dir * fwd ) {}

	V At( float t ) const
	{
		return o + dir * t;
	}
	V dir;
	V o;
};