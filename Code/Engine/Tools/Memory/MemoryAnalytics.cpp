#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Config/BuildConfig.hpp" // Enable/disable memory tracking in this file
#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Tools/Memory/MemoryAnalytics.hpp"


//-----------------------------------------------------------------------------------------------
// SymInitialize()
typedef BOOL( __stdcall *sym_initialize_t )( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );
typedef BOOL( __stdcall *sym_cleanup_t )( IN HANDLE hProcess );
typedef BOOL( __stdcall *sym_from_addr_t )( IN HANDLE hProcess, IN DWORD64 Address, OUT PDWORD64 Displacement, OUT PSYMBOL_INFO Symbol );
typedef BOOL( __stdcall *sym_get_line_t )( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Symbol );


//-----------------------------------------------------------------------------------------------
AllocationToCallstackMap g_callstackRegistry;
HMODULE gDebugHelp;
HANDLE gProcess;
SYMBOL_INFO  *gSymbol;

// only called from single thread - so can use a shared buffer
char gFileName[ MAX_FILENAME_LENGTH ];
CallstackLine gCallstackBuffer[ MAX_DEPTH ];

static sym_initialize_t LSymInitialize;
static sym_cleanup_t LSymCleanup;
static sym_from_addr_t LSymFromAddr;
static sym_get_line_t LSymGetLineFromAddr64;


//-----------------------------------------------------------------------------------------------
unsigned int g_numberOfAllocations = 0;
unsigned int g_totalBytesAllocated = 0;
unsigned int g_highwaterTotalBytesAllocated = 0;
unsigned int g_numberOfAllocationsStartup = 0;
bool g_displayMemoryInformation = true;


//-----------------------------------------------------------------------------------------------
Callstack* CallstackFetch( unsigned int numSkipFrames )
{
	void *stack[ MAX_DEPTH ];
	unsigned int frames = CaptureStackBackTrace( 1 + numSkipFrames, MAX_DEPTH, stack, NULL );

	Callstack* cs = ( Callstack* ) malloc( sizeof( Callstack ) );
	cs->framecount = frames;
	cs->frames = ( void** ) malloc( frames * sizeof( void* ) );
	memcpy( cs->frames, stack, frames * sizeof( void* ) );

	return cs;
}


//-----------------------------------------------------------------------------------------------
void FreeCallstack( Callstack* cs )
{
	free( cs->frames );
	free( cs );
}


//-----------------------------------------------------------------------------------------------
CallstackLine* CallstackGetLines( Callstack* cs )
{
	IMAGEHLP_LINE64 LineInfo;
	DWORD LineDisplacement = 0; // Displacement from the beginning of the line 
	LineInfo.SizeOfStruct = sizeof( IMAGEHLP_LINE64 );

	unsigned int count = cs->framecount;
	for ( unsigned int i = 0; i < count; ++i )
	{
		CallstackLine* line = &( gCallstackBuffer[ i ] );
		DWORD64 ptr = ( DWORD64 ) ( cs->frames[ i ] );
		LSymFromAddr( gProcess, ptr, 0, gSymbol );

		strcpy_s( line->functionName, 1024, gSymbol->Name );

		BOOL bRet = LSymGetLineFromAddr64(
			GetCurrentProcess(), // Process handle of the current process 
			ptr, // Address 
			&LineDisplacement, // Displacement will be stored here by the function 
			&LineInfo );         // File name / line information will be stored here 

		if ( bRet )
		{
			line->line = LineInfo.LineNumber;

			// 			char const *filename = StringFindLast( LineInfo.FileName, "\\src" );
			// 			if ( filename == NULL ) 
			// 			{
			// 				filename = LineInfo.FileName;
			// 			}
			// 			else 
			// 			{
			// 				filename += 5; // skip to the important bit - so that it can be double clicked in Output
			// 			}


			strcpy_s( line->filename, 1024, LineInfo.FileName );
			line->offset = LineDisplacement;
		}
		else
		{
			line->line = 0;
			line->offset = 0;
			strcpy_s( line->filename, 1024, "N/A" );
		}
	}

	return gCallstackBuffer;
}


//-----------------------------------------------------------------------------------------------
void MemoryAnalyticsStartup()
{
	gDebugHelp = LoadLibraryA( "dbghelp.dll" );
	ASSERT_OR_DIE( gDebugHelp != nullptr, "gDebugHelp == nullptr" );
	LSymInitialize = ( sym_initialize_t ) GetProcAddress( gDebugHelp, "SymInitialize" );
	LSymCleanup = ( sym_cleanup_t ) GetProcAddress( gDebugHelp, "SymCleanup" );
	LSymFromAddr = ( sym_from_addr_t ) GetProcAddress( gDebugHelp, "SymFromAddr" );
	LSymGetLineFromAddr64 = ( sym_get_line_t ) GetProcAddress( gDebugHelp, "SymGetLineFromAddr64" );

	gProcess = GetCurrentProcess();
	LSymInitialize( gProcess, NULL, TRUE );

	gSymbol = ( SYMBOL_INFO * ) malloc( sizeof( SYMBOL_INFO ) + ( MAX_FILENAME_LENGTH * sizeof( char ) ) );
	gSymbol->MaxNameLen = MAX_FILENAME_LENGTH;
	gSymbol->SizeOfStruct = sizeof( SYMBOL_INFO );

	g_numberOfAllocationsStartup = g_numberOfAllocations;

	DebuggerPrintf( "g_numberOfAllocations Startup: %u\n", g_numberOfAllocationsStartup );
	DebuggerPrintf( "g_totalBytesAllocated Startup: %u\n", g_totalBytesAllocated );
	LoggerPrintf( "g_numberOfAllocations Startup: %u\n", g_numberOfAllocationsStartup );
	LoggerPrintf( "g_totalBytesAllocated Startup: %u\n", g_totalBytesAllocated );
}


//-----------------------------------------------------------------------------------------------
void MemoryAnalyticsShutdown()
{
	DebuggerPrintf( "g_numberOfAllocations Shutdown: %u\n", g_numberOfAllocations );
	DebuggerPrintf( "g_totalBytesAllocated Shutdown: %u\n", g_totalBytesAllocated );
	LoggerPrintf( "g_numberOfAllocations Shutdown: %u\n", g_numberOfAllocations );
	LoggerPrintf( "g_totalBytesAllocated Shutdown: %u\n", g_totalBytesAllocated );

#ifdef MEMORY_TRACKING
#if MEMORY_TRACKING == 0 // Basic mode
	//ASSERT_RECOVERABLE( g_numberOfAllocations == g_numberOfAllocationsStartup, "Differing number of allocations on startup and shutdown!" );
#endif
#endif

	for ( auto const &p : g_callstackRegistry )
	{
		DebuggerPrintf( "Alloc %p\n", p.first );
		CallstackLine* line = CallstackGetLines( p.second );

		for ( size_t i = 0; i < p.second->framecount; ++i )
		{
			DebuggerPrintf( line[ i ].filename );
			DebuggerPrintf( "(" );
			DebuggerPrintf( "%u", line[ i ].line );
			DebuggerPrintf( "): " );
			DebuggerPrintf( line[ i ].functionName );
			DebuggerPrintf( "\n" );
		}
		DebuggerPrintf( "\n" );
	}

	LSymCleanup( gProcess );

	FreeLibrary( gDebugHelp );
	gDebugHelp = NULL;

	free( gSymbol );
}


//-----------------------------------------------------------------------------------------------
void MemoryAnalyticsUpdate( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if ( g_totalBytesAllocated > g_highwaterTotalBytesAllocated )
	{
		g_highwaterTotalBytesAllocated = g_totalBytesAllocated;
	}
}


//-----------------------------------------------------------------------------------------------
void MemoryAnalyticsRender()
{
	if ( !g_displayMemoryInformation )
	{
		return;
	}

	if ( !g_debugMode )
	{
		return;
	}

	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	g_theRenderer->DrawText2D( Vector2( 0.0f, 865.0f ), "#Alloc:"
		+ Stringf( "%u", g_numberOfAllocations ), 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 200.0f, 865.0f ), "#Bytes:"
		+ Stringf( "%u", g_totalBytesAllocated ), 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 450.0f, 865.0f ), "HWBytes:"
		+ Stringf( "%u", g_highwaterTotalBytesAllocated ), 15.0f, Rgba::GREEN, fixedFont );
}


//-----------------------------------------------------------------------------------------------
void PrintMemoryFlush()
{
	for ( auto const &p : g_callstackRegistry )
	{
		DebuggerPrintf( "Alloc %p\n", p.first );
		CallstackLine* line = CallstackGetLines( p.second );

		for ( size_t i = 0; i < p.second->framecount; ++i )
		{
			DebuggerPrintf( line[ i ].filename );
			DebuggerPrintf( "(" );
			DebuggerPrintf( "%u", line[ i ].line );
			DebuggerPrintf( "): " );
			DebuggerPrintf( line[ i ].functionName );
			DebuggerPrintf( "\n" );
		}
		DebuggerPrintf( "\n" );
	}
}


//-----------------------------------------------------------------------------------------------
void* operator new( size_t numBytes )
{
#ifdef MEMORY_TRACKING
#if MEMORY_TRACKING == 0 // Basic mode
	size_t* ptr = ( size_t* ) malloc( numBytes + sizeof( size_t ) );
//DebuggerPrintf( "Alloc %p of %u bytes.\n", ptr, numBytes );
++g_numberOfAllocations;
g_totalBytesAllocated += numBytes;
*ptr = numBytes;
ptr++;
return ptr;
#elif MEMORY_TRACKING == 1 // Verbose mode
	size_t* ptr = ( size_t* ) malloc( numBytes + sizeof( size_t ) );
//DebuggerPrintf( "Alloc %p of %u bytes.\n", ptr, numBytes );
++g_numberOfAllocations;
g_totalBytesAllocated += numBytes;
*ptr = numBytes;
ptr++;
g_callstackRegistry.insert( AllocationToCallstackPair( ( void* ) ptr, CallstackFetch( 2 ) ) );
return ptr;
#endif
#else
	// No memory tracking
	void* ptr = malloc( numBytes );
	return ptr;
#endif
}


//-----------------------------------------------------------------------------------------------
void* operator new[]( size_t numBytes )
{
#ifdef MEMORY_TRACKING
#if MEMORY_TRACKING == 0 // Basic mode
	size_t* ptr = ( size_t* ) malloc( numBytes + sizeof( size_t ) );
	//DebuggerPrintf( "Alloc %p of %u bytes.\n", ptr, numBytes );
	++g_numberOfAllocations;
	g_totalBytesAllocated += numBytes;
	*ptr = numBytes;
	ptr++;
	return ptr;
#elif MEMORY_TRACKING == 1 // Verbose mode
	size_t* ptr = ( size_t* ) malloc( numBytes + sizeof( size_t ) );
	//DebuggerPrintf( "Alloc %p of %u bytes.\n", ptr, numBytes );
	++g_numberOfAllocations;
	g_totalBytesAllocated += numBytes;
	*ptr = numBytes;
	ptr++;
	return ptr;
#endif
#else
	// No memory tracking
	void* ptr = malloc( numBytes );
	return ptr;
#endif
}


//-----------------------------------------------------------------------------------------------
void operator delete( void* ptr )
{
#ifdef MEMORY_TRACKING
#if MEMORY_TRACKING == 0 // Basic mode
	size_t* ptr_size = ( size_t* ) ptr;
	--ptr_size;
	size_t numBytes = *ptr_size;
	--g_numberOfAllocations;
	g_totalBytesAllocated -= numBytes;
	free( ptr_size );
#elif MEMORY_TRACKING == 1 // Verbose mode
	size_t* ptr_size = ( size_t* ) ptr;
	--ptr_size;
	size_t numBytes = *ptr_size;
	--g_numberOfAllocations;
	g_totalBytesAllocated -= numBytes;
	free( ptr_size );
	AllocationToCallstackMapIter ptrIter = g_callstackRegistry.find( ptr );
	if ( ptrIter != g_callstackRegistry.end() )
	{
		FreeCallstack( ptrIter->second );
		g_callstackRegistry.erase( ptrIter );
	}
#endif
#else
	// No memory tracking
	free( ptr );
#endif
}


//-----------------------------------------------------------------------------------------------
void operator delete[]( void* ptr )
{
#ifdef MEMORY_TRACKING
#if MEMORY_TRACKING == 0 // Basic mode
	size_t* ptr_size = ( size_t* ) ptr;
	--ptr_size;
	size_t numBytes = *ptr_size;
	--g_numberOfAllocations;
	g_totalBytesAllocated -= numBytes;
	free( ptr_size );
#elif MEMORY_TRACKING == 1 // Verbose mode
	size_t* ptr_size = ( size_t* ) ptr;
	--ptr_size;
	size_t numBytes = *ptr_size;
	--g_numberOfAllocations;
	g_totalBytesAllocated -= numBytes;
	free( ptr_size );
#endif
#else
	// No memory tracking
	free( ptr );
#endif
}


//-----------------------------------------------------------------------------------------------
#ifdef MEMORY_TRACKING
#if MEMORY_TRACKING == 0 // Basic mode
CONSOLE_COMMAND( memory_debug )
{
	UNUSED( args );
	g_displayMemoryInformation = !g_displayMemoryInformation;
}
#elif MEMORY_TRACKING == 1 // Verbose mode
CONSOLE_COMMAND( memory_debug )
{
	UNUSED( args );
	g_displayMemoryInformation = !g_displayMemoryInformation;
}
CONSOLE_COMMAND( memory_flush )
{
	UNUSED( args );
	/*			PrintMemoryFlush();*/
	g_theDeveloperConsole->ConsolePrint( "Check Output window for current allocations." );
}
#endif
#endif