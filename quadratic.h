#pragma once

#include <cmath>

template <typename T> struct Quadratic
{
	union {
		struct
		{
			T a, b, c;
		};
		T co[ 3 ];
	};

	Quadratic( T a_, T b_, T c_ )
	{
		a = a_;
		b = b_;
		c = c_;
	}

	T Discriminant() const
	{
		return b * b - 4 * a * c;
	}

	T ClosestForwardSolution( T discr )
	{
		if ( discr < 0 )
		{
			return -1;
		}
		T rt_discr = std::sqrt( discr );
		if ( -b > rt_discr )
		{
			return ( -b - std::sqrt( discr ) ) / ( T( 2 ) * a );
		}
		else if ( b < rt_discr )
		{
			return ( -b + std::sqrt( discr ) ) / ( T( 2 ) * a );
		}
		return -1;
	}
};

typedef Quadratic<float> Quadraticf;
