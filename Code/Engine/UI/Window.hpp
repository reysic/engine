#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Math/IntVector2.hpp"


//-----------------------------------------------------------------------------------------------
class Window
{
public:
	Window( int widthInPixels, int heightInPixels, HWND windowHandle );
	void Update( float deltaSeconds );
	IntVector2 GetCenterPosition();

public:
	int m_widthInPixels;
	int m_heightInPixels;
	HWND m_windowHandle;
};