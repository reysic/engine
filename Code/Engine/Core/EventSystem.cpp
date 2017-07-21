#include "Engine/Core/EventSystem.hpp"


//-----------------------------------------------------------------------------------------------
NamedProperties::NamedProperties()
{

}


//-----------------------------------------------------------------------------------------------
NamedProperty::NamedProperty()
{

}


//-----------------------------------------------------------------------------------------------
NamedProperty::~NamedProperty()
{

}


//-----------------------------------------------------------------------------------------------
EventSystem* g_theEventSystem = nullptr;


//-----------------------------------------------------------------------------------------------
EventSystem* EventSystem::Instance()
{
	if ( g_theEventSystem == nullptr )
	{
		g_theEventSystem = new EventSystem();
	}

	return g_theEventSystem;
}


//-----------------------------------------------------------------------------------------------
void EventSystem::RegisterFunction( const std::string& eventName, EventCallbackFunc* functionName )
{
	std::map< std::string, std::vector< EventCallbackFunc* > >::iterator foundEvent = m_registeredFuncs.find( eventName );
	if ( foundEvent == m_registeredFuncs.end() )
	{
		// Didn't find the event, so hasn't been registered yet
		m_registeredFuncs[ eventName ] = std::vector< EventCallbackFunc* >();
		foundEvent = m_registeredFuncs.find( eventName );
	}

	// Event has been registered, so add function callback
	foundEvent->second.push_back( functionName );
}


//-----------------------------------------------------------------------------------------------
void EventSystem::FireEvent( const std::string& eventName )
{
	NamedProperties params;

	// Start with stand-alone functions
	std::map< std::string, std::vector< EventCallbackFunc* > >::iterator foundEvent = m_registeredFuncs.find( eventName );
	if ( foundEvent != m_registeredFuncs.end() )
	{
		// Found the event, so call function callbacks
		for ( unsigned int vecIndex = 0; vecIndex < foundEvent->second.size(); ++vecIndex )
		{
			foundEvent->second[ vecIndex ]( params );
		}
	}

	// Now methods
	std::map< std::string, std::vector< RegisteredObjectBase* > >::iterator foundEventForMethod = m_registeredMethods.find( eventName );
	if ( foundEventForMethod != m_registeredMethods.end() )
	{
		// Found the event, so call function callbacks
		for ( unsigned int vecIndex = 0; vecIndex < foundEventForMethod->second.size(); ++vecIndex )
		{
			foundEventForMethod->second[ vecIndex ]->Execute( params );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void EventSystem::FireEvent( const std::string& eventName, NamedProperties& params )
{
	// Start with stand-alone functions
	std::map< std::string, std::vector< EventCallbackFunc* > >::iterator foundEvent = m_registeredFuncs.find( eventName );
	if ( foundEvent != m_registeredFuncs.end() )
	{
		// Found the event, so call function callbacks
		for ( unsigned int vecIndex = 0; vecIndex < foundEvent->second.size(); ++vecIndex )
		{
			foundEvent->second[ vecIndex ]( params );
		}
	}

	// Now methods
	std::map< std::string, std::vector< RegisteredObjectBase* > >::iterator foundEventForMethod = m_registeredMethods.find( eventName );
	if ( foundEventForMethod != m_registeredMethods.end() )
	{
		// Found the event, so call function callbacks
		for ( unsigned int vecIndex = 0; vecIndex < foundEventForMethod->second.size(); ++vecIndex )
		{
			foundEventForMethod->second[ vecIndex ]->Execute( params );
		}
	}
}