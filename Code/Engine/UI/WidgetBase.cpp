#include "Engine/UI/WidgetBase.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
bool WidgetBase::IsMouseCursorWithinBounds() const
{
	IntVector2 mouseCursorPositionWindow = g_theInputSystem->GetMouseCursorPositionInWindow( g_appWindow );

	Vector2 widgetDimensions = GetStateSpecificProperty< Vector2 >( "WidgetDimensions" );
	Vector2 widgetCenterCoords = GetStateSpecificProperty< Vector2 >( "CenterCoords" );
	Vector2 widgetMins = Vector2::ZERO;
	Vector2 widgetMaxs = Vector2::ZERO;

	int anchorPositionAsInt = GetStateSpecificProperty< int >( "AnchorPosition" );
	switch ( anchorPositionAsInt )
	{
		case 0:
		{
			// ANCHOR_POSITION_TOP_LEFT
			widgetMins = Vector2( 0, ( float ) g_appWindow->m_heightInPixels - widgetDimensions.y );
			widgetMaxs = Vector2( widgetDimensions.x, ( float ) g_appWindow->m_heightInPixels );
			widgetCenterCoords = Vector2( widgetMins.x + widgetDimensions.x / 2, widgetMins.y + widgetDimensions.y / 2 );
			break;
		}
		default:
		{
			widgetMins = Vector2( widgetCenterCoords.x - ( widgetDimensions.x / 2 ), widgetCenterCoords.y - ( widgetDimensions.y / 2 ) );
			widgetMaxs = Vector2( widgetCenterCoords.x + ( widgetDimensions.x / 2 ), widgetCenterCoords.y + ( widgetDimensions.y / 2 ) );
			break;
		}
	}

	if ( mouseCursorPositionWindow.x > widgetMins.x && mouseCursorPositionWindow.x < widgetMaxs.x &&
		mouseCursorPositionWindow.y > widgetMins.y && mouseCursorPositionWindow.y < widgetMaxs.y )
	{
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
bool WidgetBase::WasWidgetJustClicked()
{
	if ( IsMouseCursorWithinBounds() )
	{
		if ( g_theInputSystem->IsKeyPressed( VK_LBUTTON ) )
		{
			return true;
		}

		return false;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
void WidgetBase::SetWidgetPropertiesFromXML( NamedProperties& widgetProperties, const XMLAttribute& xmlAttribute )
{
	const char* widgetPropertyName = xmlAttribute.lpszName;
	std::string widgetPropertyValue = xmlAttribute.lpszValue;

	PropertyType widgetPropertyType = WidgetProperty::GetPropertyTypeForName( widgetPropertyName );

	switch ( widgetPropertyType )
	{
		case ( PROPERTY_TYPE_INT ):
		{
			int widgetProperty = ParseXMLValueAsInt( widgetPropertyValue );
			widgetProperties.Set( widgetPropertyName, widgetProperty );
			break;
		}
		case ( PROPERTY_TYPE_FLOAT ):
		{
			float widgetProperty = ParseXMLValueAsFloat( widgetPropertyValue );
			widgetProperties.Set( widgetPropertyName, widgetProperty );
			break;
		}
		case ( PROPERTY_TYPE_VECTOR2 ):
		{
			Vector2 widgetProperty = ParseXMLValueAsVector2( widgetPropertyValue );
			widgetProperties.Set( widgetPropertyName, widgetProperty );
			break;
		}
		case ( PROPERTY_TYPE_STRING ):
		{
			std::string widgetProperty = widgetPropertyValue;
			widgetProperties.Set( widgetPropertyName, widgetProperty );
			break;
		}
		case ( PROPERTY_TYPE_RGBA ):
		{
			Rgba widgetProperty = ParseXMLValueAsRgba( widgetPropertyValue );
			widgetProperties.Set( widgetPropertyName, widgetProperty );
			break;
		}
		case ( NUM_PROPERTY_TYPES ):
		{
			break;
		}
		default:
		{
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
WidgetState WidgetBase::GetStateForStateName( const char* stateName ) const
{
	if ( strcmp( stateName, "ActiveState" ) == 0 )
	{
		return WIDGET_STATE_ACTIVE;
	}
	else if ( strcmp( stateName, "HighlightedState" ) == 0 )
	{
		return WIDGET_STATE_HIGHLIGHTED;
	}
	else if ( strcmp( stateName, "PressedState" ) == 0 )
	{
		return WIDGET_STATE_PRESSED;
	}

	return NUM_WIDGET_STATES;
}