#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/EulerAngles.hpp"


//-----------------------------------------------------------------------------------------------
class Camera;
class EulerAngles;


//-----------------------------------------------------------------------------------------------
extern Camera* g_theCamera;


//-----------------------------------------------------------------------------------------------
typedef Vector2 WorldPosition;
class Camera
{
public:
	// Constructors & Destructors
	Camera();
	~Camera();

	void Update( float deltaSeconds );
	void Shake( float magnitudeZeroToOne );

	void FixAndClampAngles();
	void ResetPosition();
	void SetUp2DView();
	void SetUp3DView();

	// Accessors
	Vector3 GetForwardXYZ() const;
	Vector3 GetForwardXY() const;
	Vector3 GetLeftXY() const;
	WorldPosition GetWorldPosition() const;
	float GetOrientationDegrees() const;
	float GetNumTilesInViewVertically() const;

	// Mutators
	void SetWorldPosition( WorldPosition newWorldPosition );
	void SetOrientationDegrees( float newOrientationDegrees );
	void SetNumTilesInViewVertically( float newNumTilesInViewVertically );

public:
	// Members
	Vector3			m_position;
	Vector3			m_velocity;
	bool			m_movingForward;
	bool			m_movingBackward;
	bool			m_movingLeft;
	bool			m_movingRight;
	bool			m_movingUp;
	bool			m_movingDown;
	EulerAngles		m_orientation;
	WorldPosition	m_worldPosition;
	float			m_orientationDegrees;
	float			m_numTilesInViewVertically;
};