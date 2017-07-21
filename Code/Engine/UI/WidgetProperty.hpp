#pragma once


//-----------------------------------------------------------------------------------------------
enum PropertyType
{
	PROPERTY_TYPE_INT,
	PROPERTY_TYPE_FLOAT,
	PROPERTY_TYPE_VECTOR2,
	PROPERTY_TYPE_STRING,
	PROPERTY_TYPE_RGBA,
	NUM_PROPERTY_TYPES
};


//-----------------------------------------------------------------------------------------------
class WidgetProperty
{
public:
	constexpr WidgetProperty( const char* propertyName, PropertyType propertyType )
		: m_propertyName( propertyName )
		, m_propertyType( propertyType )
	{
	}

	const char* GetPropertyName() const;
	PropertyType GetPropertyType() const;

	static PropertyType GetPropertyTypeForName( const char* propertyName );

public:
	const char*		m_propertyName;
	PropertyType	m_propertyType;
};


//-----------------------------------------------------------------------------------------------
constexpr WidgetProperty s_widgetProperties[] =
{
	WidgetProperty( "WidgetType", PROPERTY_TYPE_STRING ),
	WidgetProperty( "WidgetName", PROPERTY_TYPE_STRING ),
	WidgetProperty( "WidgetDimensions", PROPERTY_TYPE_VECTOR2 ),
	WidgetProperty( "CenterCoords", PROPERTY_TYPE_VECTOR2 ),
	WidgetProperty( "TextString", PROPERTY_TYPE_STRING ),
	WidgetProperty( "EventName", PROPERTY_TYPE_STRING ),
	WidgetProperty( "BackgroundColor", PROPERTY_TYPE_RGBA ),
	WidgetProperty( "BorderWidth", PROPERTY_TYPE_INT ),
	WidgetProperty( "BorderColor", PROPERTY_TYPE_RGBA ),
	WidgetProperty( "TextString", PROPERTY_TYPE_STRING ),
	WidgetProperty( "TextCellHeight", PROPERTY_TYPE_FLOAT ),
	WidgetProperty( "TextColor", PROPERTY_TYPE_RGBA ),
	WidgetProperty( "TexturePath", PROPERTY_TYPE_STRING ),
	WidgetProperty( "AnchorPosition", PROPERTY_TYPE_INT )
};


//-----------------------------------------------------------------------------------------------
constexpr int GetNumberOfWidgetProperties()
{
	return ( sizeof ( s_widgetProperties ) / sizeof( s_widgetProperties[ 0 ] ) );
}