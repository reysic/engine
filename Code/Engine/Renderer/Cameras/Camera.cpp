#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Cameras/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Camera* g_theCamera = nullptr;


//-----------------------------------------------------------------------------------------------
Camera::Camera()
	: m_position( Vector3( -2.33f, -2.83f, 0.0f ) )
	, m_velocity( Vector3( 0.0f, 0.0f, 0.0f ) )
	, m_orientation( EulerAngles( 0.0f, 0.0f, 56.0f ) )
	, m_movingForward( false )
	, m_movingBackward( false )
	, m_movingLeft( false )
	, m_movingRight( false )
	, m_movingUp( false )
	, m_movingDown( false )
	, m_worldPosition( Vector2( 0.0f, 0.0f ) )
	, m_orientationDegrees( 0.0f )
	, m_numTilesInViewVertically( 20 )
{
	ResetPosition();
}


//-----------------------------------------------------------------------------------------------
Camera::~Camera()
{
}


//-----------------------------------------------------------------------------------------------
void Camera::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Camera::Shake( float magnitudeZeroToOne )
{
	const float MAX_SHAKE_DISPLACEMENT = 20.0f;

	float cameraShakeMagnitude = magnitudeZeroToOne;

	cameraShakeMagnitude *= MAX_SHAKE_DISPLACEMENT;

	Vector3 screenShakeOffset( GetRandomFloatNegativeOneToOne() * cameraShakeMagnitude, GetRandomFloatNegativeOneToOne() * cameraShakeMagnitude, 0.0f );

	g_theRenderer->TranslateView( screenShakeOffset );
}


//-----------------------------------------------------------------------------------------------
void Camera::FixAndClampAngles()
{
	while ( m_orientation.m_yawDegreesAboutZ < 0.0f )
	{
		m_orientation.m_yawDegreesAboutZ += 360.0f;
	}

	while ( m_orientation.m_yawDegreesAboutZ >= 360.0f )
	{
		m_orientation.m_yawDegreesAboutZ -= 360.0f;
	}

	while ( m_orientation.m_rollDegreesAboutY < 0.0f )
	{
		m_orientation.m_rollDegreesAboutY += 360.0f;
	}

	while ( m_orientation.m_rollDegreesAboutY >= 360.0f )
	{
		m_orientation.m_rollDegreesAboutY -= 360.0f;
	}

	m_orientation.m_pitchDegreesAboutX = ClampFloat( m_orientation.m_pitchDegreesAboutX, -89.9f, 89.9f );
}


//-----------------------------------------------------------------------------------------------
void Camera::ResetPosition()
{
	m_position = Vector3( 0.0f, -2.83f, 0.0f );
	m_orientation = EulerAngles( 0.0f, 0.0f, 0.0f );
}


//-----------------------------------------------------------------------------------------------
void Camera::SetUp2DView()
{
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) );
}


//-----------------------------------------------------------------------------------------------
void Camera::SetUp3DView()
{
	// View matrix
	g_theRenderer->m_view.MatrixMakeRotationEuler( -m_orientation.m_yawDegreesAboutZ, 
		m_orientation.m_pitchDegreesAboutX, 0.f, Vector3( 0.f, 0.f, 0.f ) );
	g_theRenderer->m_view.TransposeRotation();
	g_theRenderer->m_view.NegateTranslation( m_position );

	// Projection matrix
	float aspect = 16.f / 9.f;
	g_theRenderer->m_projection.ChangeToPerspectiveProjection( 70.0f, aspect, 0.1f, 100.0f );
	Matrix4x4 COB = Matrix4x4();
	COB.SetBasis( Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ), Vector3( 0.f, 1.f, 0.f ) );
	Matrix4x4 newProj = COB * g_theRenderer->m_projection;
	g_theRenderer->m_projection = newProj;
}


//-----------------------------------------------------------------------------------------------
Vector3 Camera::GetForwardXYZ() const
{
	float cosYaw = CosDegrees( m_orientation.m_yawDegreesAboutZ );
	float sinYaw = SinDegrees( m_orientation.m_yawDegreesAboutZ );

	float cosPitch = CosDegrees( m_orientation.m_pitchDegreesAboutX );
	float sinPitch = SinDegrees( m_orientation.m_pitchDegreesAboutX );

	return Vector3( sinYaw * cosPitch, cosYaw * cosPitch, sinPitch );
}


//-----------------------------------------------------------------------------------------------
Vector3 Camera::GetForwardXY() const
{
	float cosYaw = CosDegrees( m_orientation.m_yawDegreesAboutZ );
	float sinYaw = SinDegrees( m_orientation.m_yawDegreesAboutZ );
	return Vector3( sinYaw, cosYaw, 0.0f );
}


//-----------------------------------------------------------------------------------------------
Vector3 Camera::GetLeftXY() const
{
	Vector3 forwardXY = GetForwardXY();
	return Vector3( -forwardXY.y, forwardXY.x, 0.0f );
}


//-----------------------------------------------------------------------------------------------
WorldPosition Camera::GetWorldPosition() const
{
	return m_worldPosition;
}


//-----------------------------------------------------------------------------------------------
float Camera::GetOrientationDegrees() const
{
	return m_orientationDegrees;
}


//-----------------------------------------------------------------------------------------------
float Camera::GetNumTilesInViewVertically() const
{
	return m_numTilesInViewVertically;
}


//-----------------------------------------------------------------------------------------------
void Camera::SetWorldPosition( WorldPosition newWorldPosition )
{
	m_worldPosition = newWorldPosition;
}


//-----------------------------------------------------------------------------------------------
void Camera::SetOrientationDegrees( float newOrientationDegrees )
{
	m_orientationDegrees = newOrientationDegrees;
}


//-----------------------------------------------------------------------------------------------
void Camera::SetNumTilesInViewVertically( float newNumTilesInViewVertically )
{
	m_numTilesInViewVertically = newNumTilesInViewVertically;
}