#pragma once

#include "Engine/Math/Vector2.hpp"


//-----------------------------------------------------------------------------------------------
enum InterpolationMode
{
	INTERPOLATION_MODE_CONSTANT,
	INTERPOLATION_MODE_LINEAR,
	INTERPOLATION_MODE_CUBIC
};


//-----------------------------------------------------------------------------------------------
enum ExtrapolationMode
{
	EXTRAPOLATION_MODE_CONSTANT,
	EXTRAPOLATION_MODE_LINEAR
};


//-----------------------------------------------------------------------------------------------
struct AnimationKey
{
	Vector2 m_position;
	float m_leftVelocity;
	float m_rightVelocity;
	InterpolationMode m_interpMode;
	ExtrapolationMode m_extrapMode;
};


//-----------------------------------------------------------------------------------------------
class AnimationCurve
{

};