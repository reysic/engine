#pragma once

#include "Engine/Math/Vector4.hpp"


//-----------------------------------------------------------------------------------------------
class Rgba
{
public:
	Rgba();
	Rgba( unsigned char r, unsigned char g, unsigned char b, unsigned char a );
	//Rgba( float r, float g, float b, float a );
	Rgba( const Rgba & rgba );
	Rgba( const Rgba * rgba );
	Vector4 Rgba::FloatRepresentation() const;
	static const Rgba WHITE;
	static const Rgba BLACK;
	static const Rgba RED;
	static const Rgba BLUE;
	static const Rgba GREEN;
	static const Rgba CYAN;
	static const Rgba PINK;
	static const Rgba GRAY;
	static const Rgba YELLOW;
	static const Rgba ORANGE;
	static const Rgba MAGENTA;
	static const Rgba NAVY;
	static const Rgba REDORANGE;
	static const Rgba LIGHTOLIVEGREEN;
	static const Rgba GOLDENROD;
	static const Rgba VIOLET;
	friend bool operator==( const Rgba& lhs, const Rgba& rhs );
	friend bool operator!=( const Rgba& lhs, const Rgba& rhs );

public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};