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
		return V( 1, 1, 1 ) * ( perlin.Noise( p ) * 0.5f + 0.5f );
	}

	Perlin perlin;
};

struct Turbulence : public Texture
{
	typedef r3::Vec3f V;
	typedef r3::Vec2f V2;

	Turbulence( float scale = 1.0f ) : perlin( scale ) {}

	float Turb( V p, int octaves = 7 ) const
	{
		float accum = 0;
		float weight = 1;
		for ( int i = 0; i < octaves; i++ )
		{
			accum += weight * perlin.Noise( p );
			weight *= 0.5f;
			p *= 2;
		}
		return fabs( accum );
	}

	virtual V Value( const V2& uv, const V& p ) const
	{
		return V( 1, 1, 1 ) * Turb( p );
	}

	Perlin perlin;
};

struct Marble : public Texture
{
	typedef r3::Vec3f V;
	typedef r3::Vec2f V2;

	Marble( float scale = 1.0f ) : perlin( scale ) {}

	float Turb( V p, int octaves = 7 ) const
	{
		float accum = 0;
		float weight = 1;
		for ( int i = 0; i < octaves; i++ )
		{
			accum += weight * perlin.Noise( p );
			weight *= 0.5f;
			p *= 2;
		}
		return fabs( accum );
	}

	virtual V Value( const V2& uv, const V& p ) const
	{
		return V( 1, 1, 1 ) * 0.5 * ( 1 + sin( perlin.scale * p.z + 10 * Turb( p * ( 1 / perlin.scale ) ) ) );
	}

	Perlin perlin;
};