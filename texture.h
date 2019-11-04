#pragma once

#include "linear.h"
#include <math.h>

struct Texture
{
	typedef r3::Vec3f V;
	typedef r3::Vec2f V2;
	virtual ~Texture() {}
	virtual V Value( const V2& uv, const V& p ) const = 0;
};

struct ConstantTexture : public Texture
{
	typedef r3::Vec3f V;
	typedef r3::Vec2f V2;

	ConstantTexture( const V& constval ) : c( constval ) {}

	V Value( const V2& uv, const V& p ) const
	{
		return c;
	}

	V c;
};

struct CheckerTexture : public Texture
{
	typedef r3::Vec3f V;
	typedef r3::Vec2f V2;

	CheckerTexture( Texture* evenTex, Texture* oddTex ) : even( evenTex ), odd( oddTex ) {}
	virtual ~CheckerTexture()
	{
		delete even;
		delete odd;
	}

	V Value( const V2& uv, const V& p ) const
	{
		V sc = p * 10;
		float sines = sin( sc.x ) * sin( sc.y ) * sin( sc.z );
		return ( sines < 0 ? odd : even )->Value( uv, p );
	}

	Texture* even;
	Texture* odd;
};