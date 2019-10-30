

#include "camera.h"
#include "sphere.h"
#include "env.h"

#include <random>
#include <limits>
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace r3;

Vec3f EnvColor( const Vec3f& dir )
{
	Vec3f unit = dir.Normalized();
	float t = 0.5f * ( unit.y + 1.0f );
	return r3::Lerp( Vec3f( 1, 1, 1 ), Vec3f( 0.5f, 0.7f, 1.0f ), t );
}

int main( int argc, char** argv )
{

	const int w = 400;
	const int h = 200;
	char* img = new char[ w * h * 3 ];

	Vec3f origin( 0, 0, 0 );

	Sphere sphere( Vec3f( 0, 0, -1 ), 0.5 );
	Sphere sphere2( Vec3f( 0, -100.5, -1 ), 100 );
	Env env;
	HitableCollection collection;
	collection.hitables.push_back( &sphere );
	collection.hitables.push_back( &sphere2 );
	collection.hitables.push_back( &env );

	Camera cam( 90 /* fovy degrees */, 2 /* aspect ratio */ );

	std::mt19937 gen( 0 );
	std::uniform_real_distribution<> dis( 0.0, 1.0 );

	const int samples = 64;
	Vec2f off[ samples ];
	for ( int k = 0; k < samples; k++ )
	{
		off[ k ] = Vec2f( dis( gen ) / w, dis( gen ) / h );
	}

	for ( int j = 0; j < h; j++ )
	{
		for ( int i = 0; i < w; i++ )
		{
			const Vec2f uv( float( i ) / w, float( h - 1 - j ) / h );
			Vec3f col( 0, 0, 0 );
			for ( int k = 0; k < samples; k++ )
			{
				Ray ray = cam.GetRay( uv + off[ k ] );
				Hit hit;
				// We don't check hits, because env will always hit
				collection.Hits( ray, 0, 1000, &hit );
				if ( hit.t < std::numeric_limits<float>::max() )
				{
					col += hit.n * 0.5f + 0.5f;
				}
				else
				{
					col += EnvColor( ray.dir );
				}
			}
			col /= samples;
			int idx = ( j * w + i ) * 3;
			img[ idx + 0 ] = int( 255 * col.x + 0.5f );
			img[ idx + 1 ] = int( 255 * col.y + 0.5f );
			img[ idx + 2 ] = int( 255 * col.z + 0.5f );
		}
	}

	stbi_write_png( "out.png", w, h, 3, img, 0 );

	delete[] img;

	return 0;
}