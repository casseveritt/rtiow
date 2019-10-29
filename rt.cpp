#include "linear.h"
#include "ray.h"
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace r3;

Vec3f EnvColorFromRay( const Ray& ray )
{
	Vec3f unit = ray.dir / ray.dir.Length();
	float t = 0.5f * ( unit.y + 1.0f );
	// printf( "t = %.2f\n", t );
	return ( 1.0f - t ) * Vec3f( 1, 1, 1 ) + t * Vec3f( 0.5f, 0.7f, 1.0f );
}

template <typename T>
struct Polynomial
{
	Polynomial( int degree, const T * coef )
	{
		Init( degree, coef );
	}

	Polynomial( const T a, const T b )
	{
		deg = 1;
		T coef[] = { a, b };
		Init( 2, coef );
	}

	Polynomial( const T a, const T b, const T c )
	{
		deg = 1;
		T coef[] = { a, b, c };
		Init( 2, coef );
	}

	void Init( int degree, const T * coef )
	{
		deg = std::min( degree, 15 );
		for ( int i = 0; i < 16; i++ )
		{
			co[ i ] = i <= deg ? coef[ i ] : 0;
		}
	}

	int deg;
	union
	{
		struct
		{
			T a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
		};
		T co[16];
	};

};

typedef Polynomial<float> Polynomialf;

float Discriminant( const Polynomialf & pol )
{
	return pol.b * pol.b - 4 * pol.a * pol.c;
}

struct Sphere
{
	Sphere( const Vec3f& center, const float radius ) : c( center ), r( radius ) {}

	float Hits( const Ray& ray )
	{
		Polynomialf pol( Dot( ray.dir, ray.dir ), 2 * Dot( ray.dir, ray.o - c ), Dot( ray.o - c, ray.o - c ) - r * r );
		float discr = Discriminant( pol );
		if ( discr >= 0 )
		{
			return -pol.b - sqrt( discr ) / ( 2 * pol.a );
		}
		else
		{
			return -1;
		}
	}

	Vec3f c;
	float r;
};

int main( int argc, char** argv )
{

	const int w = 400;
	const int h = 200;
	char* img = new char[ w * h * 3 ];

	Vec3f origin( 0, 0, 0 );
	Vec3f scale( 4.0f / w, 2.0f / h, 1.0f );
	Vec3f bias( -2.0f, -1.0f, -1.0f );

	Sphere sphere( Vec3f( 0, 0, -1 ), 0.5 );

	for ( int j = 0; j < h; j++ )
	{
		for ( int i = 0; i < w; i++ )
		{
			Vec3f coord( i + 0.5f, ( h - 1 ) - j + 0.5f, 0 );
			Ray ray( origin, coord * scale + bias );
			Vec3f col = EnvColorFromRay( ray );
			float t = sphere.Hits( ray );
			if ( t > 0 )
			{
				Vec3f hit = ray.At( t );
				Vec3f n = ( hit - sphere.c ).Normalized();
				col = n * 0.5f + 0.5f;
			}
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