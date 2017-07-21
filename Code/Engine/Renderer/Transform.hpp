#pragma once

#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Vector3.hpp"


//-------------------------------------------------------------------------------------------------
class Transform
{
private:
	Vector3 m_position;
	mat44_fl m_rotation;
	Vector3 m_scale;

public:
	Transform();
	Transform( Vector3 const &position, mat44_fl const &rotation, Vector3 const &scale );
	~Transform();

	Vector3 GetPosition() const { return m_position; }
	mat44_fl GetRotation() const { return m_rotation; }
	Vector3 GetScale() const { return m_scale; }
	void SetPosition( Vector3 const &position ) { m_position = position; }
	void SetRotation( mat44_fl const &rotation ) { m_rotation = rotation; }
	void SetScale( Vector3 const &scale ) { m_scale = scale; }

	mat44_fl GetModelMatrix() const;
};