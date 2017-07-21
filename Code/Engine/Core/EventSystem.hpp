#pragma once

#include <map>
#include <vector>


//-----------------------------------------------------------------------------------------------
enum PropertyGetResult
{
	PGR_SUCCESS,
	PGR_FAILED_WRONG_TYPE,
	PGR_FAILED_NOT_PRESENT,
	PGR_FAILED_EMPTY
};


//-----------------------------------------------------------------------------------------------
enum PropertySetResult
{
	PSR_SUCCESS
};


//-----------------------------------------------------------------------------------------------
class NamedProperty
{
public:
	NamedProperty();
	virtual ~NamedProperty();
};


//-----------------------------------------------------------------------------------------------
template< typename T_PropertyType >
class TypedNamedProperty : public NamedProperty
{
public:
	TypedNamedProperty( const T_PropertyType& data )
		: m_data( data ) { }
	~TypedNamedProperty() { }

public:
	T_PropertyType m_data;
};


//-----------------------------------------------------------------------------------------------
class NamedProperties
{
public:
	NamedProperties();
	template< typename T_PropertyType >
	NamedProperties( const std::string& propertyName, const T_PropertyType& propertyValue )
	{
		Set( propertyName, propertyValue );
	}
	template< typename T_PropertyType, typename U_PropertyType >
	NamedProperties( const std::string& propertyName1, const T_PropertyType& propertyValue1,
		const std::string& propertyName2, const U_PropertyType& propertyValue2 )
	{
		Set( propertyName1, propertyValue1 );
		Set( propertyName2, propertyValue2 );
	}
	template< typename T_PropertyType, typename U_PropertyType, typename V_PropertyType >
	NamedProperties( const std::string& propertyName1, const T_PropertyType& propertyValue1,
		const std::string& propertyName2, const U_PropertyType& propertyValue2,
		const std::string& propertyName3, const V_PropertyType& propertyValue3 )
	{
		Set( propertyName1, propertyValue1 );
		Set( propertyName2, propertyValue2 );
		Set( propertyName3, propertyValue3 );
	}

	template< typename T_PropertyType >
	PropertyGetResult Get( const std::string& propertyName, T_PropertyType& out_propertyValue ) const
	{
		if ( m_properties.size() == 0 )
		{
			return PGR_FAILED_EMPTY;
		}

		std::map< const std::string, NamedProperty* >::const_iterator foundElement = m_properties.find( propertyName );
		if ( foundElement == m_properties.end() )
		{
			// Didn't find the property
			return PGR_FAILED_NOT_PRESENT;
		}

		TypedNamedProperty< T_PropertyType >* typedPropertyValue = dynamic_cast< TypedNamedProperty< T_PropertyType >* >( foundElement->second );
		// If a dynamic cast fails, it is nullptr
		if ( typedPropertyValue == nullptr )
		{
			return PGR_FAILED_WRONG_TYPE;
		}

		out_propertyValue = typedPropertyValue->m_data;
		return PGR_SUCCESS;
	}
	template< typename T_PropertyType >
	PropertySetResult Set( const std::string& propertyName, const T_PropertyType& propertyValue )
	{
		Remove( propertyName ); // Replaces property if already exists
		// could have removed return success or fail,
		// then have PSR_ADDED or PSR_REPLACED

		TypedNamedProperty< T_PropertyType >* typedPropertyValue = new TypedNamedProperty< T_PropertyType >( propertyValue );
		m_properties[ propertyName ] = ( NamedProperty* ) typedPropertyValue ;
		return PSR_SUCCESS;
	}
	void Remove( const std::string& propertyName )
	{
		std::map< const std::string, NamedProperty* >::iterator foundElement = m_properties.find( propertyName );
		if ( foundElement == m_properties.end() )
		{
			// Didn't find the property
			return;
		}

		delete foundElement->second;
		m_properties.erase( foundElement );
	}

public:
	std::map< const std::string, NamedProperty* > m_properties;
};


//-----------------------------------------------------------------------------------------------
class RegisteredObjectBase
{
public:
	RegisteredObjectBase() { }
	virtual ~RegisteredObjectBase() { }
	virtual void Execute( NamedProperties& params ) = 0;
};


//-----------------------------------------------------------------------------------------------
template< typename T_ObjectType >
class RegisteredObjectMethod : public RegisteredObjectBase
{
public:
	RegisteredObjectMethod( T_ObjectType* object, void ( T_ObjectType::*method )( NamedProperties& ) )
		: m_object( object )
		, m_method( *( void** ) &method )
	{
	}
	~RegisteredObjectMethod() { }

	T_ObjectType*	m_object;
	void*			m_method;

	void Execute( NamedProperties& params )
	{
		typedef void ( T_ObjectType::*method )( NamedProperties& );
		method objectMethod = *( method* ) &m_method;
		( m_object->*objectMethod )( params ); // ->* is pointer to member function operator
	}
};


//-----------------------------------------------------------------------------------------------
typedef void( EventCallbackFunc )( NamedProperties& );
class EventSystem
{
public:
	static EventSystem* Instance();
	
	EventSystem() { }
	~EventSystem() { }
	void RegisterFunction( const std::string& eventName, EventCallbackFunc* functionName );
	template< typename T_ObjectType >
	void RegisterMethod( const std::string& eventName, T_ObjectType* object, void ( T_ObjectType::*method )( NamedProperties& ) )
	{
		RegisteredObjectMethod< T_ObjectType >* registeredObjectMethod = new RegisteredObjectMethod< T_ObjectType >( object, method );

		std::map< std::string, std::vector< RegisteredObjectBase* > >::iterator foundEvent = m_registeredMethods.find( eventName );
		if ( foundEvent == m_registeredMethods.end() )
		{
			// Didn't find the event, so hasn't been registered yet
			m_registeredMethods[ eventName ] = std::vector< RegisteredObjectBase* >();
			foundEvent = m_registeredMethods.find( eventName );
		}

		// Event has been registered, so add method callback
		foundEvent->second.push_back( ( RegisteredObjectBase* ) registeredObjectMethod );
	}
	void FireEvent( const std::string& eventName );
	void FireEvent( const std::string& eventName, NamedProperties& params );
	template< typename T_ObjectType >
	void UnregisterFromAllEvents( T_ObjectType* object )
	{
		// Iterate over map of registered methods
		typedef std::map< std::string, std::vector< RegisteredObjectBase* > >::iterator registeredMethodIter;
		for ( registeredMethodIter iterator = m_registeredMethods.begin(); iterator != m_registeredMethods.end(); ++iterator )
		{
			// Iterate over vector of RegisteredObjectBase*
			for ( unsigned int vecIndex = 0; vecIndex < iterator->second.size(); ++vecIndex )
			{
				RegisteredObjectMethod* method = iterator->second[ vecIndex ];
				if ( method->m_object == object )
				{
					// Erase vector element corresponding to parameter object
					iterator->second.erase( iterator->second.begin() + vecIndex );
				}
			}
		}
	}
	
public:
	std::map< std::string, std::vector< EventCallbackFunc* > > m_registeredFuncs;
	std::map< std::string, std::vector< RegisteredObjectBase* > > m_registeredMethods;
};