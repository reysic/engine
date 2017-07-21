#pragma once

#include <vector>
#include <string>
#include <map>

#include "Engine/Core/Rgba.hpp"


//-----------------------------------------------------------------------------------------------
class DeveloperConsole;


//-----------------------------------------------------------------------------------------------
extern DeveloperConsole* g_theDeveloperConsole;


//-----------------------------------------------------------------------------------------------
struct ConsoleString
{
	ConsoleString() {};
	ConsoleString( const Rgba& color, const std::string& text ) :
		m_tint( color ),
		m_text( text ) {};
	Rgba m_tint;
	std::string m_text;
};

class Command
{
public:
	Command( std::string full_command_str ); // split name and args into two buffers

	std::string m_commandName;
	std::vector< std::string > m_argList;
	std::vector< std::string > m_commandContents;
};

typedef void( *console_command_cb )( Command& );

class RegisterCommandHelper
{
public:
	RegisterCommandHelper( std::string name, console_command_cb cb );
};

class CommandRegistry
{
	public:
		CommandRegistry() {};

	std::map< std::string, console_command_cb > m_registry;
};


//-----------------------------------------------------------------------------------------------
class DeveloperConsole
{
public:
	DeveloperConsole( int screenHeight, int screenWidth );
	~DeveloperConsole();
	void Update( float deltaSeconds );
	void Render();
	void ConsolePrint( const std::string& text, const Rgba& color = Rgba::WHITE );
	void ReceiveInput( unsigned char asKey );
	static void RegisterCommand( std::string name, console_command_cb cb ); // Put name and console_command_cb into a map
	void ExecuteCommand( std::string name );

public:
	bool m_isOpen;
	bool m_exitCommandIssued;
	bool m_renderBlinkingCursor;
	bool m_hasBeenOpenedFirstTime;
	int m_screenHeight;
	int m_screenWidth;
	float m_timeSinceLastCursorBlink;
	int m_cursorIndex;
	int m_linesToScrollUp;
	float currentScreenY;
	float maxScreenY;
	float incrementMe;

	std::vector<ConsoleString> m_log;
	std::string m_curCommand;
};


#define CONSOLE_COMMAND(name) void ConsoleCommand_ ## name ## ( Command & args ); \
   static RegisterCommandHelper RegistrationHelper_## name ##( #name, ConsoleCommand_ ## name ## ); \
   void ConsoleCommand_ ## name ##(Command &args)