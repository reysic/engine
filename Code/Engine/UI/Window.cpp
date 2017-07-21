#include "Engine/UI/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"


//-----------------------------------------------------------------------------------------------
Window::Window( int widthInPixels, int heightInPixels, HWND windowHandle )
	: m_widthInPixels( widthInPixels )
	, m_heightInPixels( heightInPixels )
	, m_windowHandle( windowHandle )
{
}


//-----------------------------------------------------------------------------------------------
void Window::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if ( GetFocus() == m_windowHandle )
	{
		g_theInputSystem->m_appHasFocus = true;
	}
	else
	{
		g_theInputSystem->m_appHasFocus = false;
	}
}


//-----------------------------------------------------------------------------------------------
IntVector2 Window::GetCenterPosition()
{
	return IntVector2( m_widthInPixels / 2, m_heightInPixels / 2 );
}