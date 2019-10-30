

#include "camera.h"
#include "lambertian.h"
#include "metal.h"
#include "sphere.h"

#include <limits>
#include <random>
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

struct TraceResult
{
	TraceResult() : Valid( false ) {}
	TraceResult( const Vec3f& col ) : Valid( true ), Color( col ) {}
	TraceResult( const Vec3f& atten, const TraceResult& tr ) : Valid( tr.Valid ), Color( atten * tr.Color ) {}
	bool Valid;
	Vec3f Color;
};

TraceResult TraceRay( const Ray& ray, Hitable* hitable )
{
	Hit hit;
	if ( hitable->Hits( ray, 0.001, 10000, hit ) )
	{
		Ray scatter;
		Vec3f atten;
		if ( hit.mat->Scatter( ray, hit, atten, scatter ) )
		{
			return TraceResult( atten, TraceRay( scatter, hitable ) );
		}
		else
		{
			return TraceResult();
		}
	}
	else
	{
		return TraceResult( EnvColor( ray.dir ) );
	}
}

Vec3f GammaFromLinear( const Vec3f& col )
{
	return Vec3f( pow( col.x, 1 / 2.2 ), pow( col.y, 1 / 2.2 ), pow( col.z, 1 / 2.2 ) );
}

int main( int argc, char** argv )
{

	const int w = 400;
	const int h = 200;
	const int samples = 100;
	char* img = new char[ w * h * 3 ];

	Vec3f origin( 0, 0, 0 );

	Lambertian lamb_rose( Vec3f( 0.8f, 0.3f, 0.3f ) );
	Lambertian lamb_greenish( Vec3f( 0.8f, 0.8f, 0.0f ) );
	Metal metal_gold( Vec3f( 0.8, 0.6, 0.2 ), 0.3 );
	Metal metal_silver( Vec3f( 0.8, 0.8, 0.8 ), 0.1 );

	Sphere sphere( Vec3f( 0, 0, -1 ), 0.5, &lamb_rose );
	Sphere ground_sphere( Vec3f( 0, -100.5, -1 ), 100, &lamb_greenish );
	Sphere gold_sphere( Vec3f( 1, 0, -1 ), 0.5, &metal_gold );
	Sphere silver_sphere( Vec3f( -1, 0, -1 ), 0.5, &metal_silver );

	HitableCollection collection;
	collection.hitables.push_back( &sphere );
	collection.hitables.push_back( &ground_sphere );
	collection.hitables.push_back( &gold_sphere );
	collection.hitables.push_back( &silver_sphere );

	Camera cam( 90 /* fovy degrees */, 2 /* aspect ratio */ );

	std::mt19937 gen( 0 );
	std::uniform_real_distribution<> dis( 0.0, 1.0 );

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
			int s = 0;
			for ( int k = 0; k < samples; k++ )
			{
				Ray ray = cam.GetRay( uv + off[ k ] );
				TraceResult res = TraceRay( ray, &collection );
				if ( res.Valid )
				{
					s++;
					col += res.Color;
				}
			}
			col /= s;
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