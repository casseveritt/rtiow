#pragma once

#include "material.h"
#include <random>

struct Dielectric : public Material
{
	typedef r3::Vec3f V;

	Dielectric( float index_of_refraction ) : refr_index( index_of_refraction ), gen( 0 ), dis( 0.0, 1.0 ) {}

	static inline V Reflect( const V& incident, const V& normal )
	{
		return incident - 2 * incident.Dot( normal ) * normal;
	}

	static inline float Schlick( float cosine, float nr )
	{
		float r0 = ( 1 - nr ) / ( 1 + nr );
		r0 = r0 * r0;
		return r0 + ( 1 - r0 ) * pow( ( 1 - cosine ), 5.0f );
	}

	static inline bool Refract( const V& incident, const V& normal, float nr, V& refracted )
	{
		float dt = incident.Dot( normal );
		float discriminant = 1.0f - nr * nr * ( 1 - dt * dt );
		if ( discriminant > 0.0f )
		{
			refracted = nr * ( incident - dt * normal ) - normal * sqrt( discriminant );
			return true;
		}
		return false;
	}

	bool Scatter( const Ray& incident, const Hit& hit, V& attenuation, Ray& scattered ) const
	{
		bool outside = incident.dir.Dot( hit.n ) < 0;
		V normal = outside ? hit.n : -hit.n;
		float nr = outside ? 1.0f / refr_index : refr_index;
		V refr;
		if ( Refract( incident.dir, normal, nr, refr ) && RandomScalar() > Schlick( -incident.dir.Dot( normal ), nr ) )
		{
			scattered = Ray( hit.p, refr );
		}
		else
		{
			scattered = Ray( hit.p, Reflect( incident.dir, normal ) );
		}
		attenuation = V( 1, 1, 1 );
		return true;
	}

	struct RandomPointInUnitSphere
	{
		RandomPointInUnitSphere() : gen( 0 ), dis( -1.0, 1.0 ) {}

		V GetPoint()
		{
			V p( dis( gen ), dis( gen ), dis( gen ) );
			while ( p.LengthSquared() > 1.0f )
			{
				p = V( dis( gen ), dis( gen ), dis( gen ) );
			}
			return p;
		}

		std::mt19937 gen;
		std::uniform_real_distribution<> dis;
	};

	float RandomScalar() const
	{
		return dis( gen );
	}

	mutable RandomPointInUnitSphere rpius;

	float refr_index;
	mutable std::mt19937 gen;
	mutable std::uniform_real_distribution<> dis;
};
