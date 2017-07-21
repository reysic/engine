#include "Engine/Renderer/Transform.hpp"
#include "Engine/Math/Matrix4x4.hpp"


//-------------------------------------------------------------------------------------------------
Transform::Transform()
	: m_position( Vector3::ZERO )
	, m_rotation( mat44_fl::identity )
	, m_scale( Vector3::ONE )
{
}


//-------------------------------------------------------------------------------------------------
Transform::Transform( Vector3 const &position, mat44_fl const &rotation, Vector3 const &scale )
	: m_position( position )
	, m_rotation( rotation )
	, m_scale( scale )
{
}


//-------------------------------------------------------------------------------------------------
Transform::~Transform()
{
}


//-------------------------------------------------------------------------------------------------
mat44_fl Transform::GetModelMatrix() const
{
	mat44_fl scale = mat44_fl::identity;
	//scale.MakeScale(m_scale);
	MatrixMakeScale( &scale, m_scale.x );
	mat44_fl model = mat44_fl::identity;
	model = m_rotation;// .GetRotationMatrix( );
	model.data[ 12 ] = m_position.x;
	model.data[ 13 ] = m_position.y;
	model.data[ 14 ] = m_position.z;
	return scale * model;
}

