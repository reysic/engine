#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sysinfoapi.h>

#include "Engine/Tools/Jobs/JobSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
TheJobSystem* g_theJobSystem = nullptr;


//-----------------------------------------------------------------------------------------------
TheJobSystem::TheJobSystem( unsigned int numJobCategories, unsigned int numWorkerThreads )
	: m_numJobCategories( numJobCategories )
	, m_numWorkerThreads( numWorkerThreads )
	, m_numSystemCores( GetSystemCoreCount() )
	, m_isRunning( false )
{
	m_jobPool.Initialize( 1024 );

	m_jobQueue.resize( NUM_JOB_CATEGORIES );

	for ( int j = 0; j < JobCategory::NUM_JOB_CATEGORIES; j++ )
	{
		m_jobQueue[ j ] = new ThreadSafeQueue< Job* >();
	}
}


//-----------------------------------------------------------------------------------------------
TheJobSystem::~TheJobSystem()
{
	Shutdown();
}


//-----------------------------------------------------------------------------------------------
void TheJobSystem::Startup()
{
	m_isRunning = true;

	int threadsToUse = m_numWorkerThreads;

	if ( m_numWorkerThreads < 0 )
	{
		threadsToUse = m_numSystemCores + m_numWorkerThreads;
	}

	if ( threadsToUse <= 0 )
	{
		threadsToUse = 1;
	}

	for ( int i = 0; i < threadsToUse; i++ )
	{
		m_threads.push_back( std::thread( &( JobThread ) ) );
	}

	LoggerPrintf( "The Job System initialized.\n" );
}


//-----------------------------------------------------------------------------------------------
void TheJobSystem::Shutdown()
{
	m_isRunning = false;

	for ( unsigned int i = 0; i < m_threads.size(); ++i )
	{
		m_threads[ i ].join();
	}

	m_jobPool.Shutdown();

	for ( int j = 0; j > JobCategory::NUM_JOB_CATEGORIES; j++ )
	{
		delete m_jobQueue[ j ];
	}

	LoggerPrintf( "The Job System de-initialized.\n" );
}


//-----------------------------------------------------------------------------------------------
unsigned int TheJobSystem::GetSystemCoreCount() const
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );

	return ( unsigned int ) sysinfo.dwNumberOfProcessors;
}


//-----------------------------------------------------------------------------------------------
Job* TheJobSystem::JobCreate( JobCategory category, JobCallback* callback )
{
	Job* newJob = m_jobPool.Alloc();

	newJob->m_callbackFunc = callback;
	newJob->m_jobCategory = category;
	newJob->m_readHead = 0;
	newJob->m_writeHead = 0;
	newJob->m_refCount = 1;

	return newJob;
}


//-----------------------------------------------------------------------------------------------
void TheJobSystem::JobDispatch( Job* job )
{
	ThreadSafeQueue< Job* >* jobQueue = GetQueueForCategory( job->m_jobCategory );
	job->m_refCount++;
	jobQueue->Enqueue( job );
}


//-----------------------------------------------------------------------------------------------
void TheJobSystem::JobDetach( Job* job )
{
	job->m_refCount--;
	if ( job->m_refCount == 0 )
	{
		m_jobPool.Delete( job );
	}
}


//-----------------------------------------------------------------------------------------------
void TheJobSystem::JobJoin( Job* job )
{
	while ( job->m_refCount == 2 );
	JobDetach( job );
}


//-----------------------------------------------------------------------------------------------
ThreadSafeQueue< Job* >* TheJobSystem::GetQueueForCategory( JobCategory category )
{
	return m_jobQueue[ category ];
}


//-----------------------------------------------------------------------------------------------
JobConsumer::JobConsumer()
{
}


//-----------------------------------------------------------------------------------------------
void JobConsumer::AddCategory( JobCategory category )
{
	m_categoriesToConsume.push_back( category );
}


//-----------------------------------------------------------------------------------------------
void JobConsumer::ConsumeAll()
{
	while ( Consume() );
}


//-----------------------------------------------------------------------------------------------
bool JobConsumer::Consume()
{
	for each ( JobCategory category in m_categoriesToConsume )
	{
		ThreadSafeQueue< Job* >* thisQueue = g_theJobSystem->GetQueueForCategory( category );
		Job* thisJob;
		if ( thisQueue->Dequeue( &thisJob ) )
		{
			thisJob->DoWork();
			thisJob->FinishJob();
			return true;
		}
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
void JobConsumer::FinishJob( Job* job )
{
	g_theJobSystem->JobDetach( job );
}


//-----------------------------------------------------------------------------------------------
void Job::EndJobRead()
{
	m_readHead = 0;
}


//-----------------------------------------------------------------------------------------------
void Job::EndJobWrite()
{
	m_writeHead = 0;
}


//-----------------------------------------------------------------------------------------------
void Job::DoWork()
{
	( *m_callbackFunc )( this );
}


//-----------------------------------------------------------------------------------------------
void Job::FinishJob()
{
	m_refCount--;
}


//-----------------------------------------------------------------------------------------------
void JobThread()
{
	JobConsumer consumer = JobConsumer();

	consumer.AddCategory( JOB_CATEGORY_GENERIC_SLOW );
	consumer.AddCategory( JOB_CATEGORY_GENERIC );

	while ( g_theJobSystem->m_isRunning )
	{
		consumer.ConsumeAll();
		std::this_thread::yield();
	}

	consumer.ConsumeAll();
}
