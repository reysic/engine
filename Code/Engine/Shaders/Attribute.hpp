#pragma once

#include <string>


//-------------------------------------------------------------------------------------------------
// #TODO: Currently not used, but maybe one day...
struct Attribute
{
public:
	int m_bindPoint;
	int m_length;
	int m_size;
	unsigned int m_type;
	std::string m_name;
	void* const m_data;

public:
	Attribute( int bindPoint, int length, int size, unsigned int type, std::string const &name, void* const data = nullptr );
	~Attribute();

	int GetBindPoint() const { return m_bindPoint; }
};