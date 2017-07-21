#include "Engine/UI/ButtonWidget.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"


//-----------------------------------------------------------------------------------------------
void ButtonWidget::Update( float deltaSeconds )
{
	deltaSeconds;
}


//-----------------------------------------------------------------------------------------------
void ButtonWidget::Render() const
{
	g_theRenderer->DepthTestingEnabled( false );

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	// Draw the button itself
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

	std::string texturePath = GetStateSpecificProperty< std::string >( "TexturePath" );
	if ( texturePath != "" )
	{
		g_theRenderer->DrawTexturedQuad3D( Vector3( widgetMins.x, widgetMins.y, 0.0f ),
			Vector3( widgetMaxs.x, widgetMins.y, -1.0f ),
			Vector3( widgetMaxs.x, widgetMaxs.y, -1.0f ),
			Vector3( widgetMins.x, widgetMaxs.y, -1.0f ),
			*Texture::CreateOrGetTexture( texturePath.c_str() ),
			Vector2( 0.0f, 0.0f ),
			Vector2( 1.0f, 1.0f ),
			GetStateSpecificProperty< Rgba >( "BackgroundColor" ) );
	}
	else
	{
		g_theRenderer->DrawQuad( widgetMins, widgetMaxs, GetStateSpecificProperty< Rgba >( "BackgroundColor" ) );
	}

	// Draw the border
	int borderWidth = GetStateSpecificProperty< int >( "BorderWidth" );
	Rgba borderColor = GetStateSpecificProperty< Rgba >( "BorderColor" );
	g_theRenderer->DrawQuad( Vector2( widgetMins .x - borderWidth, widgetMins.y ), Vector2( widgetMins.x, widgetMaxs.y ), borderColor );
	g_theRenderer->DrawQuad( Vector2( widgetMaxs.x, widgetMins.y ), Vector2( widgetMaxs.x + borderWidth, widgetMaxs.y ), borderColor );
	g_theRenderer->DrawQuad( Vector2( widgetMins.x - borderWidth, widgetMaxs.y ), Vector2( widgetMaxs.x + borderWidth, widgetMaxs.y + borderWidth ), borderColor );
	g_theRenderer->DrawQuad( Vector2( widgetMins.x - borderWidth, widgetMins.y - borderWidth ), Vector2( widgetMaxs.x + borderWidth, widgetMins.y ), borderColor );

 	// Draw button text
	std::string textStr = GetStateSpecificProperty< std::string >( "TextString" );
	float textCellHeight = GetStateSpecificProperty< float >( "TextCellHeight" );
	Rgba textColor = GetStateSpecificProperty< Rgba >( "TextColor" );
	float textWidth = BitmapFont::GetTextWidth( textStr, textCellHeight );
	Vector2 textDrawPosition = Vector2( widgetCenterCoords.x - ( textWidth / 2.0f ), widgetCenterCoords.y - ( textCellHeight / 2.0f ) );
	g_theRenderer->DrawText2D( textDrawPosition, textStr, textCellHeight, textColor, fixedFont );
}