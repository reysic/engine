#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <xinput.h> // include the Xinput API

#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library

#include <math.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Input/XboxController.hpp"


//-----------------------------------------------------------------------------------------------
XboxController::XboxController( unsigned int controllerNumber )
	: m_controllerNumber( controllerNumber )
{
	rawLeftThumbStickPosition.x = 0.0f;
	rawLeftThumbStickPosition.y = 0.0f;
	leftThumbStickPositionPolar = ConvertRawStickPositionToPolar( rawLeftThumbStickPosition, static_cast < float > ( XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ) );
	rightThumbStickPositionPolar = ConvertRawStickPositionToPolar( rawRightThumbStickPosition, static_cast < float > ( XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ) );
}


//-----------------------------------------------------------------------------------------------
void XboxController::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( m_controllerNumber, &xboxControllerState );
	
	if ( errorStatus == ERROR_SUCCESS ) 
	{
		// Check button state
		buttons = xboxControllerState.Gamepad.wButtons;

		// Check stick state
		rawLeftThumbStickPosition.x = static_cast <float> ( xboxControllerState.Gamepad.sThumbLX );
		rawLeftThumbStickPosition.y = static_cast <float> ( xboxControllerState.Gamepad.sThumbLY );
		rawRightThumbStickPosition.x = static_cast <float> ( xboxControllerState.Gamepad.sThumbRX );
		rawRightThumbStickPosition.y = static_cast <float> ( xboxControllerState.Gamepad.sThumbRY );

		leftThumbStickPositionPolar = ConvertRawStickPositionToPolar( rawLeftThumbStickPosition, static_cast <float> ( XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ) );
		rightThumbStickPositionPolar = ConvertRawStickPositionToPolar( rawRightThumbStickPosition, static_cast <float> ( XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ) );

		// Check trigger state

		rawLeftTriggerValue = xboxControllerState.Gamepad.bLeftTrigger;
		rawRightTriggerValue = xboxControllerState.Gamepad.bRightTrigger;

		connected = true;
	}
	else if ( errorStatus == ERROR_DEVICE_NOT_CONNECTED ) 
	{
		connected = false;
	}
}


//-----------------------------------------------------------------------------------------------
PolarCoordinates XboxController::ConvertRawStickPositionToPolar( const Vector2& rawStickPosition, float deadzoneRadius )
{
	float xNormalized = rawStickPosition.x / 32000.0f;
	float yNormalized = rawStickPosition.y / 32000.0f;

	Vector2 normalizedVector( xNormalized, yNormalized );
	float radius = normalizedVector.Length();
	float angle = atan2( yNormalized, xNormalized );

	float rangeMappedRadius = RangeMapZeroToOne( radius, ( deadzoneRadius / 32768.0f ), ( 32000.0f / 32768.0f) );

	return PolarCoordinates( rangeMappedRadius, angle );
}


//-----------------------------------------------------------------------------------------------
bool XboxController::IsAButtonDown() const
{
	return ( ( buttons & 0x1000 ) != 0 );
}


//-----------------------------------------------------------------------------------------------
bool XboxController::IsBButtonDown() const
{
	return ( ( buttons & 0x2000 ) != 0 );
}


//-----------------------------------------------------------------------------------------------
bool XboxController::IsStartButtonDown() const
{
	return ( ( buttons & 0x0010 ) != 0 );
}


//-----------------------------------------------------------------------------------------------
PolarCoordinates XboxController::GetLeftStickPositionPolar() const
{
	return leftThumbStickPositionPolar;
}


//-----------------------------------------------------------------------------------------------
PolarCoordinates XboxController::GetRightStickPositionPolar() const
{
	return rightThumbStickPositionPolar;
}


//-----------------------------------------------------------------------------------------------
Vector2 XboxController::GetLeftStickPosition() const
{
	return Vector2( leftThumbStickPositionPolar.radius * cos( leftThumbStickPositionPolar.theta ), leftThumbStickPositionPolar.radius * sin( leftThumbStickPositionPolar.theta ) );
}
	

//-----------------------------------------------------------------------------------------------
Vector2 XboxController::GetRightStickPosition() const
{
	return Vector2( rightThumbStickPositionPolar.radius * cos( rightThumbStickPositionPolar.theta ), rightThumbStickPositionPolar.radius * sin( rightThumbStickPositionPolar.theta ) );
}


//-----------------------------------------------------------------------------------------------
float XboxController::GetLeftTriggerValue() const 
{
	float rawLeftTriggerValueFloat = static_cast <float> ( rawLeftTriggerValue );
	return RangeMapZeroToOne( rawLeftTriggerValueFloat, static_cast <float> ( XINPUT_GAMEPAD_TRIGGER_THRESHOLD ), 255.0f );
}


//-----------------------------------------------------------------------------------------------
float XboxController::GetRightTriggerValue() const
{
	float rawRightTriggerValueFloat = static_cast <float> ( rawRightTriggerValue );
	return RangeMapZeroToOne( rawRightTriggerValueFloat, static_cast <float> (XINPUT_GAMEPAD_TRIGGER_THRESHOLD), 255.0f );
}


//-----------------------------------------------------------------------------------------------
bool XboxController::IsConnected() const
{
	return connected;
}