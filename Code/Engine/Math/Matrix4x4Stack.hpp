#pragma once

#include <vector>

#include "Engine/Math/Matrix4x4.hpp"


//-----------------------------------------------------------------------------------------------
class Matrix4x4Stack
{
public:
	std::vector< mat44_fl > matrices;

	Matrix4x4Stack()
	{
		matrices.push_back( mat44_fl::identity );
	}

	bool IsEmpty()
	{
		return ( matrices.size() == 1 );
	}

	mat44_fl& GetTop()
	{
		return matrices.back();
	}

	void Push( const mat44_fl& transform )
	{
		mat44_fl top = GetTop();

		// If your engine multiplies vectors on the left (v * T)
		//T new_top = transform * top;

		// If your engine multiplies vectors on the right (T * v)
		mat44_fl new_top = top * transform;

		matrices.push_back( new_top );
	}

	void Pop()
	{
		if ( !IsEmpty() ) {
			matrices.pop_back();
		}
	}
};