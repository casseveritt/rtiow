#include "linear.h"

struct Ray
{
	typedef r3::Vec3f V;
	Ray() {}
	Ray( const V& origin, const V& direction ) : o( origin ), dir( direction ) {}

	V At( float t ) const
	{
		return o + dir * t;
	}
	V o;
	V dir;
};