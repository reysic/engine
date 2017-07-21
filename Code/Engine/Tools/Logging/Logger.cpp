#include <stdlib.h>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <stdarg.h>
#include <iostream>
#include <fstream>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Config/BuildConfig.hpp" // Adjust logging level in this file
#include "Engine/Tools/Logging/Logger.hpp"
#include "Engine/Tools/Memory/MemoryAnalytics.hpp"


//-----------------------------------------------------------------------------------------------
ThreadSafeQueue< LogMessage* > g_messageQueue;
bool g_loggerIsRunning = false;
FileBinaryWriter g_writer;
bool g_flushLogs = false;
int g_loggingLevel = 3;


//-----------------------------------------------------------------------------------------------
void LoggingThread( ThreadSafeQueue< LogMessage* > &messageQueue )
{
	UNUSED( messageQueue );
#ifdef PROGRAM_LOGGING
#if PROGRAM_LOGGING == 0 
	g_loggingLevel = 0;
#endif
#if PROGRAM_LOGGING == 1 
	g_loggingLevel = 1;
#endif
#if PROGRAM_LOGGING == 2 
	g_loggingLevel = 2;
#endif
#if PROGRAM_LOGGING == 3 
	g_loggingLevel = 3;
#endif
#if PROGRAM_LOGGING == 4 
	g_loggingLevel = 4;
#endif
#endif

#ifdef PROGRAM_LOGGING
	char timeChar[ 80 ];


	time_t     now = time( 0 );
	struct tm  tstruct;
	char       buf[ 80 ];
	UNUSED( buf );
	localtime_s( &tstruct, &now );
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime( timeChar, sizeof( buf ), "%Y%m%d_%H%M%S", &tstruct );

	char filename[ 100 ];
	sprintf_s( filename, "sd5a2_%s.log", timeChar );
	g_writer.Open( filename );

	while ( g_loggerIsRunning )
	{
		HandleRemainingMessages( messageQueue );
		std::this_thread::yield();
	}

	HandleRemainingMessages( messageQueue );

	g_writer.Close();

	std::ifstream src( filename, std::ios::binary );
	std::ofstream dst( "sd5a2.log", std::ios::binary );
	dst << src.rdbuf();
#endif
}


//-----------------------------------------------------------------------------------------------
void HandleMessage( LogMessage* msg )
{
	UNUSED( msg );
#ifdef PROGRAM_LOGGING
	if ( msg->logLevel > g_loggingLevel )
	{
		// Log does not meet logging threshold
		return;
	}

	// Print to Output
	DebuggerPrintf( "\n%i %s %s %s\n", msg->logLevel, msg->time, msg->tag, msg->contents );
	if ( msg->cs != nullptr )
	{
		// Message does have an associated callstack
		CallstackLine* line = CallstackGetLines( msg->cs );

		for ( size_t i = 0; i < msg->cs->framecount; ++i )
		{
			DebuggerPrintf( "\t%s(%u): %s\n", line[ i ].filename, line[ i ].line, line[ i ].functionName );
		}
		DebuggerPrintf( "\n" );
	}
	DebuggerPrintf( "\n" );

	// Print to File
	g_writer.WriteStringText( msg->time );
	g_writer.WriteStringText( " " );
	g_writer.WriteStringText( std::to_string( msg->logLevel ) );
	g_writer.WriteStringText( " " );
	g_writer.WriteStringText( msg->tag );
	g_writer.WriteStringText( " " );
	g_writer.WriteStringText( msg->contents );
	if ( msg->cs != nullptr )
	{
		CallstackLine* line = CallstackGetLines( msg->cs );

		for ( size_t i = 0; i < msg->cs->framecount; ++i )
		{
			g_writer.WriteStringText( "\t" );
			g_writer.WriteStringText( line[ i ].filename );
			g_writer.WriteStringText( std::to_string( line[ i ].line ) );
			g_writer.WriteStringText( line[ i ].functionName );
			g_writer.WriteStringText( "\n" );
		}
	}
#endif
}


//-----------------------------------------------------------------------------------------------
void HandleRemainingMessages( ThreadSafeQueue< LogMessage* > &messageQueue )
{
	UNUSED( messageQueue );
#ifdef PROGRAM_LOGGING
	LogMessage *msg;
	while ( messageQueue.QueueSize() > 0 )
	{
		messageQueue.Dequeue( &msg );
		HandleMessage( msg );
		delete msg;
	}

	if ( g_flushLogs )
	{
		g_writer.Flush();
		g_flushLogs = false;
	}
#endif
}


//-----------------------------------------------------------------------------------------------
void LoggerPrintf( const char* messageFormat, ... )
{
	UNUSED( messageFormat );
#ifdef PROGRAM_LOGGING
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[ MESSAGE_MAX_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, messageFormat );
	vsnprintf_s( messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList );
	va_end( variableArgumentList );
	messageLiteral[ MESSAGE_MAX_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	LogMessage* thisMessage = new LogMessage();

	// Log message time
	time_t     now = time( 0 );
	struct tm  tstruct;
	char       buf[ 80 ];
	UNUSED( buf );
	localtime_s( &tstruct, &now );
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime( thisMessage->time, sizeof( buf ), "%Y-%m-%d.%X", &tstruct );

	// Log message contents
	strncpy_s( thisMessage->contents, messageLiteral, 2048 );

	// Log message tag
	thisMessage->tag = "default";

	g_messageQueue.Enqueue( thisMessage );
#endif
}


//-----------------------------------------------------------------------------------------------
void LoggerPrintfWithCallstack( Callstack* cs, const char* messageFormat, ... )
{
	UNUSED( cs );
	UNUSED( messageFormat );
#ifdef PROGRAM_LOGGING
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[ MESSAGE_MAX_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, messageFormat );
	vsnprintf_s( messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList );
	va_end( variableArgumentList );
	messageLiteral[ MESSAGE_MAX_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	LogMessage* thisMessage = new LogMessage();

	// Log message time
	time_t     now = time( 0 );
	struct tm  tstruct;
	char       buf[ 80 ];
	UNUSED( buf );
	localtime_s( &tstruct, &now );
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime( thisMessage->time, sizeof( buf ), "%Y-%m-%d.%X", &tstruct );

	// Log message contents
	strncpy_s( thisMessage->contents, messageLiteral, 2048 );

	// Log message tag
	thisMessage->tag = "default";

	// Log message callstack
	thisMessage->cs = cs;

	g_messageQueue.Enqueue( thisMessage );
#endif
}


//-----------------------------------------------------------------------------------------------
void LoggerPrintfWithTag( const char* tag, const char* messageFormat, ... )
{
	UNUSED( tag );
	UNUSED( messageFormat );
#ifdef PROGRAM_LOGGING
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[ MESSAGE_MAX_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, messageFormat );
	vsnprintf_s( messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList );
	va_end( variableArgumentList );
	messageLiteral[ MESSAGE_MAX_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	LogMessage* thisMessage = new LogMessage();

	// Log message time
	time_t     now = time( 0 );
	struct tm  tstruct;
	char       buf[ 80 ];
	UNUSED( buf );
	localtime_s( &tstruct, &now );
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime( thisMessage->time, sizeof( buf ), "%Y-%m-%d.%X", &tstruct );

	// Log message contents
	strncpy_s( thisMessage->contents, messageLiteral, 2048 );

	// Log message tag
	thisMessage->tag = tag;

	g_messageQueue.Enqueue( thisMessage );
#endif
}


//-----------------------------------------------------------------------------------------------
void LoggerPrintfWithLevel( int logLevel, const char* messageFormat, ... )
{
	UNUSED( logLevel );
	UNUSED( messageFormat );
#ifdef PROGRAM_LOGGING
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[ MESSAGE_MAX_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, messageFormat );
	vsnprintf_s( messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList );
	va_end( variableArgumentList );
	messageLiteral[ MESSAGE_MAX_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	LogMessage* thisMessage = new LogMessage();

	// Log message time
	time_t     now = time( 0 );
	struct tm  tstruct;
	char       buf[ 80 ];
	UNUSED( buf );
	localtime_s( &tstruct, &now );
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime( thisMessage->time, sizeof( buf ), "%Y-%m-%d.%X", &tstruct );

	// Log message contents
	strncpy_s( thisMessage->contents, messageLiteral, 2048 );

	// Log message tag
	thisMessage->tag = "default";

	// Log message level
	thisMessage->logLevel = logLevel;

	g_messageQueue.Enqueue( thisMessage );
#endif
}

//-----------------------------------------------------------------------------------------------
void LoggerFlush()
{
#ifdef PROGRAM_LOGGING
	g_flushLogs = true;

	while ( g_flushLogs )
	{
		std::this_thread::yield();
	}
#endif
}