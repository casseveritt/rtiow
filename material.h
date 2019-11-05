#pragma once

#include "hitable.h"

struct Material
{
	typedef r3::Vec3f V;
	typedef r3::Vec2f V2;
	virtual bool Scatter( const Ray& incident, const Hit& hit, V& attenuation, Ray& scattered ) const = 0;
	virtual V Emitted( const V2& uv, const V& p ) const
	{
		return V( 0, 0, 0 );
	}
};