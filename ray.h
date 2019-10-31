#pragma once

#include "linear.h"
#include <stdio.h>

extern int trace_recursion;

struct Ray
{
	typedef r3::Vec3f V;
	Ray() {}
	Ray( const V& origin, const V& direction, float fwd = 0.0f ) : dir( direction.Normalized() ), o( origin + dir * fwd )
	{
		if ( trace_recursion > 100 )
		{
			printf( "%d ray: o = ( %.2f, %.2f, %.2f ), dir = ( %.2f, %.2f, %.2f )\n", trace_recursion, o.x, o.y, o.z, dir.x, dir.y, dir.z );
		}
	}

	V At( float t ) const
	{
		return o + dir * t;
	}
	V dir;
	V o;
};
