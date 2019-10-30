#pragma once

#include "material.h"

struct Metal : public Material
{
	typedef r3::Vec3f V;

	Metal( const V& albdo ) : albedo( albdo ) {}

	static inline V Reflect( const V& incident, const V& normal )
	{
		return incident - 2 * incident.Dot( normal ) * normal;
	}

	bool Scatter( const Ray& incident, const Hit& hit, V& attenuation, Ray& scattered ) const
	{
		V refl = Reflect( incident.dir, hit.n );
		if ( refl.Dot( hit.n ) > 0 )
		{
			scattered = Ray( hit.p, refl );
			attenuation = albedo;
			return true;
		}
		return false;
	}

	V albedo;
};