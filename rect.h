#pragma once

#include "hitable.h"

// Rect is defined in the xy plane and transformed into
// the world using a rigid body pose transform.

struct Rect : public Hitable
{
	typedef r3::Vec3f V;
	typedef r3::Vec2f V2;
	typedef r3::Posef Pose;

	Rect( const V2& mins, const V2& maxs, const Pose& pose, Material * material )
	: min( mins )
	, max( maxs )
	, p( pose )
	, ip( pose.Inverted() )
	, mat( material )
	{
		n = p.r.Rotate( V( 0, 0, 1 ) );
		V b[4] = {
			V( min.x, min.y, 0 ),
			V( max.x, min.y, 0 ),
			V( min.x, max.y, 0 ),
			V( max.x, max.y, 0 )
		};
		for ( int i = 0; i < 4; i++ )
		{
			b[ i ] = p.Transform( b[ i ] );
		}
		V bmin = b[ 0 ];
		V bmax = b[ 0 ];
		for ( int i = 1; i < 4; i++ )
		{
			bmin = r3::Min( bmin, b[ i ] );
			bmax = r3::Max( bmax, b[ i ] );
		}
		box = Aabb( bmin, bmax );
		V2 dim = max - min;
		rcpdim = V2( 1 / dim.x, 1 / dim.y );
	}

	bool Hits( const Ray& ray, float s_min, Hit& hit )
	{
		Ray rr;
		rr.o = ip.Transform( ray.o );

		// Ray origin must be in +z half space to intersect plane
		if ( rr.o.z < 0 )
		{
			return false;
		}
		rr.dir = ip.r.Rotate( ray.dir );

		// If the ray isn't pointing toward the plane, it won't intersect
		if ( rr.dir.z >= 0 )
		{
			return false;
		}

		// Project to the z=0 plane
		const float s = -rr.o.z / rr.dir.z;
		const V hp = rr.At( s );

		// If the hit point for the plane is outside the rect bounds, no hit
		if ( hp.x < min.x || max.x < hp.x || hp.y < min.y || max.y < hp.y )
		{
			return false;
		}

		hit.s = s;
		hit.p = ray.At( s );
		hit.n = n;
		V2 dim = max - min;
		hit.uv = ( V2( hp.x, hp.y ) - min ) * rcpdim;
		hit.mat = mat;
		return true;
	}

	Aabb GetBounds( float time0 = 0, float time1 = 0 ) const
	{
		return box;
	}

	V2 min;
	V2 max;
	Pose p;
	Pose ip;
	Material * mat;
	// derived values
	V n;
	Aabb box;
	V2 rcpdim;
};

