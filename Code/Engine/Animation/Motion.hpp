#pragma once

#include <string>


//-----------------------------------------------------------------------------------------------
class Skeleton;
struct mat44_fl;
class IBinaryWriter;
class IBinaryReader;


//-----------------------------------------------------------------------------------------------
enum WrapMode
{
	WRAP_MODE_CLAMP = 0,
	WRAP_MODE_LOOP,
	WRAP_MODE_PING_PONG
};


//-----------------------------------------------------------------------------------------------
class Motion
{
public:
	Motion();
	Motion( const std::string& name, float timeSpan, float frameRate, Skeleton* skeleton );
	~Motion();
	void GetFrameIndicesWithBlend( uint32_t &outFrameIndex0, uint32_t &outFrameIndex1,
		float &outBlend, // Normalized distance between frames
		float inTime );
	mat44_fl* GetJointKeyFrames( uint32_t jointIndex );
	void ApplyMotionToSkeleton( Skeleton* skeleton, float time );
	mat44_fl MatrixLerp( mat44_fl const &a, mat44_fl const &b, float const time );
	void Update( Skeleton* skeleton, float deltaSeconds ); // Total elapsed time, not delta seconds
	void Reset();

	// File I/O
	void WriteToFile( const std::string& filename );
	void ReadFromFile( const std::string& filename );
	void WriteToStream( IBinaryWriter& writer );
	void ReadFromStream( IBinaryReader& reader );

public:
	int m_jointCount;
	int m_frameCount;
	std::string m_name;
	float m_totalLengthSeconds;
	float m_frameRate;
	float m_frameTime;
	mat44_fl* m_keyframes;

	// Don't write these vars out
	float m_currentTime;
	float m_age;
	bool m_isPlaying = true;
	WrapMode m_wrapMode = WRAP_MODE_LOOP;
};