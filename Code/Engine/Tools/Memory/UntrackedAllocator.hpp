#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdlib.h>
#pragma warning (disable: 4091)
#include <DbgHelp.h>
#include <iostream>

//#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
template <typename T>
class TUntrackedAllocator
{
public:
	//    typedefs
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

public:
	//    convert an allocator<T> to allocator<U>
	template<typename U>
	struct rebind
	{
		typedef TUntrackedAllocator<U> other;
	};

public:
	inline explicit TUntrackedAllocator() {}
	inline ~TUntrackedAllocator() {}
	inline explicit TUntrackedAllocator( TUntrackedAllocator const& ) {}
	template<typename U>
	inline explicit TUntrackedAllocator( TUntrackedAllocator<U> const& ) {}

	//    address
	inline pointer address( reference r )
	{
		return &r;
	}

	inline const_pointer address( const_reference r )
	{
		return &r;
	}

	//    memory allocation
	inline pointer allocate( size_type cnt,
		typename std::allocator<void>::const_pointer = 0 )
	{
		T *ptr = ( T* ) malloc( cnt * sizeof( T ) );
		return ptr;
	}

	inline void deallocate( pointer p, size_type cnt )
	{
		cnt;
		free( p );
	}

	//   size
	inline size_type max_size() const
	{
		return ( std::numeric_limits<size_type>::max )( ) / sizeof( T );
	}

	// construction/destruction
	inline void construct( pointer p, const T& t )
	{
		new( p ) T( t );
	}

	inline void destroy( pointer p )
	{
		p;
		p->~T();
	}

	inline bool operator==( TUntrackedAllocator const& a ) { return this == &a; }
	inline bool operator!=( TUntrackedAllocator const& a ) { return !operator==( a ); }
};