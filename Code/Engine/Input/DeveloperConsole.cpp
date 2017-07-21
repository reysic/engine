#include <sstream>

#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Tools/Logging/Logger.hpp"


//-----------------------------------------------------------------------------------------------
DeveloperConsole* g_theDeveloperConsole = nullptr;


//-----------------------------------------------------------------------------------------------
const char* FONT_NAME = "Data/Fonts/Arial.fnt";
const int NUM_CONSOLE_LINES = 20;
const float TEXT_SCALE = 0.4f;
const float TIME_BETWEEN_CURSOR_BLINKS = 0.5f;
const float CURSOR_PADDING = 0.2f;
static CommandRegistry* g_commandRegistry;


//-----------------------------------------------------------------------------------------------
DeveloperConsole::DeveloperConsole( int screenWidth, int screenHeight )
{
	m_screenHeight = screenHeight;
	m_screenWidth = screenWidth;
	m_isOpen = false;
	m_exitCommandIssued = false;
	m_renderBlinkingCursor = true;
	m_hasBeenOpenedFirstTime = false;
	m_curCommand = "";
	m_timeSinceLastCursorBlink = 0.0f;
	m_cursorIndex = 0;
	m_linesToScrollUp = 0;
	currentScreenY = 0;
	maxScreenY = ( float ) screenHeight / 2.0f;

	incrementMe = 0.0f;

	// LoggerPrintfWithLevel( 2, "DeveloperConsole initialized.\n" );
}


//-----------------------------------------------------------------------------------------------
DeveloperConsole::~DeveloperConsole()
{
	if ( g_commandRegistry != nullptr )
	{
		delete g_commandRegistry;
		g_commandRegistry = nullptr;
	}

	// LoggerPrintf( "DeveloperConsole deinitialized.\n" );
}


//-----------------------------------------------------------------------------------------------
void DeveloperConsole::Update( float deltaSeconds )
{
	m_timeSinceLastCursorBlink += deltaSeconds;

	if ( m_timeSinceLastCursorBlink > TIME_BETWEEN_CURSOR_BLINKS )
	{
		m_renderBlinkingCursor = !m_renderBlinkingCursor;

		m_timeSinceLastCursorBlink = 0.0f;
	}

	if ( g_theInputSystem->IsKeyPressed( VK_LEFT ) )
	{
		if ( m_cursorIndex > 0 )
		{
			m_cursorIndex--;
		}
	}
	
	if ( g_theInputSystem->IsKeyPressed( VK_RIGHT ) )
	{
		if ( ( unsigned int ) m_cursorIndex < m_curCommand.length() )
		{
			m_cursorIndex++;
		}
	}

	if ( g_theInputSystem->IsKeyPressed( VK_HOME ) )
	{
		m_cursorIndex = 0;
	}

	if ( g_theInputSystem->IsKeyPressed( VK_END ) )
	{
		m_cursorIndex = m_curCommand.length();
	}

	if ( g_theInputSystem->IsKeyPressed( VK_DELETE ) )
	{
		if ( ( unsigned int ) m_cursorIndex < m_curCommand.length() )
		{
			m_curCommand.erase( m_cursorIndex, 1 );
		}
	}

	if ( g_theInputSystem->IsKeyPressed( VK_PRIOR ) )
	{
		if ( m_linesToScrollUp < ( int ) m_log.size() - 1 )
		{
			m_linesToScrollUp++;
		}
	}

	if ( g_theInputSystem->IsKeyPressed( VK_NEXT ) )
	{
		if ( m_linesToScrollUp > 0 )
		{
			m_linesToScrollUp--;
		}
	}

	if ( g_theDeveloperConsole->m_isOpen )
	{
		incrementMe += 50.0f;
		float rangeMap = RangeMapZeroToOne( incrementMe, 0.0f, maxScreenY );
		float smoothStep = SmoothStep( rangeMap );
		currentScreenY = RangeMap( smoothStep, 0.0f, 1.0f, 0.0f, maxScreenY );
	}
	else
	{
		incrementMe = 0.0f;
		currentScreenY = 0.0f;
	}
}


//-----------------------------------------------------------------------------------------------
void DeveloperConsole::Render()
{
	if ( m_isOpen )
	{
		if ( !m_hasBeenOpenedFirstTime )
		{
			m_hasBeenOpenedFirstTime = true;
			ExecuteCommand( "info" );
		}

		static BitmapFont* bitmapFontFixed = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
		static BitmapFontProportional* bitmapFont = BitmapFontProportional::CreateOrGetFont( FONT_NAME );

		//-----------------------------------------------------------------------------------------------
		// Let's make this overlay nice and scrolly
		AABB2 overlay = AABB2( Vector2( 0.f, 0.f ), Vector2( ( float ) m_screenWidth, currentScreenY ) );

		//-----------------------------------------------------------------------------------------------
		AABB2 prompt = overlay;
		prompt.maxs.y = prompt.maxs.y / ( float ) NUM_CONSOLE_LINES;

		Rgba overlayColor = Rgba( 0, 0, 0, 127 );
		Rgba promptColor = Rgba( 190, 190, 190, 127 );
		g_theRenderer->DepthTestingEnabled( false );
		g_theRenderer->DrawQuad( overlay.mins, overlay.maxs, overlayColor );
		g_theRenderer->DrawQuad( prompt.mins, prompt.maxs, promptColor );

		float lineYPos = 0.0f;
		//float lineYPos = prompt.maxs.y;
		g_theRenderer->DrawText2D( Vector2( 0.0f, lineYPos ), m_curCommand, 15.0f, Rgba::WHITE, bitmapFontFixed );
		//g_theRenderer->DrawTextProportional2D( Vector3( 0.f, lineYPos, 0.f ), m_curCommand, TEXT_SCALE, Rgba::WHITE, bitmapFont );

		if ( m_renderBlinkingCursor )
		{
			std::string substring = m_curCommand.substr( 0, m_cursorIndex );
			float cursorXPosition = m_cursorIndex * 15.0f;
			//float cursorXPosition = bitmapFont->CalcTextWidth( substring, TEXT_SCALE );
			g_theRenderer->DrawLine3D( Vector3( cursorXPosition + CURSOR_PADDING, 0.0f, 0.0f ), Vector3( cursorXPosition + CURSOR_PADDING, prompt.maxs.y, 0.0f ), Rgba::WHITE, 2.0f );
		}

		int index = 2;
		std::vector<ConsoleString>::reverse_iterator logIter = m_log.rbegin();

		for ( int lineCount = 0; lineCount < m_linesToScrollUp; lineCount++ )
		{
			logIter++;
			if ( logIter == m_log.rend() )
				break;
		}
		for ( logIter; logIter != m_log.rend(); ++logIter )
		{
			if ( index > NUM_CONSOLE_LINES )
				break;
			lineYPos = prompt.maxs.y * index - prompt.maxs.y;
			//lineYPos = prompt.maxs.y * index;
			std::string text = logIter->m_text;
			Rgba color = logIter->m_tint;
			g_theRenderer->DrawText2D( Vector2( 0.0f, lineYPos ), text, 15.0f, color, bitmapFontFixed );
			//g_theRenderer->DrawTextProportional2D( Vector3( 0.f, lineYPos, 0.f ), text, TEXT_SCALE, color, bitmapFont );
			index++;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void DeveloperConsole::ConsolePrint( const std::string& text, const Rgba& color )
{
	ConsoleString cs( color, text );
	m_log.push_back( cs );
}


//-----------------------------------------------------------------------------------------------
void DeveloperConsole::ReceiveInput( unsigned char asKey )
{
	if ( asKey == VK_BACK )
	{
		if ( m_curCommand.length() > 0 )
		{
			m_curCommand = m_curCommand.erase( m_curCommand.size() - 1 );
			m_cursorIndex--;
		}
	}
	else if ( asKey == VK_RETURN )
	{
		if ( m_curCommand != "" )
		{
			ConsolePrint( m_curCommand, Rgba::GRAY );
			ExecuteCommand( m_curCommand );
			m_curCommand = "";
			m_cursorIndex = 0;
			m_linesToScrollUp = 0;
		}
		else
		{
			m_isOpen = false;
		}
	}
	else if ( asKey == VK_ESCAPE )
	{
		if ( m_curCommand != "" )
		{
			m_curCommand = "";
			m_cursorIndex = 0;
		}
		else
		{
			m_isOpen = false;
		}
	}
	else if ( asKey == '`' || asKey == '~' )
	{
		// Do nothing
	}
	else
	{
		static BitmapFontProportional* bitmapFont = BitmapFontProportional::CreateOrGetFont( FONT_NAME );
		std::map< int, Glyph* >::iterator it;
		it = bitmapFont->m_glyphRegistry.find( asKey );

		if ( it != bitmapFont->m_glyphRegistry.end() )
		{
			if ( ( unsigned int ) m_cursorIndex == m_curCommand.length() )
			{
				m_curCommand += ( unsigned char ) it->second->m_charId;
			}
			else
			{
				std::string firstSubstring = m_curCommand.substr( 0, m_cursorIndex );
				std::string secondSubstring = m_curCommand.substr( m_cursorIndex, m_curCommand.length() );
				m_curCommand = "";
				m_curCommand += firstSubstring;
				m_curCommand += ( unsigned char ) it->second->m_charId;
				m_curCommand += secondSubstring;
			}
			m_cursorIndex++;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void DeveloperConsole::RegisterCommand( std::string name, console_command_cb cb )
{
	if ( g_commandRegistry == nullptr )
	{
		g_commandRegistry = new CommandRegistry();
	}
	g_commandRegistry->m_registry.insert( std::pair< std::string, console_command_cb>( name, cb ) );
}


//-----------------------------------------------------------------------------------------------
void DeveloperConsole::ExecuteCommand( std::string full_command_str )
{
	Command command( full_command_str );

	std::map< std::string, console_command_cb >::iterator it;
	it = g_commandRegistry->m_registry.find( command.m_commandName );

	if ( it != g_commandRegistry->m_registry.end() )
	{
		// Hooray! A valid command!
		it->second( command );
	}
	else
	{
		ConsolePrint( "Invalid command!", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
Command::Command( std::string full_command_str )
{
	std::stringstream fullCommandStringStream( full_command_str );

	std::string token;
	while ( fullCommandStringStream >> token )
	{
		m_commandContents.push_back( token );
	}

	if ( m_commandContents.size() > 0 )
	{
		m_commandName = m_commandContents[ 0 ];
	}

	if ( m_commandContents.size() > 1 )
	{
		for ( std::vector< std::string >::iterator argIter = m_commandContents.begin() + 1; argIter != m_commandContents.end(); ++argIter )
		{
			m_argList.push_back( *argIter );
		}
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( help )
{
	if ( args.m_argList.empty() )
	{
		g_theDeveloperConsole->ConsolePrint( "Commands Available:", Rgba::WHITE );
		for ( std::map< std::string, console_command_cb >::iterator comIter = g_commandRegistry->m_registry.begin(); comIter != g_commandRegistry->m_registry.end(); ++comIter )
		{
			std::string strToPrint = comIter->first;
			if ( comIter->first == "fbx_load" )
 			{
 				strToPrint += " <filename> [scale]";
 			}
			else if ( comIter->first == "fbx_list" )
			{
				strToPrint += " <filename>";
			}
			else if ( comIter->first == "mesh_save" )
			{
				strToPrint += " <filename>";
			}
			else if ( comIter->first == "mesh_load" )
			{
				strToPrint += " <filename>";
			}
			g_theDeveloperConsole->ConsolePrint( strToPrint, Rgba::GREEN );
		}
	}
	else if ( args.m_argList[ 0 ] == "help" )
	{
		g_theDeveloperConsole->ConsolePrint( "help <commandname>: Prints a description of the command", Rgba::RED );
	}
	else if ( args.m_argList[ 0 ] == "exit" )
	{
		g_theDeveloperConsole->ConsolePrint( "exit: Closes the application.", Rgba::RED );
	}
	else if ( args.m_argList[ 0 ] == "draw" )
	{
		g_theDeveloperConsole->ConsolePrint( "draw <debugobjectname>: Toggles drawing of a debug object in renderer.", Rgba::RED );
	}
	else if ( args.m_argList[ 0 ] == "particle" )
	{
		g_theDeveloperConsole->ConsolePrint( "particle <emittertype>: Toggles drawing of particles using emitter type.", Rgba::RED );
	}
	else if ( args.m_argList[ 0 ] == "clear" )
	{
		g_theDeveloperConsole->ConsolePrint( "clear: Clears the contents of the console.", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( clear )
{
	args;
	g_theDeveloperConsole->m_log.clear();
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( exit )
{
	args;
	g_theDeveloperConsole->m_exitCommandIssued = true;
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( sound )
{
	args;
	g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( "Data/Sounds/test.wav" ), 1.0f );
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( debug )
{
	args;
	g_debugMode = !g_debugMode;
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( info )
{
	args;
	const char* vendorString = ( const char* ) glGetString( GL_VENDOR );
	const char* versionString = ( const char* ) glGetString( GL_VERSION );
	const char* glslVersionString = ( const char* ) glGetString( GL_SHADING_LANGUAGE_VERSION );
	g_theDeveloperConsole->ConsolePrint( Stringf( "OpenGL vendor string is %s", vendorString ), Rgba::GREEN );
	g_theDeveloperConsole->ConsolePrint( Stringf( "OpenGL version string is %s", versionString ), Rgba::GREEN );
	g_theDeveloperConsole->ConsolePrint( Stringf( "GLSL version string is %s", glslVersionString ), Rgba::GREEN );
}


//-----------------------------------------------------------------------------------------------
RegisterCommandHelper::RegisterCommandHelper( std::string name, console_command_cb cb )
{
	DeveloperConsole::RegisterCommand( name, cb );
}