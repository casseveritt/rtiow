#pragma once

#include "material.h"
#include <random>

struct Lambertian : public Material
{
	typedef r3::Vec3f V;

	Lambertian( const V& attenuation ) : albedo( attenuation ) {}

	bool Scatter( const Ray& incident, const Hit& hit, V& attenuation, Ray& scattered ) const
	{
		scattered = Ray( hit.p, hit.n + rpius.GetPoint() );
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
};