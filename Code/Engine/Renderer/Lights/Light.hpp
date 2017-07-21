#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"


//-----------------------------------------------------------------------------------------------
class Light
{
public:
	Light();
	Light( const Vector4& color, const Vector3& position, const Vector3& direction,
		float minLightDistance, float maxLightDistance, float poewrAtMinDistance,
		float powerAtMaxDistance, float directionalInterp, float thetaInner, float thetaOuter,
		float thetaInnerPower, float thetaOuterPower );
	Light( const Vector4& color, const Vector3& position, float radius );

public:
	Vector4 m_color;
	Vector3 m_position;
	Vector3 m_direction;
	float m_minLightDistance;
	float m_maxLightDistance;
	float m_powerAtMinDistance;
	float m_powerAtMaxDistance;
	float m_directionalInterp;
	float m_thetaInner;
	float m_thetaOuter;
	float m_thetaInnerPower;
	float m_thetaOuterPower;
};