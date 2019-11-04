#pragma once

#include "texture.h"

#include "stb.h"

struct ImageTexture : public Texture
{
	typedef r3::Vec3f V;
	typedef r3::Vec2f V2;

	ImageTexture( const char* filename ) : n( 0 ), img( nullptr )
	{
		int w, h;
		img = image_load( filename, &w, &h, &n );
		dim = V2( w, h );
	}

	~ImageTexture()
	{
		image_free( img );
	}

	V Value( const V2& uv, const V& p ) const
	{
		V2 uv_flipped = uv;
		uv_flipped.y = 1 - uv_flipped.y;
		V2 ij = uv_flipped * dim;
		int i = ij.x;
		int j = ij.y;
		unsigned char* pix = img + ( j * int( dim.x ) + i ) * n;
		return V( pix[ 0 ], pix[ 1 ], pix[ 2 ] ) / 255;
	}

	V2 dim;
	int n;
	unsigned char* img;
};
