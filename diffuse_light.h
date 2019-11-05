#pragma once

#include "material.h"
#include "texture.h"

struct DiffuseLight : public Material
{
	typedef r3::Vec3f V;
	typedef r3::Vec2f V2;

	DiffuseLight( Texture* emission ) : emit( emission ) {}
	~DiffuseLight()
	{
		delete emit;
	}

	bool Scatter( const Ray& incident, const Hit& hit, V& attenuation, Ray& scattered ) const
	{
		return false;
	}
	V Emitted( const V2& uv, const V& p ) const
	{
		return emit->Value( uv, p );
	}

	Texture* emit;
};