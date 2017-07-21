#include <string.h>

#include "Engine/UI/WidgetProperty.hpp"


//-----------------------------------------------------------------------------------------------
const char* WidgetProperty::GetPropertyName() const
{
	return m_propertyName;
}


//-----------------------------------------------------------------------------------------------
PropertyType WidgetProperty::GetPropertyType() const
{
	return m_propertyType;
}


//-----------------------------------------------------------------------------------------------
PropertyType WidgetProperty::GetPropertyTypeForName( const char* propertyName )
{
	for ( int propertyIndex = 0; propertyIndex < GetNumberOfWidgetProperties(); ++propertyIndex )
	{
		const char* nameForPropertyIndex = s_widgetProperties[ propertyIndex ].GetPropertyName();
		if ( strcmp( nameForPropertyIndex, propertyName ) == 0 )
		{
			return s_widgetProperties[ propertyIndex ].GetPropertyType();
		}
	}

	return NUM_PROPERTY_TYPES;
}
