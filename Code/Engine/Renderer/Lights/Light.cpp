#include "Engine/Renderer/Lights/Light.hpp"


//-----------------------------------------------------------------------------------------------
Light::Light()
	: m_color( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) )
	, m_position( Vector3( 0.0f, 0.0f, 0.0f ) )
	, m_direction( Vector3( 0.0f, 0.0f, 0.0f ) )
	, m_minLightDistance( 0.0f )
	, m_maxLightDistance( 0.0f )
	, m_powerAtMinDistance( 0.0f )
	, m_powerAtMaxDistance( 0.0f )
	, m_directionalInterp( 0.0f )
	, m_thetaInner( 0.0f )
	, m_thetaOuter( 0.0f )
	, m_thetaInnerPower( 0.0f )
	, m_thetaOuterPower( 0.0f )
{
};


//-----------------------------------------------------------------------------------------------
Light::Light( const Vector4& color, const Vector3& position, const Vector3& direction,
	float minLightDistance, float maxLightDistance, float powerAtMinDistance,
	float powerAtMaxDistance, float directionalInterp, float thetaInner, float thetaOuter,
	float thetaInnerPower, float thetaOuterPower )
	: m_color( color )
	, m_position( position )
	, m_direction( direction )
	, m_minLightDistance( minLightDistance )
	, m_maxLightDistance( maxLightDistance )
	, m_powerAtMinDistance( powerAtMinDistance )
	, m_powerAtMaxDistance( powerAtMaxDistance )
	, m_directionalInterp( directionalInterp )
	, m_thetaInner( thetaInner )
	, m_thetaOuter( thetaOuter )
	, m_thetaInnerPower( thetaInnerPower )
	, m_thetaOuterPower( thetaOuterPower )
{
}


//-----------------------------------------------------------------------------------------------
Light::Light( const Vector4& color, const Vector3& position, float radius )
	: m_color( color )
	, m_position( position )
	, m_direction( Vector3( 0.0f, 0.0f, 0.0f ) )
	, m_minLightDistance( radius - 1.0f )
	, m_maxLightDistance( radius + 1.0f )
	, m_powerAtMinDistance( 1.0f )
	, m_powerAtMaxDistance( 0.0f )
	, m_directionalInterp( 0.0f )
	, m_thetaInner( 1.0f )
	, m_thetaOuter( -1.0f )
	, m_thetaInnerPower( 1.0f )
	, m_thetaOuterPower( 1.0f )
{
}