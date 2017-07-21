#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Tools/Profiling/Profiler.hpp"
#include "Engine/Tools/Logging/Logger.hpp"
#include "Engine/Config/BuildConfig.hpp" // Enable/disable profiling in this file
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
const int MAX_NUMBER_OF_SAMPLES = 1024;


//-----------------------------------------------------------------------------------------------
ProfileSample* g_currentSample = nullptr;
ProfileSample* g_currentFrame = nullptr;
ProfileSample* g_previousFrame = nullptr;
ObjectPool< ProfileSample > g_samplePool;
bool g_enabled = false;
bool g_desiredEnabled = false;


//-----------------------------------------------------------------------------------------------
double GetPerformanceFrequency()
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );

	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}



//-----------------------------------------------------------------------------------------------
// Just gets the current performance count without converting seconds so that conversion cost
// isn't paid while profiling
uint64_t GetCurrentPerformanceCount()
{
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	return static_cast< uint64_t >( currentCount.QuadPart );
}


//-----------------------------------------------------------------------------------------------
// Converts a provided performance count to seconds so information can be presented in a human-
// readable way
double PerformanceCountToSeconds( uint64_t inCount )
{
	static double secondsPerCount = GetPerformanceFrequency();

	double currentSeconds = static_cast< double >( inCount ) * secondsPerCount;
	return currentSeconds;
}


//-----------------------------------------------------------------------------------------------
void ProfilerSystemStartup()
{
	// Set up object pool
	g_samplePool.Initialize( MAX_NUMBER_OF_SAMPLES );
	g_desiredEnabled = true;
}


//-----------------------------------------------------------------------------------------------
void ProfilerSystemShutdown()
{
	// Clear object pool
	g_samplePool.Shutdown();
}


//-----------------------------------------------------------------------------------------------
ProfileLogScoped::ProfileLogScoped( const char* tag )
{
	UNUSED( tag );
#ifdef PROGRAM_PROFILING
	m_sample.tag = tag;
	m_sample.Start();
#endif
}


//-----------------------------------------------------------------------------------------------
ProfileLogScoped::~ProfileLogScoped()
{
#ifdef PROGRAM_PROFILING
	m_sample.End();
	LoggerPrintfWithTag( "profiler", "%s took %.8fms\n", m_sample.tag, m_sample.GetSeconds() * 1000.0f );
#endif
}


//-----------------------------------------------------------------------------------------------
// Indicates when a frame starts and a frame ends
void ProfileFrameMark()
{
	if ( g_enabled )
	{
		ASSERT_OR_DIE( g_currentFrame == g_currentSample, "gCurrentFrame != g_currentSample" ); // Ensure we have popped everything up to this
		PopProfileSample(); // Pops g_currentFrame
		DeleteProfileSample( g_previousFrame ); // Performs a NULL check and deletes children
		g_previousFrame = g_currentFrame;
	}

	g_enabled = g_desiredEnabled;

	if ( g_enabled )
	{
		PushProfileSample( "frame" ); // Have "frame" be a constant somewhere
		g_currentFrame = g_currentSample;
	}
}


//-----------------------------------------------------------------------------------------------
void PopProfileSample()
{
 	ASSERT_OR_DIE( g_currentSample != nullptr, "gCurrentSample == nullptr" );
	g_currentSample->End();
	g_currentSample = g_currentSample->parentSample;
}


//-----------------------------------------------------------------------------------------------
void DeleteProfileSample( ProfileSample* sample )
{
	if ( sample == nullptr )
	{
		return;
	}
	
	ProfileSample* currentChildSample = sample->firstChildSample;
	if ( currentChildSample != nullptr )
	{
		while ( currentChildSample->nextSample != nullptr )
		{
			ProfileSample* nextChildSample = currentChildSample->nextSample;
			DeleteProfileSample( currentChildSample );
			currentChildSample = nextChildSample;
		}

		if ( currentChildSample->nextSample == nullptr )
		{
			DeleteProfileSample( currentChildSample );
		}
	}

	g_samplePool.Delete( sample );
}


//-----------------------------------------------------------------------------------------------
void PushProfileSample( const char* tag )
{
	ProfileSample* newSample = g_samplePool.Alloc();
	newSample->tag = tag;
	newSample->parentSample = g_currentSample;

	if ( g_currentSample != nullptr )
	{
		g_currentSample->AddChildSample( newSample );
	}

	g_currentSample = newSample;	
	g_currentSample->Start();
}


//-----------------------------------------------------------------------------------------------
ProfileSample* ProfilerGetLastFrame()
{
	return g_previousFrame;
}


//-----------------------------------------------------------------------------------------------
void ProfilerEnable( bool enable )
{
	g_desiredEnabled = enable;
}


//-----------------------------------------------------------------------------------------------
#ifdef PROGRAM_PROFILING
CONSOLE_COMMAND( toggle_profiling )
{
	UNUSED( args );
	g_desiredEnabled = !g_desiredEnabled;
}
#endif