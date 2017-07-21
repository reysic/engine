#pragma once


//-----------------------------------------------------------------------------------------------
struct PageNode
{
	PageNode* nextNode;
};


//-----------------------------------------------------------------------------------------------
template < typename T >
class ObjectPool
{
public:
	void Initialize( const size_t& numberOfObjects )
	{
		size_t bufferSize = sizeof( T ) * numberOfObjects;
		/*ASSERT( sizeof( T ) >= sizeof( page_node ) );*/
		m_buffer = ( T* ) malloc( bufferSize );
		m_freeStack = nullptr;

		for ( int32_t objectIndex = numberOfObjects - 1; objectIndex >= 0; objectIndex-- )
		{
			T* pointerToObject = &m_buffer[ objectIndex ];
			PageNode* nodePointer = ( PageNode* ) pointerToObject;
			nodePointer->nextNode = m_freeStack;
			m_freeStack = nodePointer;
		}
	}

	void Shutdown()
	{
		free( m_buffer );
	}

	T* Alloc()
	{
		T* t = ( T* ) m_freeStack;
		m_freeStack = m_freeStack->nextNode;
		new ( t ) T();
		return t;
	}

	void Delete( T* t )
	{
		t->~T();
		PageNode* n = ( PageNode* ) t;
		n->nextNode = m_freeStack;
		m_freeStack = n;
	}

public:
	// Members
	PageNode* m_freeStack;
	T* m_buffer;
};