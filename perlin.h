#pragma once

#include "linear.h"
#include <algorithm>
#include <random>

struct Perlin
{
	typedef r3::Vec3f V;

	static constexpr int Size = 256;

	float scale;
	int permx[ Size ];
	int permy[ Size ];
	int permz[ Size ];
	V ranvec[ Size ];

	static void GeneratePerm( std::mt19937& gen, std::uniform_real_distribution<>& dis, int* tbl )
	{
		for ( int i = 0; i < Size; i++ )
		{
			tbl[ i ] = i;
		}
		for ( int i = Size - 1; i > 0; i-- )
		{
			int target = dis( gen ) * ( i + 1 );
			std::swap( tbl[ i ], tbl[ target ] );
		}
	}

	Perlin( float p_scale ) : scale( p_scale )
	{
		std::mt19937 gen( 0 );
		std::uniform_real_distribution<> dis( 0.0, 1.0 );

		GeneratePerm( gen, dis, permx );
		GeneratePerm( gen, dis, permy );
		GeneratePerm( gen, dis, permz );

		std::uniform_real_distribution<> disu( -1.0, 1.0 );
		for ( int i = 0; i < Size; i++ )
		{
			ranvec[ i ] = V( disu( gen ), disu( gen ), disu( gen ) ).Normalized();
		}
	}

	float Noise( const V& pos ) const
	{
		V p = pos * scale;
		V ijk( floor( p.x ), floor( p.y ), floor( p.z ) );
		V uvw = p - ijk;
		V huvw = uvw * uvw * ( V( 3, 3, 3 ) - 2 * uvw );
		float accum = 0;
		for ( int k = 0; k < 2; k++ )
		{
			for ( int j = 0; j < 2; j++ )
			{
				for ( int i = 0; i < 2; i++ )
				{
					V lattice_vec = ranvec[ permx[ ( i + int( ijk.x ) ) & 0xff ] ^ permy[ ( j + int( ijk.y ) ) & 0xff ] ^
											permz[ ( k + int( ijk.z ) ) & 0xff ] ];
					V weight = uvw - V( i, j, k );
					accum += ( i > 0 ? huvw.x : ( 1 - huvw.x ) ) * ( j > 0 ? huvw.y : ( 1 - huvw.y ) ) *
							 ( k > 0 ? huvw.z : ( 1 - huvw.z ) ) * r3::Dot( weight, lattice_vec );
				}
			}
		}
		return accum;
	}
};