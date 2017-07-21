#pragma once

#include <stdlib.h>
#include <queue>
#include <mutex>

#include "Engine/Tools/Memory/UntrackedAllocator.hpp"


//-----------------------------------------------------------------------------------------------
template < typename T >
class ThreadSafeQueue : protected std::queue< T, std::deque< T, TUntrackedAllocator< T > > >
{
public:
	std::mutex mutex;

	void Enqueue( T const &value )
	{
		mutex.lock();
		push( value );
		mutex.unlock();
	}

	bool Dequeue( T *out )
	{
		bool result = false;

		mutex.lock();
		if ( !empty() )
		{
			*out = front();
			pop();
			result = true;
		}
		mutex.unlock();

		return result;
	}

	size_t QueueSize()
	{
		return size();
	}
};