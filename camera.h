#pragma once

#include "ray.h"
#include <cmath>
#include <random>

struct Camera
{
	typedef r3::Vec2f V2;
	typedef r3::Vec3f V3;
	typedef r3::Posef Pose;

	// symmetric fov
	Camera() : gen( 0 ), dis( -1, 1 )
	{
		SetFov( 60, 1 );
	}

	void SetFov( float fovy_deg, float aspect )
	{
		ur.y = tan( 0.5f * fovy_deg * M_PI / 180.0 );
		ur.x = aspect * ur.y;
		ll = -ur;
	}

	void SetPose( const V3& from, const V3& to, const V3& up )
	{
		pose.SetValue( from, to, up );
	}

	void SetFocus( float aperture, float focal_distance )
	{
		lens_radius = aperture / 2;
		focal_dist = focal_distance;
	}

	Ray GetRay( const V2& uv )
	{
		V3 o = PointOnLens();
		V3 target = V3( r3::Lerp( ll.x, ur.x, uv.x ), r3::Lerp( ll.y, ur.y, uv.y ), -1 ) * focal_dist;
		return Ray( pose.t + pose.r.Rotate( o ), pose.r.Rotate( target - o ) );
	}

	V3 PointOnLens() const
	{
		V2 p( dis( gen ), dis( gen ) );
		while ( p.LengthSquared() > 1 )
		{
			p = V2( dis( gen ), dis( gen ) );
		}
		p *= lens_radius;
		return V3( p.x, p.y, 0 );
	}

	V2 ll;
	V2 ur;
	Pose pose;
	float lens_radius = 0;
	float focal_dist = 1;
	mutable std::mt19937 gen;
	mutable std::uniform_real_distribution<> dis;
};
