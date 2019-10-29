#include "linear.h"
#include "ray.h"
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace r3;

namespace
{

Vec3f EnvColorFromRay( const Ray& ray )
{
	Vec3f unit = ray.dir / ray.dir.Length();
	float t = 0.5f * ( unit.y + 1.0f );
	// printf( "t = %.2f\n", t );
	return ( 1.0f - t ) * Vec3f( 1, 1, 1 ) + t * Vec3f( 0.5f, 0.7f, 1.0f );
}

float Discriminant( float* co )
{
	return co[ 1 ] * co[ 1 ] - 4 * co[ 0 ] * co[ 2 ];
}

struct Sphere
{
	Sphere( const Vec3f& center, const float radius ) : c( center ), r( radius ) {}

	bool Hits( const Ray& ray )
	{
		float coeffs[ 3 ] = {Dot( ray.dir, ray.dir ), 2 * Dot( ray.dir, ray.o - c ), Dot( ray.o - c, ray.o - c ) - r * r};
		float discr = Discriminant( coeffs );
		return discr >= 0;
	}

	Vec3f c;
	float r;
};

} // namespace

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
			Vec3f col = sphere.Hits( ray ) ? Vec3f( 1, 0, 0 ) : EnvColorFromRay( ray );
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