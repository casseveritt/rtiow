#pragma once

#include "ray.h"
#include <cmath>
#include <stdio.h>

struct Camera
{
	typedef r3::Vec2f V2;
	typedef r3::Vec3f V3;

	// symmetric fov
	Camera( float fovy_deg, float aspect )
	{
		ur.y = tan( 0.5f * fovy_deg * M_PI / 180.0 );
		ur.x = aspect * ur.y;
		ll = -ur;
		printf( "cam: ur = ( %.2f, %.2f )\n", ur.x, ur.y );
	}

	Ray GetRay( float u, float v )
	{
		V3 dir( r3::Lerp( ll.x, ur.x, u ), r3::Lerp( ll.y, ur.y, v ), -1 );
		return Ray( V3( 0, 0, 0 ), dir );
	}

	V2 ll;
	V2 ur;
};
