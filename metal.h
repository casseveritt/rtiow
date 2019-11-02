#pragma once

#include "material.h"
#include <random>

struct Metal : public Material
{
	typedef r3::Vec3f V;

	Metal( const V& albdo, float fuzziness ) : albedo( albdo ), fuzz( fuzziness ) {}

	static inline V Reflect( const V& incident, const V& normal )
	{
		return incident - 2 * incident.Dot( normal ) * normal;
	}

	bool Scatter( const Ray& incident, const Hit& hit, V& attenuation, Ray& scattered ) const
	{
		V refl = Reflect( incident.dir, hit.n ) + rpius.GetPoint() * fuzz;
		scattered = Ray( hit.p, refl, incident.t );
		attenuation = albedo;
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

	V albedo;
	float fuzz;
};
