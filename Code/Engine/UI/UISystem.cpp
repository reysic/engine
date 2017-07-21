#include "Engine/UI/UISystem.hpp"
#include "Engine/UI/ButtonWidget.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"


//-----------------------------------------------------------------------------------------------
UISystem*	g_theUISystem = nullptr;
bool		g_waitForRKeyUp = false;


//-----------------------------------------------------------------------------------------------
UISystem::UISystem()
{
	LoadUIFromXML();
	RegisterUIEvents();
}


//-----------------------------------------------------------------------------------------------
void UISystem::Update( float deltaSeconds )
{
	deltaSeconds;

	CheckForUIReloadKey();

	bool haveHighlightedAWidget = false;

	for ( int widgetIndex = ( m_widgets.size() - 1 ); widgetIndex > -1; --widgetIndex )
	{
		WidgetBase* thisWidget = m_widgets[ widgetIndex ];
		if ( thisWidget->WasWidgetJustClicked() )
		{
			std::string eventNameProperty = thisWidget->GetStateSpecificProperty< std::string >( "EventName" );
			NamedProperties p;
			EventSystem::Instance()->FireEvent( eventNameProperty, p );
			return;
		}

		if ( thisWidget->IsMouseCursorWithinBounds() && !haveHighlightedAWidget )
		{
			thisWidget->m_currentState = WIDGET_STATE_HIGHLIGHTED;
			haveHighlightedAWidget = true;
		}
		else
		{
			thisWidget->m_currentState = WIDGET_STATE_ACTIVE;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void UISystem::CheckForUIReloadKey()
{
	if ( g_theInputSystem->IsKeyDown( 'R' ) && !g_waitForRKeyUp )
	{
		m_widgets.clear();
		LoadUIFromXML();
		g_waitForRKeyUp = true;
	}

	if ( !( g_theInputSystem->IsKeyDown( 'R' ) ) )
	{
		g_waitForRKeyUp = false;
	}
}


//-----------------------------------------------------------------------------------------------
void UISystem::Render() const
{
	for ( WidgetBase* widget : m_widgets )
	{
		widget->Render();
	}
}


//-----------------------------------------------------------------------------------------------
void UISystem::LoadUIFromXML()
{
	std::vector< std::string > uiFiles = EnumerateFilesInFolder( "Data/UI", "*.xml" );

	for ( std::string file : uiFiles )
	{
		XMLNode node = XMLNode::openFileHelper( file.c_str() );
		XMLNode rootNode = node.getChildNode( 0 ); // UIWidgets

		if ( !rootNode.IsContentEmpty() )
		{
			int widgetCount = rootNode.nChildNode(); // # of UIWidget nodes

			for ( int i = 0; i < widgetCount; i++ )
			{
				XMLNode thisNode = rootNode.getChildNode( i );
				std::string name = thisNode.getName();
				if ( name != "UIWidget" )
				{
					// Not a valid node, skip
					continue;
				}

				// Valid node, so create the widget
				if ( thisNode.getAttribute( "WidgetType" ) )
				{
					std::string typeStr = thisNode.getAttribute( "WidgetType" ); // Get the widget type

					if ( typeStr == "Button" )
					{
						ButtonWidget* buttonWidget = new ButtonWidget();

						buttonWidget->m_currentState = buttonWidget->GetStateForStateName( thisNode.getAttribute( "CurrentState" ) );
						
						int numCommonProperties = thisNode.nAttribute();
						for ( int commonPropertyIndex = 2; commonPropertyIndex < numCommonProperties; ++commonPropertyIndex )
						{
							XMLAttribute currentCommonProperty = thisNode.getAttribute( commonPropertyIndex );
							buttonWidget->SetWidgetPropertiesFromXML( buttonWidget->m_commonProperties, currentCommonProperty );
						}

						LoadStateSpecificProperties( thisNode, "ActiveState", buttonWidget );
						LoadStateSpecificProperties( thisNode, "HighlightedState", buttonWidget );
						LoadStateSpecificProperties( thisNode, "PressedState", buttonWidget );

						m_widgets.push_back( buttonWidget );
					}
					else
					{
						DebuggerPrintf( "%s", "Widget type not recognized." );
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void UISystem::LoadStateSpecificProperties( const XMLNode& widgetNode, const char* stateName, WidgetBase* currentWidget ) const
{
	XMLNode stateNode = widgetNode.getChildNode( stateName );
	WidgetState currentState = currentWidget->GetStateForStateName( stateName );

	int numberOfProperties = stateNode.nAttribute();
	for ( int propertyIndex = 0; propertyIndex < numberOfProperties; ++propertyIndex )
	{
		XMLAttribute currentProperty = stateNode.getAttribute( propertyIndex );
		currentWidget->SetWidgetPropertiesFromXML( currentWidget->m_stateSpecificProperties[ currentState ], currentProperty );
	}
}


//-----------------------------------------------------------------------------------------------
void UISystem::RegisterUIEvents()
{
	// #TODO: Get event names from XML
	EventSystem::Instance()->RegisterMethod( "PlayButtonSound", this, &UISystem::PlayButtonSound );
	EventSystem::Instance()->RegisterMethod( "MakeExitButton", this, &UISystem::MakeExitButton );
	EventSystem::Instance()->RegisterMethod( "ExitApplication", this, &UISystem::ExitApplication );
}


//-----------------------------------------------------------------------------------------------
void UISystem::PlayButtonSound( NamedProperties& namedProperties )
{
	namedProperties;
	g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( "Data/Sounds/test.wav" ), 1.0f );
}


//-----------------------------------------------------------------------------------------------
void UISystem::MakeExitButton( NamedProperties& namedProperties )
{
	namedProperties;

	ButtonWidget* exitButton = new ButtonWidget();

	exitButton->m_currentState = WIDGET_STATE_ACTIVE;

	// Active state properties
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_ACTIVE ].Set( "WidgetDimensions", Vector2( 100.0f, 50.0f ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_ACTIVE ].Set( "CenterCoords", Vector2( 800.0f, 450.0f ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_ACTIVE ].Set( "BorderWidth", 5 );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_ACTIVE ].Set( "TextString", std::string( "Yes" ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_ACTIVE ].Set( "EventName", std::string( "ExitApplication" ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_ACTIVE ].Set( "TextCellHeight", 20.0f );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_ACTIVE ].Set( "BackgroundColor", Rgba( 175, 0, 0, 255 ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_ACTIVE ].Set( "BorderColor", Rgba( 0, 0, 0, 175 ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_ACTIVE ].Set( "TextColor", Rgba( 255, 255, 255, 255 ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_ACTIVE ].Set( "TexturePath", std::string( "" ) );

	// Highlighted state properties
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_HIGHLIGHTED ].Set( "WidgetDimensions", Vector2( 100.0f, 50.0f ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_HIGHLIGHTED ].Set( "CenterCoords", Vector2( 800.0f, 450.0f ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_HIGHLIGHTED ].Set( "BorderWidth", 5 );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_HIGHLIGHTED ].Set( "TextString", std::string( "Yes" ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_HIGHLIGHTED ].Set( "EventName", std::string( "ExitApplication" ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_HIGHLIGHTED ].Set( "TextCellHeight", 20.0f );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_HIGHLIGHTED ].Set( "BackgroundColor", Rgba( 255, 0, 0, 255 ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_HIGHLIGHTED ].Set( "BorderColor", Rgba( 0, 0, 0, 175 ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_HIGHLIGHTED ].Set( "TextColor", Rgba( 255, 255, 255, 255 ) );
	exitButton->m_stateSpecificProperties[ WIDGET_STATE_HIGHLIGHTED ].Set( "TexturePath", std::string( "" ) );

	m_widgets.push_back( exitButton );
}


//-----------------------------------------------------------------------------------------------
void UISystem::ExitApplication( NamedProperties& namedProperties )
{
	namedProperties;
	g_theDeveloperConsole->m_exitCommandIssued = true;
}


//-----------------------------------------------------------------------------------------------
AABB2 UISystem::CalculateWidgetBounds( int widgetWidth, int widgetHeight, 
	Vector2 widgetCenterPosition )
{
	int halfWidgetWidth = widgetWidth / 2;
	int halfWidgetHeight = widgetHeight / 2;

	Vector2 widgetMins( widgetCenterPosition.x - halfWidgetWidth, widgetCenterPosition.y - halfWidgetHeight );
	Vector2 widgetMaxs( widgetCenterPosition.x + halfWidgetWidth, widgetCenterPosition.y + halfWidgetHeight );

	return AABB2( widgetMins, widgetMaxs );
}
