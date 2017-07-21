#include "Engine/Renderer/Sprites/SpriteSheet.hpp"
#include "Engine/Renderer/Sprites/SpriteAnimation.hpp"


//-----------------------------------------------------------------------------------------------
SpriteAnimation::SpriteAnimation( SpriteSheet* spriteSheet, float durationSeconds, SpriteAnimMode mode, int startSpriteIndex, int endSpriteIndex )
{
	m_spriteSheet = spriteSheet;
	m_mode = mode;
	m_startSpriteIndex = startSpriteIndex;
	m_endSpriteIndex = endSpriteIndex;

	m_isFinished = false;
	m_isPlaying = true;
	m_durationSeconds = durationSeconds;
	m_elapsedSeconds = 0.0f;
	m_fractionElapsed = 0.0f;
}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::Update( float deltaSeconds )
{
	m_elapsedSeconds += deltaSeconds;

	m_fractionElapsed = ( m_elapsedSeconds / m_durationSeconds );
}


//-----------------------------------------------------------------------------------------------
AABB2 SpriteAnimation::GetCurrentTexCoords() const
{
	float framesPerSecond = ( m_endSpriteIndex - m_startSpriteIndex ) / m_durationSeconds;
	float currentFrame = m_elapsedSeconds * framesPerSecond;

	int currentIndex = ( int ) floor( currentFrame );
	return ( m_spriteSheet->GetTexCoordsForSpriteIndex( currentIndex ) );
}


//-----------------------------------------------------------------------------------------------
Texture* SpriteAnimation::GetTexture() const
{
	return nullptr;
}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::Pause()
{

}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::Resume()
{

}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::Reset()
{

}


//-----------------------------------------------------------------------------------------------
float SpriteAnimation::GetSecondsRemaining() const
{
	return ( m_durationSeconds - m_elapsedSeconds );
}


//-----------------------------------------------------------------------------------------------
float SpriteAnimation::GetFractionElapsed() const
{
	return ( m_elapsedSeconds / m_durationSeconds );
}


//-----------------------------------------------------------------------------------------------
float SpriteAnimation::GetFractionRemaining() const
{
	return ( 1.0f - ( m_elapsedSeconds / m_durationSeconds ) );
}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::SetSecondsElapsed( float secondsElapsed )
{
	m_elapsedSeconds = secondsElapsed;
}


//-----------------------------------------------------------------------------------------------
void SpriteAnimation::SetFractionElapsed( float fractionElapsed )
{
	m_fractionElapsed = fractionElapsed;
}