#pragma once

#include "material.h"
#include <random>

struct Dielectric : public Material
{
	typedef r3::Vec3f V;

	Dielectric( float index_of_refraction ) : refr_index( index_of_refraction ) {}

	static inline V Reflect( const V& incident, const V& normal )
	{
		return incident - 2 * incident.Dot( normal ) * normal;
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
		else
		{
			return false;
		}
	}

	bool Scatter( const Ray& incident, const Hit& hit, V& attenuation, Ray& scattered ) const
	{
		bool outside = incident.dir.Dot( hit.n ) < 0;
		V normal = outside ? hit.n : -hit.n;
		float index_ratio = outside ? 1.0f / refr_index : refr_index;
		V refr;
		if ( Refract( incident.dir, normal, index_ratio, refr ) )
		{
			scattered = Ray( hit.p, refr );
			attenuation = V( 1, 1, 1 );
		}
		else
		{
			V refl = Reflect( incident.dir, normal );
			scattered = Ray( hit.p, refl );
			attenuation = V( 1, 0, 0 );
		}
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

	mutable RandomPointInUnitSphere rpius;

	float refr_index;
};
