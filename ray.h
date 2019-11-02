#pragma once

#include "linear.h"
#include <stdio.h>

extern int trace_recursion;

struct Ray
{
	typedef r3::Vec3f V;

	Ray() : t( 0 ) {}

	Ray( const V& origin, const V& direction, float time = 0 ) : dir( direction.Normalized() ), o( origin ), t( time ) {}

	V At( float s ) const
	{
		return o + dir * s;
	}
	V dir;
	V o;
	float t;
};
