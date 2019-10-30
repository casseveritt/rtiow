

#include "camera.h"
#include "hitable.h"
#include "sphere.h"

#include <limits>
#include <random>
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace r3;

namespace
{

Vec3f EnvColor( const Vec3f& dir )
{
	Vec3f unit = dir / dir.Length();
	float t = 0.5f * ( unit.y + 1.0f );
	// printf( "t = %.2f\n", t );
	return ( 1.0f - t ) * Vec3f( 1, 1, 1 ) + t * Vec3f( 0.5f, 0.7f, 1.0f );
}

struct RandomPointInUnitSphere
{
	RandomPointInUnitSphere() : gen( 0 ), dis( -1.0, 1.0 ) {}

	Vec3f GetPoint()
	{
		Vec3f p( dis( gen ), dis( gen ), dis( gen ) );
		while ( p.LengthSquared() > 1.0f )
		{
			p = Vec3f( dis( gen ), dis( gen ), dis( gen ) );
		}
		return p;
	}

	std::mt19937 gen;
	std::uniform_real_distribution<> dis;
};

RandomPointInUnitSphere rpius;

Vec3f ColorFromRay( const Ray& ray, Hitable* hitable )
{
	Hit hit;
	if ( hitable->Hits( ray, 0, 1000, &hit ) )
	{
		Ray new_ray( hit.p, hit.n + rpius.GetPoint() );
		return 0.5f * ColorFromRay( new_ray, hitable );
	}
	else
	{
		return EnvColor( ray.dir );
	}
}

Vec3f GammaFromLinear( const Vec3f& col )
{
	return Vec3f( sqrt( col.x ), sqrt( col.y ), sqrt( col.z ) );
}

} // namespace

int main( int argc, char** argv )
{

	const int w = 400;
	const int h = 200;
	char* img = new char[ w * h * 3 ];

	Vec3f origin( 0, 0, 0 );

	Sphere sphere( Vec3f( 0, 0, -1 ), 0.5 );
	Sphere sphere2( Vec3f( 0, -100.5, -1 ), 100 );
	HitableCollection collection;
	collection.hitables.push_back( &sphere );
	collection.hitables.push_back( &sphere2 );

	Camera cam( 90, 2 );

	std::mt19937 gen( 0 );
	std::uniform_real_distribution<> dis( 0.0, 1.0 );

	const int samples = 16;
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
				if ( collection.Hits( ray, 0, 1000, &hit ) )
				{
					col += hit.n * 0.5f + 0.5f;
				}
				else
				{
					col += EnvColor( ray.dir );
				}
			}
			col /= samples;
			col = GammaFromLinear( col );
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