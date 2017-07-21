#pragma once

#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
class PolarCoordinates;


//-----------------------------------------------------------------------------------------------
const int TRIGGER_PULLED_THRESHOLD = 150;


//-----------------------------------------------------------------------------------------------
class XboxController
{
public:
	XboxController( unsigned int controllerNumber );
	void Update( float deltaSeconds );
	PolarCoordinates ConvertRawStickPositionToPolar( const Vector2& rawStickPosition, float deadzoneRadius );
	bool IsAButtonDown() const;
	bool IsBButtonDown() const;
	bool IsStartButtonDown() const;
	bool IsLeftTriggerPulled() const { return ( rawLeftTriggerValue > TRIGGER_PULLED_THRESHOLD ); };
	bool IsRightTriggerPulled() const { return ( rawRightTriggerValue > TRIGGER_PULLED_THRESHOLD ); };
	PolarCoordinates GetLeftStickPositionPolar() const; // Returns left stick position in polar coordinates, corrected for deadzone, radius from 0 to 1
	PolarCoordinates GetRightStickPositionPolar() const; // Returns left stick position in polar coordinates, corrected for deadzone, radius from 0 to 1
	Vector2 GetLeftStickPosition() const; // Converts from polar, deadzone-corrected, position to cartesian
	Vector2 GetRightStickPosition() const; // Converts from polar, deadzone-corrected, position to cartesian
	float GetLeftTriggerValue() const; // Returns a value from 0 to 1
	float GetRightTriggerValue() const; // Returns a value from 0 to 1
	bool IsConnected() const;

private:
	unsigned int m_controllerNumber;
	unsigned short buttons;
	Vector2 rawLeftThumbStickPosition;
	Vector2 rawRightThumbStickPosition;
	PolarCoordinates leftThumbStickPositionPolar;
	PolarCoordinates rightThumbStickPositionPolar;
	int rawLeftTriggerValue;
	int rawRightTriggerValue;
	bool connected;
};