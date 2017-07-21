#pragma once

#include <stdint.h>

#include "Engine/Tools/Profiling/ObjectPool.hpp"


//-----------------------------------------------------------------------------------------------
double GetPerformanceFrequency();
uint64_t GetCurrentPerformanceCount();
double PerformanceCountToSeconds( uint64_t inCount );
void ProfilerSystemStartup();
void ProfilerSystemShutdown();


//-----------------------------------------------------------------------------------------------
struct ProfileSample
{
	void Start()
	{
		startTime = GetCurrentPerformanceCount();
	}

	void End()
	{
		endTime = GetCurrentPerformanceCount();
	}

	uint64_t GetCycles()
	{
		return ( endTime - startTime );
	}

	uint64_t GetSelfCycles()
	{
		uint64_t totalChildrenCycles = 0;

		ProfileSample* currentChildSample = firstChildSample;
		if ( currentChildSample != nullptr )
		{
			totalChildrenCycles = currentChildSample->GetCycles();

			while ( currentChildSample->nextSample != nullptr )
			{
				currentChildSample = currentChildSample->nextSample;
				totalChildrenCycles = currentChildSample->GetCycles();
			}
		}

		return ( GetCycles() - totalChildrenCycles );
	}

	double GetSeconds()
	{
		return PerformanceCountToSeconds( endTime - startTime );
	}

	void AddChildSample( ProfileSample* childSample )
	{
		if ( firstChildSample == nullptr )
		{
			firstChildSample = childSample;
			return;
		}

		ProfileSample* lastChildSample = firstChildSample;
		while ( lastChildSample->nextSample != nullptr )
		{
			lastChildSample = lastChildSample->nextSample;
		}

		lastChildSample->nextSample = childSample;
	}

	const char* tag;
	uint64_t startTime;
	uint64_t endTime;
	ProfileSample* parentSample;
	ProfileSample* firstChildSample;
	ProfileSample* nextSample;
};


//-----------------------------------------------------------------------------------------------
class ProfileLogScoped
{
public:
	ProfileLogScoped( const char* tag );
	~ProfileLogScoped();

public:
	ProfileSample m_sample;
};


//-----------------------------------------------------------------------------------------------
#define PROFILE_LOG_SCOPE( tag ) ProfileLogScoped _S_##__LINE__##( tag )


//-----------------------------------------------------------------------------------------------
extern ProfileSample* g_currentSample;
extern ProfileSample* g_currentFrame;
extern ProfileSample* g_previousFrame;
extern ObjectPool< ProfileSample > g_samplePool;
extern bool g_enabled;
extern bool g_desiredEnabled;


//-----------------------------------------------------------------------------------------------
void ProfileFrameMark();
void PopProfileSample();
void DeleteProfileSample( ProfileSample* sample );
void PushProfileSample( const char* tag );
ProfileSample* ProfilerGetLastFrame();
void ProfilerEnable( bool enable );