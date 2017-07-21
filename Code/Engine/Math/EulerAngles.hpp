#pragma once


//-----------------------------------------------------------------------------------------------
class EulerAngles
{
public:
	EulerAngles( float pitchDegreesAboutX, float rollDegreesAboutY, float yawDegreesAboutZ );

	static const EulerAngles ZERO;

public:
	float m_pitchDegreesAboutX;
	float m_rollDegreesAboutY;
	float m_yawDegreesAboutZ;
};