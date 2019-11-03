#pragma once

#include "aabb.h"
#include "linear.h"
#include "ray.h"

#include <assert.h>
#include <vector>

struct Material;

struct Hit
{
	typedef r3::Vec3f V;
	float s;
	V p;
	V n;
	const Material* mat;
};

struct Hitable
{
	virtual bool Hits( const Ray& ray, float s_min, Hit& hit ) = 0;
	virtual Aabb GetBounds( float time0 = 0, float time1 = 0 ) const = 0;
};

struct HitableCollection : public Hitable
{
	typedef r3::Vec3f V;

	struct Bvh
	{
		Bvh() : child0( nullptr ), child1( nullptr ) {}
		~Bvh()
		{
			delete child0;
			delete child1;
		}

		void Add( Hitable* hitable, float time0, float time1 )
		{
			hitables.push_back( hitable );
			bounds = Union( bounds, hitable->GetBounds( time0, time1 ) );
		}

		bool Hits( const Ray& ray, float s_min, Hit& hit )
		{
			if ( bounds.Hits( ray ) == false )
			{
				return false;
			}

			bool has_hit = false;
			Hit closest_hit;

			for ( int i = 0; i < 2; i++ )
			{
				Hit h;
				Bvh* child = i == 0 ? child0 : child1;
				if ( child && child->Hits( ray, s_min, h ) )
				{
					if ( has_hit == false )
					{
						closest_hit = h;
						has_hit = true;
						continue;
					}
					if ( h.s < closest_hit.s )
					{
						closest_hit = h;
					}
				}
			}
			for ( auto hitable : hitables )
			{
				Hit h;
				if ( hitable->Hits( ray, s_min, h ) )
				{
					if ( has_hit == false )
					{
						closest_hit = h;
						has_hit = true;
						continue;
					}
					if ( h.s < closest_hit.s )
					{
						closest_hit = h;
					}
				}
			}
			if ( has_hit )
			{
				hit = closest_hit;
			}
			return has_hit;
		}

		Aabb bounds;
		std::vector<Hitable*> hitables;
		Bvh* child0;
		Bvh* child1;
	};

	Bvh bvh;

	void Add( Hitable* hitable )
	{
		bvh.Add( hitable, 0, 0 );
	}

	void BuildBvh( float time0, float time1, Bvh* node = nullptr, int level = 0 )
	{
		if ( node == nullptr )
		{
			node = &bvh;
		}

		assert( node->hitables.size() > 0 );

		if ( node->hitables.size() == 1 )
		{
			return;
		}

		assert( node->child0 == nullptr && node->child1 == nullptr );

		node->child0 = new Bvh;
		node->child1 = new Bvh;

		Aabb& bounds = node->bounds;
		bounds = Aabb();
		for ( auto hitable : node->hitables )
		{
			bounds = Union( bounds, hitable->GetBounds( time0, time1 ) );
		}

		V dims = bounds.max - bounds.min;
		int axis = 0;
		for ( int i = 1; i < 3; i++ )
		{
			if ( dims.v[ axis ] < dims.v[ i ] )
			{
				axis = i;
			}
		}

		float d = bounds.min.v[ axis ] + dims.v[ axis ] / 2.0f;

		int i = 0;
		for ( auto hitable : node->hitables )
		{
			Aabb b = hitable->GetBounds( time0, time1 );
			V bc = b.Center();
			float c = bc.v[ axis ];
			( ( c < d ) ? node->child0 : node->child1 )->Add( hitable, time0, time1 );
			i++;
		}

		if ( node->child0->hitables.size() == 0 || node->child1->hitables.size() == 0 )
		{
			delete node->child0;
			delete node->child1;
			node->child0 = nullptr;
			node->child1 = nullptr;
		}
		else
		{
			node->hitables.clear();
			BuildBvh( time0, time1, node->child0, level + 1 );
			BuildBvh( time0, time1, node->child1, level + 1 );
		}
	}

	Aabb GetBounds( float time0 = 0, float time1 = 0 ) const
	{
		return bvh.bounds;
	}

	bool Hits( const Ray& ray, float s_min, Hit& hit )
	{
		return bvh.Hits( ray, s_min, hit );
	}
};