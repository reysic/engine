#pragma once

#include <vector>
#include <atomic>

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Tools/Profiling/ObjectPool.hpp"
#include "Engine/Tools/Logging/ThreadSafeQueue.hpp"


//-----------------------------------------------------------------------------------------------
class Job;
class TheJobSystem;


//-----------------------------------------------------------------------------------------------
extern TheJobSystem* g_theJobSystem;


//-----------------------------------------------------------------------------------------------
enum JobCategory
{
	JOB_CATEGORY_GENERIC = 0, // Can run on main thread
	JOB_CATEGORY_GENERIC_SLOW, // Shouldn't run on main thread, but can
	NUM_JOB_CATEGORIES
};


//-----------------------------------------------------------------------------------------------
typedef unsigned char byte_t;
typedef void ( JobCallback )( Job* );
class Job
{
public:
	template < typename T >
	T JobRead()
	{
		T retData;

		// ASSERT
		memcpy( &retData, m_byteBuffer + m_readHead, sizeof( T ) );
		m_readHead += sizeof( T );
		return retData;
	}

	template < typename T >
	void JobWrite( T const &v )
	{
		// ASSERT
		memcpy( m_byteBuffer + m_writeHead, &v, sizeof( T ) );
		m_writeHead += sizeof( T );
	}

	void EndJobRead();
	void EndJobWrite();

	void DoWork();
	void FinishJob();

public:
	std::atomic< int > m_refCount;
	byte_t m_byteBuffer[ 512 ];
	unsigned int m_readHead;
	unsigned int m_writeHead;
	JobCategory m_jobCategory;
	JobCallback* m_callbackFunc;
};


//-----------------------------------------------------------------------------------------------
class JobConsumer
{
public:
	JobConsumer();

	void AddCategory( JobCategory category );
	void ConsumeAll();
	bool Consume();
	void FinishJob( Job* job );

public:
	std::vector< JobCategory > m_categoriesToConsume;
};


//-----------------------------------------------------------------------------------------------
class TheJobSystem
{
public:
	TheJobSystem( unsigned int numJobCategories, unsigned int numWorkerThreads );
	~TheJobSystem();

	void Startup();
	void Shutdown();

	unsigned int GetSystemCoreCount() const;

	Job* JobCreate( JobCategory category, JobCallback* callback);
	void JobDispatch( Job* job );
	void JobDetach( Job* job );
	void JobJoin( Job* job );

	ThreadSafeQueue< Job* >* GetQueueForCategory( JobCategory category );

public:
	unsigned int m_numJobCategories;
	unsigned int m_numWorkerThreads;
	unsigned int m_numSystemCores;
	std::vector< ThreadSafeQueue< Job* >* > m_jobQueue;
	std::vector< std::thread > m_threads;
	ObjectPool< Job > m_jobPool;
	bool m_isRunning;
};


//-----------------------------------------------------------------------------------------------
void JobThread();