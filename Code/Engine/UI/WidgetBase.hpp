#pragma once

#include <string>

#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/UI/UISystem.hpp"


//-----------------------------------------------------------------------------------------------
enum AnchorPosition : unsigned char
{
	ANCHOR_POSITION_TOP_LEFT = 0,
	ANCHOR_POSITION_TOP_CENTER,
	ANCHOR_POSITION_TOP_RIGHT,
	ANCHOR_POSITION_MIDDLE_LEFT,
	ANCHOR_POSITION_MIDDLE_CENTER,
	ANCHOR_POSITION_MIDDLE_RIGHT,
	ANCHOR_POSITION_BOTTOM_LEFT,
	ANCHOR_POSITION_BOTTOM_CENTER,
	ANCHOR_POSITION_BOTTOM_RIGHT,
	NUM_ANCHOR_POSITIONS
};


//-----------------------------------------------------------------------------------------------
enum WidgetState : unsigned char
{
	WIDGET_STATE_ACTIVE = 0,
	WIDGET_STATE_HIGHLIGHTED,
	WIDGET_STATE_PRESSED,
	NUM_WIDGET_STATES
};


//-----------------------------------------------------------------------------------------------
class WidgetBase
{
public:
	WidgetBase() { }
	virtual void Update( float deltaSeconds ) = 0;
	virtual void Render() const = 0;
	bool IsMouseCursorWithinBounds() const;
	bool WasWidgetJustClicked();

	template < typename T >
	T GetCommonProperty( const std::string& propertyName ) const
	{
		T propertyValue;
		m_commonProperties.Get( propertyName, propertyValue );
		return propertyValue;
	}

	template < typename T >
	T GetStateSpecificProperty( const std::string& propertyName ) const
	{
		T propertyValue;
		m_stateSpecificProperties[ m_currentState ].Get( propertyName, propertyValue );
		return propertyValue;
	}

	void SetWidgetPropertiesFromXML( NamedProperties& widgetProperties, const XMLAttribute& xmlAttribute );
	WidgetState GetStateForStateName( const char* stateName ) const; // Can be moved to UISystem.hpp

public:
	WidgetBase* m_parentWidget;
	WidgetState m_currentState;

	NamedProperties m_commonProperties;
	NamedProperties m_stateSpecificProperties[ NUM_WIDGET_STATES ];
};