
#pragma once

#pragma comment( lib, "fmodex_vc" ) // Link in the fmodex_vc.lib static library

#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------------------------
typedef unsigned int SoundID;
typedef void* AudioChannelHandle;
const unsigned int MISSING_SOUND_ID = 0xffffffff;


//-----------------------------------------------------------------------------------------------
class AudioSystem;


//-----------------------------------------------------------------------------------------------
extern AudioSystem* g_theAudioSystem;


//-----------------------------------------------------------------------------------------------
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();
	SoundID CreateOrGetSound( const char* soundFileName );
	AudioChannelHandle PlaySound( SoundID soundID, float volumeLevel = 1.0f, float pitchModifier = 1.0f );
	void StopChannel( AudioChannelHandle channel );
	void Update( float deltaSeconds ); // Must be called at regular intervals (e.g. every frame)

protected:
	void InitializeFMOD();
	void ValidateResult( FMOD_RESULT result );

protected:
	FMOD::System*						m_fmodSystem;
	std::map< const char*, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
};


//---------------------------------------------------------------------------
void InitializeAudio();