#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdlib.h>
#pragma warning (disable: 4091)
#include <DbgHelp.h>
#include <map>

#include "Engine/Tools/Memory/UntrackedAllocator.hpp"

#define MAX_SYMBOL_NAME_LENGTH 128
#define MAX_FILENAME_LENGTH 1024
#define MAX_DEPTH 128


//-----------------------------------------------------------------------------------------------
struct Callstack
{
	void **frames; // array of void *
	size_t framecount;
};


//-----------------------------------------------------------------------------------------------
struct CallstackLine
{
	char filename[ 1024 ];
	char functionName[ 1024 ];
	unsigned int line;
	unsigned int offset;
};


//-----------------------------------------------------------------------------------------------
typedef std::pair< void*, Callstack* > AllocationToCallstackPair;
typedef std::map< void*, Callstack*, std::less< void* >, TUntrackedAllocator< AllocationToCallstackPair > > AllocationToCallstackMap;
typedef AllocationToCallstackMap::iterator AllocationToCallstackMapIter;


//-----------------------------------------------------------------------------------------------
extern unsigned int g_numberOfAllocations;
extern unsigned int g_totalBytesAllocated;
extern unsigned int g_highwaterTotalBytesAllocated;
extern unsigned int g_numberOfAllocationsStartup;
extern bool g_displayMemoryInformation;
extern AllocationToCallstackMap g_callstackRegistry;
extern HMODULE gDebugHelp;
extern HANDLE gProcess;
extern SYMBOL_INFO  *gSymbol;
extern char gFileName[ MAX_FILENAME_LENGTH ];
extern CallstackLine gCallstackBuffer[ MAX_DEPTH ];


//-----------------------------------------------------------------------------------------------
Callstack* CallstackFetch( unsigned int numSkipFrames );
void FreeCallstack( Callstack* cs ); 
CallstackLine* CallstackGetLines( Callstack* cs );
void MemoryAnalyticsStartup();
void MemoryAnalyticsShutdown();
void MemoryAnalyticsUpdate( float deltaSeconds );
void MemoryAnalyticsRender();
void PrintMemoryFlush();
void* operator new( size_t numBytes );
void* operator new[]( size_t numBytes );
void operator delete( void* ptr );
void operator delete[]( void* ptr );