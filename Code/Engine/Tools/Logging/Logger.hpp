#pragma once

#include "Engine/Tools/Logging/ThreadSafeQueue.hpp"


//-----------------------------------------------------------------------------------------------
class FileBinaryWriter;
struct Callstack;


//-----------------------------------------------------------------------------------------------
enum LogLevel
{
	LOG_NONE,
	LOG_SEVERE,
	LOG_RECOVERABLE,
	LOG_DEFAULT,
	LOG_ALL
};


//-----------------------------------------------------------------------------------------------
struct LogMessage
{
	char time[ 80 ];
	char contents[ 2048 ];
	const char* tag;
	int logLevel = 3;
	Callstack* cs = nullptr;
};


//-----------------------------------------------------------------------------------------------
extern ThreadSafeQueue< LogMessage* > g_messageQueue;
extern bool g_loggerIsRunning;
extern FileBinaryWriter g_writer;
extern bool g_flushLogs;
extern int g_loggingLevel;


//-----------------------------------------------------------------------------------------------
void LoggingThread( ThreadSafeQueue< LogMessage* > &messageQueue );
void HandleMessage( LogMessage* msg );
void HandleRemainingMessages( ThreadSafeQueue< LogMessage* > &messageQueue );
void LoggerPrintf( const char* messageFormat, ... );
void LoggerPrintfWithCallstack( Callstack* cs, const char* messageFormat, ... );
void LoggerPrintfWithTag( const char* tag, const char* messageFormat, ... );
void LoggerPrintfWithLevel( int logLevel, const char* messageFormat, ... );
void LoggerFlush();