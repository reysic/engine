#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Input/InputSystem.hpp"
#include "Engine/UI/Window.hpp"


//-----------------------------------------------------------------------------------------------
InputSystem* g_theInputSystem = nullptr;


//-----------------------------------------------------------------------------------------------
KeyButtonState::KeyButtonState()
{
	isKeyDown = false;
	didKeyJustChange = false;
}


//-----------------------------------------------------------------------------------------------
InputSystem::InputSystem()
{
	// Set all keys to "not down"
	for ( int keyIndex = 0; keyIndex < NUM_KEYS; ++keyIndex )
	{
		m_keyState[ keyIndex ].isKeyDown = false;
	}

	// Set all Xbox controller pointers to nullptr
	for ( int index = 0; index < MAX_XBOX_CONTROLLERS; ++index )
	{
		m_controllers[ index ] = nullptr;
	}

	// New off Xbox controllers
	for ( int index = 0; index < MAX_XBOX_CONTROLLERS; ++index )
	{
		m_controllers[ index ] = new XboxController( index );
	}

	m_appHasFocus = false;
	m_mouseCursorHidden = false;
}


//-----------------------------------------------------------------------------------------------
InputSystem::~InputSystem()
{
	// Delete Xbox controllers
	for ( int index = 0; index < MAX_XBOX_CONTROLLERS; ++index )
	{
		delete m_controllers[ index ];
		m_controllers[ index ] = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetKeyDownStatus( unsigned char windowsKeyCode, bool isNowDown )
{
	if ( m_keyState[ windowsKeyCode ].isKeyDown != isNowDown )
	{
		m_keyState[ windowsKeyCode ].didKeyJustChange = true;
	}

	m_keyState[ windowsKeyCode ].isKeyDown = isNowDown;
}


//-----------------------------------------------------------------------------------------------
void InputSystem::UpdateKeyJustPressedState()
{
	for ( int keyIndex = 0; keyIndex < NUM_KEYS; ++keyIndex )
	{
		m_keyState[ keyIndex ].didKeyJustChange = false;
	}
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::IsKeyDown( unsigned char windowsKeyCode ) const
{
	return m_keyState[ windowsKeyCode ].isKeyDown;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::IsKeyJustPressed( unsigned char windowsKeyCode ) const
{
	return m_keyState[ windowsKeyCode ].didKeyJustChange;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::IsKeyPressed( unsigned char windowsKeyCode )
{
	return ( IsKeyDown( windowsKeyCode ) && IsKeyJustPressed( windowsKeyCode ) );
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::GetLeftMouseButtonDownStatus() const
{
	return ( GetKeyState( VK_LBUTTON ) < 0 );
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::GetRightMouseButtonDownStatus() const
{
	return ( GetKeyState( VK_RBUTTON ) < 0 );
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::GetXboxADownStatus( unsigned char controllerIndex ) const
{
	return m_controllers[ controllerIndex ]->IsAButtonDown();
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::GetXboxBDownStatus( unsigned char controllerIndex ) const
{
	return m_controllers[ controllerIndex ]->IsBButtonDown();
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::GetXboxStartDownStatus( unsigned char controllerIndex ) const
{
	return m_controllers[ controllerIndex ]->IsStartButtonDown();
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::GetXboxRightTriggerPulledStatus( unsigned char controllerIndex ) const
{
	return m_controllers[ controllerIndex ]->IsRightTriggerPulled();
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::GetXboxLeftTriggerPulledStatus( unsigned char controllerIndex ) const
{
	return m_controllers[ controllerIndex ]->IsLeftTriggerPulled();
}


//-----------------------------------------------------------------------------------------------
Vector2 InputSystem::GetXboxLeftStickPosition( unsigned char controllerIndex ) const
{
	return m_controllers[ controllerIndex ]->GetLeftStickPosition();
}


//-----------------------------------------------------------------------------------------------
Vector2 InputSystem::GetXboxRightStickPosition( unsigned char controllerIndex ) const
{
	return m_controllers[ controllerIndex ]->GetRightStickPosition();
}


//-----------------------------------------------------------------------------------------------
PolarCoordinates InputSystem::GetXboxLeftStickPositionPolar( unsigned char controllerIndex ) const
{
	return m_controllers[ controllerIndex ]->GetLeftStickPositionPolar();
}


//-----------------------------------------------------------------------------------------------
PolarCoordinates InputSystem::GetXboxRightStickPositionPolar( unsigned char controllerIndex ) const
{
	return m_controllers[ controllerIndex ]->GetRightStickPositionPolar();
}


//-----------------------------------------------------------------------------------------------
void InputSystem::Update( float deltaSeconds )
{
	m_controllers[ 0 ]->Update( deltaSeconds );
	m_controllers[ 1 ]->Update( deltaSeconds );
	m_controllers[ 2 ]->Update( deltaSeconds );
	m_controllers[ 3 ]->Update( deltaSeconds );

	m_mouseCursorPosition = GetMouseCursorPosition() ;
	m_mouseCursorDelta = IntVector2( m_mouseCursorPosition.x - m_mouseCursorSnapbackPosition.x, m_mouseCursorPosition.y - m_mouseCursorSnapbackPosition.y );
}


//-----------------------------------------------------------------------------------------------
void InputSystem::ShowMouseCursor()
{
	ShowCursor( TRUE );
	m_mouseCursorHidden = false;
}


//-----------------------------------------------------------------------------------------------
void InputSystem::HideMouseCursor()
{
	ShowCursor( FALSE );
	m_mouseCursorHidden = true;
}


//-----------------------------------------------------------------------------------------------
IntVector2 InputSystem::GetMouseCursorPosition() const
{
	POINT cursorPos;
	GetCursorPos( &cursorPos );
	return IntVector2( cursorPos.x, cursorPos.y );
}


//-----------------------------------------------------------------------------------------------
IntVector2 InputSystem::GetMouseCursorPositionInWindow( Window* window )
{
	POINT cursorPos;
	GetCursorPos( &cursorPos );
	ScreenToClient( window->m_windowHandle, &cursorPos );
	return IntVector2( cursorPos.x, window->m_heightInPixels - cursorPos.y );
}


//-----------------------------------------------------------------------------------------------
IntVector2 InputSystem::GetMouseCursorDeltaSinceLastFrame() const
{
	return m_mouseCursorDelta;
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetMouseCursorPosition( const IntVector2& position )
{
	SetCursorPos( position.x, position.y );
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetMouseCursorSnapBackPosition( const IntVector2& position )
{
	m_mouseCursorSnapbackPosition = position;
}