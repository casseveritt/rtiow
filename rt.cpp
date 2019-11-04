

#include "aabb.h"
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

int trace_recursion = 0;

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

struct ScopedRefCount
{
	ScopedRefCount( int& refcount ) : rc( refcount )
	{
		rc++;
	}
	~ScopedRefCount()
	{
		rc--;
	}
	int& rc;
};

TraceResult TraceRay( const Vec3f& attenuation, const Ray& ray, Hitable* hitable, int depth = 0 )
{
	ScopedRefCount src( trace_recursion );

	// if "fully" attenuated, give up
	if ( depth > 50 || attenuation.Dot( attenuation ) < 0.001 )
	{
		return TraceResult( Vec3f( 0, 0, 0 ) );
	}

	Hit hit;
	if ( hitable->Hits( ray, 0.001, hit ) )
	{
		Ray scatter;
		Vec3f atten;
		if ( hit.mat->Scatter( ray, hit, atten, scatter ) )
		{
			// cast a scattering ray, accumulating attenuation
			return TraceRay( atten * attenuation, scatter, hitable, depth + 1 );
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

void random_scene( HitableCollection& hc )
{
	CheckerTexture* checker =
		new CheckerTexture( new ConstantTexture( Vec3f( 0.2, 0.3, 0.1 ) ), new ConstantTexture( Vec3f( 0.9, 0.9, 0.9 ) ) );
	hc.Add( new Sphere( Vec3f( 0, -1000, 0 ), 1000, new Lambertian( checker ) ) );
	int i = 1;
	std::mt19937 gen( 0 );
	std::uniform_real_distribution<> dis( 0.0, 1.0 );
	for ( int a = -11; a < 11; a++ )
	{
		for ( int b = -11; b < 11; b++ )
		{
			float choose_mat = dis( gen );
			Vec3f center( a + 0.9 * dis( gen ), 0.2, b + 0.9 * dis( gen ) );
			if ( ( center - Vec3f( 4, 0.2, 0 ) ).Length() > 0.9 )
			{
				if ( choose_mat < 0.8 )
				{ // diffuse
					hc.Add( new Sphere( center, 0.2,
										new Lambertian( new ConstantTexture( Vec3f(
											dis( gen ) * dis( gen ), dis( gen ) * dis( gen ), dis( gen ) * dis( gen ) ) ) ),
										Vec3f( 0, 0.5 * dis( gen ), 0 ) ) );
				}
				else if ( choose_mat < 0.95 )
				{ // metal
					hc.Add( new Sphere(
						center, 0.2,
						new Metal( Vec3f( 0.5 * ( 1 + dis( gen ) ), 0.5 * ( 1 + dis( gen ) ), 0.5 * ( 1 + dis( gen ) ) ),
								   0.5 * dis( gen ) ),
						Vec3f( 0, 0.5 * dis( gen ), 0 ) ) );
				}
				else
				{ // glass
					hc.Add( new Sphere( center, 0.2, new Dielectric( 1.5 ), Vec3f( 0, 0.5 * dis( gen ), 0 ) ) );
				}
			}
		}
	}
	hc.Add( new Sphere( Vec3f( 0, 1, 0 ), 1.0, new Dielectric( 1.5 ) ) );
	hc.Add( new Sphere( Vec3f( -4, 1, 0 ), 1.0, new Lambertian( new ConstantTexture( Vec3f( 0.4, 0.2, 0.1 ) ) ) ) );
	hc.Add( new Sphere( Vec3f( 4, 1, 0 ), 1.0, new Metal( Vec3f( 0.7, 0.6, 0.5 ), 0.0 ) ) );
}

} // namespace

int main( int argc, char** argv )
{

	const int w = 1200;
	const int h = 800;
	const int samples = 20;
	char* img = new char[ w * h * 3 ];

	Vec3f origin( 0, 0, 0 );

	HitableCollection collection;
	random_scene( collection );

	Camera cam;
	cam.SetFov( 20, float( w ) / float( h ) );
	Vec3f from( 13, 2, 3 );
	Vec3f to( 0, 0, 0 );
	Vec3f up( 0, 1, 0 );
	cam.SetPose( from, to, up );
	cam.SetFocus( 0.1f, 10 );
	cam.SetExposure( 0, 0 );

	collection.BuildBvh( cam.t0, cam.t1 );

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
