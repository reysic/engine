#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
const int NUM_KEYS = 256;


//-----------------------------------------------------------------------------------------------
const int MAX_XBOX_CONTROLLERS = 4;


//-----------------------------------------------------------------------------------------------
class InputSystem;


//-----------------------------------------------------------------------------------------------
class XboxController; // forward declaration of XboxController
class Vector2; // forward declaration of Vector2
class PolarCoordinates;


//-----------------------------------------------------------------------------------------------
extern InputSystem* g_theInputSystem;


//-----------------------------------------------------------------------------------------------
struct KeyButtonState
{
	bool isKeyDown;
	bool didKeyJustChange;
	KeyButtonState::KeyButtonState();
};

//-----------------------------------------------------------------------------------------------
struct MouseButtonState
{

};
//-----------------------------------------------------------------------------------------------
class InputSystem
{
public:
	InputSystem();
	~InputSystem();
	void SetKeyDownStatus( unsigned char windowsKeyCode, bool isNowDown );
	void UpdateKeyJustPressedState();
	bool IsKeyDown( unsigned char windowsKeyCode ) const;
	bool IsKeyJustPressed( unsigned char windowsKeyCode ) const;
	bool IsKeyPressed( unsigned char windowsKeyCode );
	bool GetLeftMouseButtonDownStatus() const;
	bool GetRightMouseButtonDownStatus() const;
	bool GetXboxADownStatus( unsigned char controllerIndex ) const;
	bool GetXboxBDownStatus( unsigned char controllerIndex ) const;
	bool GetXboxStartDownStatus( unsigned char controllerIndex ) const;
	bool GetXboxRightTriggerPulledStatus( unsigned char controllerIndex ) const;
	bool GetXboxLeftTriggerPulledStatus( unsigned char controllerIndex ) const;
	Vector2 GetXboxLeftStickPosition( unsigned char controllerIndex ) const;
	Vector2 GetXboxRightStickPosition( unsigned char controllerIndex ) const;
	PolarCoordinates GetXboxLeftStickPositionPolar( unsigned char controllerIndex ) const;
	PolarCoordinates GetXboxRightStickPositionPolar( unsigned char controllerIndex ) const;
	void Update( float deltaSeconds );
	void ShowMouseCursor();
	void HideMouseCursor();
	IntVector2 GetMouseCursorPosition() const;
	IntVector2 GetMouseCursorPositionInWindow( Window* window );
	IntVector2 GetMouseCursorDeltaSinceLastFrame() const;
	void SetMouseCursorPosition( const IntVector2& position );
	void SetMouseCursorSnapBackPosition( const IntVector2& position );

	XboxController* m_controllers[ MAX_XBOX_CONTROLLERS ];

public:
	bool m_appHasFocus;
	bool m_mouseCursorHidden;
	IntVector2 m_mouseCursorPosition;
	IntVector2 m_mouseCursorDelta;
	IntVector2 m_mouseCursorSnapbackPosition;

private:
	KeyButtonState m_keyState[ NUM_KEYS ];
};