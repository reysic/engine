#include "Engine/Math/EulerAngles.hpp"


//-----------------------------------------------------------------------------------------------
EulerAngles::EulerAngles( float pitchDegreesAboutX, float rollDegreesAboutY, float yawDegreesAboutZ )
	: m_pitchDegreesAboutX( pitchDegreesAboutX )
	, m_rollDegreesAboutY( rollDegreesAboutY )
	, m_yawDegreesAboutZ( yawDegreesAboutZ )
{
}


//-----------------------------------------------------------------------------------------------
const EulerAngles EulerAngles::ZERO( 0.0f, 0.0f, 0.0f );