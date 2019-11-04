#pragma once

#include "perlin.h"
#include "texture.h"

struct Noise : public Texture
{
	typedef r3::Vec3f V;
	typedef r3::Vec2f V2;

	Noise( float scale = 1.0f ) : perlin( scale ) {}
	virtual V Value( const V2& uv, const V& p ) const
	{
		float n = perlin.Noise( p ) * 0.5f + 0.5f;
		return V( n, n, n );
	}

	Perlin perlin;
};