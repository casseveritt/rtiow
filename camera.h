#pragma once

#include "ray.h"
#include <cmath>

struct Camera
{
	typedef r3::Vec2f V2;
	typedef r3::Vec3f V3;
	typedef r3::Posef Pose;

	// symmetric fov
	Camera( float fovy_deg, float aspect )
	{
		SetFov( fovy_deg, aspect );
	}

	Camera( float fovy_deg, float aspect, const V3& from, const V3& to, const V3& up )
	{
		SetFov( fovy_deg, aspect );
		SetPose( from, to, up );
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

	Ray GetRay( const V2& uv )
	{
		V3 dir( r3::Lerp( ll.x, ur.x, uv.x ), r3::Lerp( ll.y, ur.y, uv.y ), -1 );
		pose.r.MultVec( dir );
		return Ray( pose.t, dir );
	}

	V2 ll;
	V2 ur;
	Pose pose;
};
