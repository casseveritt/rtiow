#pragma once

#include "linear.h"
#include "ray.h"

#include <limits>
#include <utility>

struct Aabb
{
	typedef r3::Vec3f V;

	V min;
	V max;
	static constexpr float inf = std::numeric_limits<float>::infinity();

	Aabb() : min( inf, inf, inf ), max( -min ) {}
	Aabb( const V& minCorner, const V& maxCorner ) : min( minCorner ), max( maxCorner ) {}

	V Center() const
	{
		return ( min + max ) * 0.5f;
	}

	bool Hits( const Ray& ray )
	{
		const V& o = ray.o;
		const V& d = ray.dir;
		float sx0 = ( min.x - o.x ) / d.x;
		float sx1 = ( max.x - o.x ) / d.x;
		if ( sx0 > sx1 )
		{
			std::swap( sx0, sx1 );
		}
		float sy0 = ( min.y - o.y ) / d.y;
		float sy1 = ( max.y - o.y ) / d.y;
		if ( sy0 > sy1 )
		{
			std::swap( sy0, sy1 );
		}
		float sz0 = ( min.z - o.z ) / d.z;
		float sz1 = ( max.z - o.z ) / d.z;
		if ( sz0 > sz1 )
		{
			std::swap( sz0, sz1 );
		}
		float s0 = std::max( sx0, std::max( sy0, sz0 ) );
		float s1 = std::min( sx1, std::min( sy1, sz1 ) );
		const float diff = s1 - s0;
		return ( s0 <= s1 ) || ( diff != diff ); // true if diff is NaN
	}
};

inline Aabb Union( const Aabb& b0, const Aabb& b1 )
{
	Aabb u;
	u.min = r3::Min( b0.min, b1.min );
	u.max = r3::Max( b0.max, b1.max );
	return u;
}
