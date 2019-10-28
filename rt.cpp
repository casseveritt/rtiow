#include <stdio.h>
#include "linear.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace r3;

int main( int argc, char ** argv )
{

	const int w = 200;
	const int h = 100;
	char * img = new char [ w * h * 3 ];

	Vec3f scale( 255.0f / (w - 1), 255.0f / (h - 1), 255.0f * 0.21f );

	for ( int j = 0; j < h; j++ )
	{
		for ( int i = 0; i < w; i++ )
		{
			Vec3f coord( i,  (h - 1) - j, 1 );
			Vec3f col = coord * scale;
			int idx = ( j * w + i ) * 3;
			img[ idx + 0 ] = int( col.x + 0.5f );
			img[ idx + 1 ] = int( col.y + 0.5f );
			img[ idx + 2 ] = int( col.z + 0.5f );
		}
	}

	stbi_write_png( "out.png", w, h, 3, img, 0 );

	delete [] img;

	return 0;
}