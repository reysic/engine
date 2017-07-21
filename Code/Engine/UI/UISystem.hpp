#pragma once

#include <vector>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/UI/Window.hpp"
#include "Engine/UI/WidgetProperty.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"


//-----------------------------------------------------------------------------------------------
class UISystem;
class WidgetBase;


//-----------------------------------------------------------------------------------------------
extern UISystem* g_theUISystem;


//-----------------------------------------------------------------------------------------------
class UISystem
{
public:
	UISystem();

	void Update( float deltaSeconds );
	void CheckForUIReloadKey();
	void Render() const;

	void LoadUIFromXML();
	void LoadStateSpecificProperties( const XMLNode& widgetNode, const char* stateName, WidgetBase* currentWidget ) const;
	void RegisterUIEvents();

	void PlayButtonSound( NamedProperties& namedProperties );
	void MakeExitButton( NamedProperties& namedProperties );
	void ExitApplication( NamedProperties& namedProperties );

	AABB2 CalculateWidgetBounds( int widgetWidth, int widgetHeight,
		Vector2 widgetCenterPosition );

public:
	std::vector< WidgetBase* > m_widgets;
};