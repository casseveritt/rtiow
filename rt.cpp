

#include "aabb.h"
#include "camera.h"
#include "dielectric.h"
#include "diffuse_light.h"
#include "image_texture.h"
#include "lambertian.h"
#include "metal.h"
#include "noise.h"
#include "sphere.h"
#include "rect.h"

#include "stb.h"

#include <limits>
#include <random>
#include <stdio.h>

using namespace r3;

int trace_recursion = 0;

namespace
{

typedef Vec3f V;
typedef Vec2f V2;

V EnvColor( const V& dir )
{
	float t = 0.5f * dir.y + 0.5f;
	return r3::Lerp( V( 1, 1, 1 ), V( 0.5f, 0.7f, 1.0f ), t );
}

struct TraceResult
{
	TraceResult() : Valid( false ) {}
	TraceResult( const V& col ) : Valid( true ), Color( col ) {}
	TraceResult( const V& atten, const TraceResult& tr ) : Valid( tr.Valid ), Color( atten * tr.Color ) {}
	bool Valid;
	V Color;
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

TraceResult TraceRay( const V& attenuation, const Ray& ray, Hitable* hitable, int depth = 0 )
{
	ScopedRefCount src( trace_recursion );

	// if "fully" attenuated, give up
	if ( depth > 50 || attenuation.Dot( attenuation ) < 0.001 )
	{
		return TraceResult( V( 0, 0, 0 ) );
	}

	Hit hit;
	if ( hitable->Hits( ray, 0.001, hit ) )
	{
		Ray scatter;
		V atten;
		V emission = hit.mat->Emitted( hit.uv, hit.p );
		if ( hit.mat->Scatter( ray, hit, atten, scatter ) )
		{
			// cast a scattering ray, accumulating attenuation
			return TraceRay( atten * attenuation, scatter, hitable, depth + 1 );
		}
		else
		{
			// scatter failed, give up on ray
			return TraceResult( emission * attenuation );
		}
	}
	else
	{
		// environment
		return TraceResult( /*EnvColor( ray.dir )*/ V( 0, 0, 0 ) );
	}
}

V GammaFromLinear( const V& col )
{
	return V( pow( col.x, 1 / 2.2 ), pow( col.y, 1 / 2.2 ), pow( col.z, 1 / 2.2 ) );
}

void random_scene( HitableCollection& hc )
{
	CheckerTexture* checker =
		new CheckerTexture( new ConstantTexture( V( 0.2, 0.3, 0.1 ) ), new ConstantTexture( V( 0.9, 0.9, 0.9 ) ) );
	hc.Add( new Sphere( V( 0, -1000, 0 ), 1000, new Lambertian( checker ) ) );
	std::mt19937 gen( 0 );
	std::uniform_real_distribution<> dis( 0.0, 1.0 );
	for ( int a = -11; a < 11; a++ )
	{
		for ( int b = -11; b < 11; b++ )
		{
			float choose_mat = dis( gen );
			V center( a + 0.9 * dis( gen ), 0.2, b + 0.9 * dis( gen ) );
			if ( ( center - V( 4, 0.2, 0 ) ).Length() > 0.9 )
			{
				if ( choose_mat < 0.8 )
				{ // diffuse
					hc.Add( new Sphere( center, 0.2,
										new Lambertian( new ConstantTexture( V(
											dis( gen ) * dis( gen ), dis( gen ) * dis( gen ), dis( gen ) * dis( gen ) ) ) ),
										V( 0, 0.5 * dis( gen ), 0 ) ) );
				}
				else if ( choose_mat < 0.95 )
				{ // metal
					hc.Add( new Sphere(
						center, 0.2,
						new Metal( V( 0.5 * ( 1 + dis( gen ) ), 0.5 * ( 1 + dis( gen ) ), 0.5 * ( 1 + dis( gen ) ) ),
								   0.5 * dis( gen ) ),
						V( 0, 0.5 * dis( gen ), 0 ) ) );
				}
				else
				{ // glass
					hc.Add( new Sphere( center, 0.2, new Dielectric( 1.5 ), V( 0, 0.5 * dis( gen ), 0 ) ) );
				}
			}
		}
	}
	hc.Add( new Sphere( V( 0, 1, 0 ), 1.0, new Dielectric( 1.5 ) ) );
	hc.Add( new Sphere( V( -4, 1, 0 ), 1.0, new Lambertian( new ConstantTexture( V( 0.4, 0.2, 0.1 ) ) ) ) );
	hc.Add( new Sphere( V( 4, 1, 0 ), 1.0, new Metal( V( 0.7, 0.6, 0.5 ), 0.0 ) ) );
}

void two_spheres_scene( HitableCollection& hc )
{
	hc.Add( new Sphere( V( 0, 7, 0 ), 2, new DiffuseLight( new ConstantTexture( V( 2, 2, 2 ) ) ) ) );
	hc.Add( new Sphere( V( 0, 2, 0 ), 2, new Metal( V( 0.8, 0.8, 0.8 ), 0.2 ) ) );
	hc.Add( new Sphere( V( 0, -1000, 0 ), 1000, new Lambertian( new Marble( 6 ) ) ) );
	hc.Add( new Rect( V2( 0, 0 ), V2( 4, 2 ), Posef( Rotationf( V( 0, 0, 1 ), r3::ToRadians( 25.0f ) ), V( 1, 0, -2 ) ),
					  new DiffuseLight( new ImageTexture( "earthmap.jpg" ) ) ) );
}

void cornell_box_scene( HitableCollection& hc )
{
	Posef bot( Rotationf( V( 1, 0, 0 ), r3::ToRadians( -90.0f ) ), V( 0, 0, 0 ) );
	Posef top( Rotationf( V( 1, 0, 0 ), r3::ToRadians( 90.0f ) ), V( 0, 4, 0 ) );
	Posef left( Rotationf( V( 0, 1, 0 ), r3::ToRadians( 90.0f ) ), V( -2, 2, 0) );
	Posef right( Rotationf( V( 0, 1, 0 ), r3::ToRadians( -90.0f ) ), V( 2, 2, 0 ) );
	Posef back( Rotationf(), V( 0, 2, -2 ) );
	Posef lightpose( Rotationf( V( 1, 0, 0 ), r3::ToRadians( -90.0f ) ), V( 0, 4 - 0.01, 0 ) );
	DiffuseLight * light = new DiffuseLight( new ConstantTexture( V( 4, 4, 4 ) ) );
	hc.Add( new Rect( V2( -0.5, -0.5 ), V2( 0.5, 0.5 ), lightpose, light ) );
	V2 m0( -2, -2 );
	V2 m1( 2, 2 );
	Lambertian * white = new Lambertian( new ConstantTexture( V( .8, .8, .8 ) ) );
	Lambertian * green = new Lambertian( new ConstantTexture( V( .1, .8, .1 ) ) );
	Lambertian * red = new Lambertian( new ConstantTexture( V( .8, .1, .1 ) ) );

	hc.Add( new Rect( m0, m1, top, white ) );
	hc.Add( new Rect( m0, m1, bot, white ) );
	hc.Add( new Rect( m0, m1, back, white ) );
	hc.Add( new Rect( m0, m1, left, green ) );
	hc.Add( new Rect( m0, m1, right, red ) );
	hc.Add( new Sphere( V( 0, 1, 0 ), .5, new Metal( V( 0.8, 0.8, 0.8 ), 0.2 ) ) );
}

} // namespace

int main( int argc, char** argv )
{

	const int w = 800;
	const int h = 400;
	const int samples = 600;
	unsigned char* img = new unsigned char[ w * h * 3 ];

	V origin( 0, 0, 0 );

	HitableCollection collection;
	// random_scene( collection );
	// two_spheres_scene( collection );
	cornell_box_scene( collection );

	Camera cam;
	cam.SetFov( 30, float( w ) / float( h ) );
	V from( 0, 2, 10 );
	V to( 0, 2, 0 );
	V up( 0, 1, 0 );
	cam.SetPose( from, to, up );
	cam.SetFocus( 0.05f, 10 );
	cam.SetExposure( 0, 0 );

	collection.BuildBvh( cam.t0, cam.t1 );

	std::mt19937 gen( 0 );
	std::uniform_real_distribution<> dis( 0.0, 1.0 );

	V2 off[ samples ];
	for ( int k = 0; k < samples; k++ )
	{
		off[ k ] = V2( dis( gen ) / w, dis( gen ) / h );
	}

	for ( int j = 0; j < h; j++ )
	{
		for ( int i = 0; i < w; i++ )
		{
			const V2 uv( float( i ) / w, float( h - 1 - j ) / h );
			V col( 0, 0, 0 );
			int s = 0;
			for ( int k = 0; k < samples; k++ )
			{
				Ray ray = cam.GetRay( uv + off[ k ] );
				TraceResult res = TraceRay( V( 1, 1, 1 ), ray, &collection );
				if ( res.Valid )
				{
					s++;
					col += res.Color;
				}
			}
			col /= s;
			col = r3::Min( GammaFromLinear( col ), V( 1, 1, 1 ) );
			int idx = ( j * w + i ) * 3;
			img[ idx + 0 ] = int( 255 * col.x + 0.5f );
			img[ idx + 1 ] = int( 255 * col.y + 0.5f );
			img[ idx + 2 ] = int( 255 * col.z + 0.5f );
		}
	}

	image_store_png( "out.png", w, h, 3, img );

	delete[] img;

	return 0;
}
