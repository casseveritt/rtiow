

#include "camera.h"
#include "dielectric.h"
#include "lambertian.h"
#include "metal.h"
#include "sphere.h"

#include <limits>
#include <random>
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace r3;

namespace
{

Vec3f EnvColor( const Vec3f& dir )
{
	float t = 0.5f * dir.y + 0.5f;
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

TraceResult TraceRay( const Vec3f& attenuation, const Ray& ray, Hitable* hitable )
{
	// if "fully" attenuated, give up
	if ( attenuation.Dot( attenuation ) < 0.001 )
	{
		return TraceResult( Vec3f( 0, 0, 0 ) );
	}

	Hit hit;
	if ( hitable->Hits( ray, hit ) )
	{
		Ray scatter;
		Vec3f atten;
		if ( hit.mat->Scatter( ray, hit, atten, scatter ) )
		{
			// cast a scattering ray, accumulating attenuation
			return TraceRay( atten * attenuation, scatter, hitable );
		}
		else
		{
			// scatter failed, give up on ray
			return TraceResult();
		}
	}
	else
	{
		// hit the environment
		return TraceResult( attenuation * EnvColor( ray.dir ) );
	}
}

Vec3f GammaFromLinear( const Vec3f& col )
{
	return Vec3f( pow( col.x, 1 / 2.2 ), pow( col.y, 1 / 2.2 ), pow( col.z, 1 / 2.2 ) );
}

} // namespace

int main( int argc, char** argv )
{

	const int w = 400;
	const int h = 200;
	const int samples = 100;
	char* img = new char[ w * h * 3 ];

	Vec3f origin( 0, 0, 0 );

	Lambertian lamb_blue( Vec3f( 0.1f, 0.2f, 0.5f ) );
	Lambertian lamb_greenish( Vec3f( 0.8f, 0.8f, 0.0f ) );
	Metal metal_gold( Vec3f( 0.8, 0.6, 0.2 ), 0.3 );
	// Metal metal_silver( Vec3f( 0.8, 0.8, 0.8 ), 0.1 );
	Dielectric diel_yellow( 1.5f );

	Sphere blue_sphere( Vec3f( 0, 0, -1 ), 0.5, &lamb_blue );
	Sphere ground_sphere( Vec3f( 0, -100.5, -1 ), 100, &lamb_greenish );
	Sphere gold_sphere( Vec3f( 1, 0, -1 ), 0.5, &metal_gold );
	Sphere diel_yellow_sphere( Vec3f( -1, 0, -1 ), 0.5, &diel_yellow );

	HitableCollection collection;
	collection.hitables.push_back( &blue_sphere );
	collection.hitables.push_back( &ground_sphere );
	collection.hitables.push_back( &gold_sphere );
	collection.hitables.push_back( &diel_yellow_sphere );

	Camera cam( 90, 2 );

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
				TraceResult res = TraceRay( Vec3f( 1, 1, 1 ), ray, &collection );
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