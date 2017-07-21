#include "Engine/Animation/Motion.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/FileUtils.hpp"


//-----------------------------------------------------------------------------------------------
Motion::Motion( const std::string& name, float timeSpan, float frameRate, Skeleton* skeleton )
	: m_name( name )
	, m_frameCount( ( int ) ceil( frameRate * timeSpan ) + 1 )
	, m_jointCount( skeleton->GetJointCount() )
	, m_totalLengthSeconds( timeSpan )
	, m_frameTime( 1.0f / frameRate )
	, m_frameRate( frameRate )
	, m_currentTime( 0.0f )
	, m_age( 0.0f )
{
	m_isPlaying = true;
	m_keyframes = new mat44_fl[ m_frameCount * m_jointCount ];
}


//-----------------------------------------------------------------------------------------------
Motion::Motion()
{
	m_age = 0.0f;
	m_currentTime = 0.0f;
	m_isPlaying = true;
	m_keyframes = new mat44_fl[ 1 * 1 ];
}


//-----------------------------------------------------------------------------------------------
Motion::~Motion()
{
	delete[] m_keyframes;
}


//-----------------------------------------------------------------------------------------------
void Motion::GetFrameIndicesWithBlend( uint32_t &outFrameIndex0, uint32_t &outFrameIndex1, 
	float &outBlend, // Normalized distance between frames
	float inTime )
{
	outFrameIndex0 = ( uint32_t ) floor( inTime / m_frameTime );
	outFrameIndex1 = outFrameIndex0 + 1;

	if ( outFrameIndex0 == ( uint32_t ) ( m_frameCount - 1 ) )
	{
		outFrameIndex1 = m_frameCount - 1;
		outBlend = 0.0f;
	}
	else if ( outFrameIndex0 == ( uint32_t ) ( m_frameCount - 2 ) )
	{
		float lastFrameTime = m_totalLengthSeconds - ( m_frameTime * outFrameIndex0 );
		outBlend = fmodf( inTime, m_frameTime ) / lastFrameTime;
		outBlend = ClampFloatZeroToOne( outBlend );
	}
	else
	{
		outBlend = fmodf( inTime, m_frameTime ) / m_frameTime;
	}
}


//-----------------------------------------------------------------------------------------------
mat44_fl* Motion::GetJointKeyFrames( uint32_t jointIndex )
{
	return m_keyframes + m_frameCount * jointIndex;
}


//-----------------------------------------------------------------------------------------------
void Motion::ApplyMotionToSkeleton( Skeleton* skeleton, float time )
{
	uint32_t frame0;
	uint32_t frame1;
	float blend;
	GetFrameIndicesWithBlend( frame0, frame1, blend, time );

	uint32_t jointCount = skeleton->GetJointCount();
	for ( uint32_t jointIndex = 0; jointIndex < jointCount; ++jointIndex )
	{
		mat44_fl *jointKeyframes = GetJointKeyFrames( jointIndex );
		mat44_fl &mat0 = jointKeyframes[ frame0 ];
		mat44_fl &mat1 = jointKeyframes[ frame1 ];

		mat44_fl newModel = MatrixLerp( mat0, mat1, blend );

		// Needs to set bone_to_model
		// (or set your matrix tree's worth to this, and set
		// bone to model on Skeleton world's array)
		skeleton->SetJointWorldTransform( jointIndex, newModel );
	}
}


//-----------------------------------------------------------------------------------------------
mat44_fl Motion::MatrixLerp( mat44_fl const &a, mat44_fl const &b, float const time )
{
	Vector3 r0, u0, f0, t0;
	MatrixGetBasis( &a, &r0, &u0, &f0, &t0 );

	Vector3 r1, u1, f1, t1;
	MatrixGetBasis( &b, &r1, &u1, &f1, &t1 );

	Vector3 r, u, f, t;
	r = SlerpForseth( r0, r1, time );
	u = SlerpForseth( u0, u1, time );
	f = SlerpForseth( f0, f1, time );
	t = Lerp( t0, t1, time );

	mat44_fl returnMat;
	MatrixSetBasis( &returnMat, r, u, f, t );
	return returnMat;
}


//-----------------------------------------------------------------------------------------------
void Motion::Update( Skeleton* skeleton, float deltaSeconds )
{
	if ( m_isPlaying )
	{
		m_age += deltaSeconds;
	}

	// Set m_currentTime based on the wrap mode
	switch ( m_wrapMode )
	{
		case ( WRAP_MODE_CLAMP ) : // Do nothing when we are after max time
		{
			if ( m_age > m_totalLengthSeconds )
			{
				m_currentTime = m_totalLengthSeconds;
			}
			else
			{
				m_currentTime = m_age;
			}
			break;
		}
		case ( WRAP_MODE_LOOP ) : // Start at beginning of animation when we are after max time
		{
			if ( m_age > m_totalLengthSeconds )
			{
				m_age = 0.0f;
			}
			m_currentTime = m_age;

			break;
		}
		case ( WRAP_MODE_PING_PONG ) :
		{
			if ( m_age > m_totalLengthSeconds * 2 )
			{
				m_age -= m_totalLengthSeconds * 2;
			}

			if ( m_age < m_totalLengthSeconds * 2 && m_age > m_totalLengthSeconds )
			{
				m_currentTime = m_totalLengthSeconds - fmodf( m_age, m_totalLengthSeconds );
			}
			else
			{
				m_currentTime = fmodf( m_age, m_totalLengthSeconds );
			}

			break;
		}
		default:
		{
			break;
		}
	}

	ApplyMotionToSkeleton( skeleton, m_currentTime );
}


//-----------------------------------------------------------------------------------------------
void Motion::Reset()
{
	m_age = 0.0f;
	m_currentTime = 0.0f;
}


//-----------------------------------------------------------------------------------------------
void Motion::WriteToFile( const std::string& filename )
{
	FileBinaryWriter writer;
	if ( writer.Open( filename ) )
	{
		WriteToStream( writer );
		writer.Close();
	}
}


//-----------------------------------------------------------------------------------------------
void Motion::ReadFromFile( const std::string& filename )
{
	FileBinaryReader reader;
	if ( reader.Open( filename ) )
	{
		ReadFromStream( reader );
		reader.Close();
	}
}


//-----------------------------------------------------------------------------------------------
void Motion::WriteToStream( IBinaryWriter& writer )
{
	writer.Write<int>( m_jointCount );
	writer.Write<int>( m_frameCount );
	writer.WriteString( m_name );
	writer.Write<float>( m_totalLengthSeconds );
	writer.Write<float>( m_frameRate );
	writer.Write<float>( m_frameTime );

	for ( int joint_idx = 0; joint_idx < m_jointCount; ++joint_idx )
	{
		mat44_fl *bone_keyframes = GetJointKeyFrames( joint_idx );

		for ( int frame_idx = 0; frame_idx < m_frameCount; ++frame_idx )
		{
			mat44_fl* bone_keyframe = bone_keyframes + frame_idx;

			writer.Write<mat44_fl>( *bone_keyframe );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Motion::ReadFromStream( IBinaryReader& reader )
{
	int joint_count = 0;
	int frame_count = 0;

	reader.Read<int>( &joint_count );
	reader.Read<int>( &frame_count );

	std::string motionName = "";
	reader.ReadString( &motionName );

	float totalSeconds = 0.0f;
	float frameRate = 0.0f;
	float frameTime = 0.0f;

	reader.Read<float>( &totalSeconds );
	reader.Read<float>( &frameRate );
	reader.Read<float>( &frameTime );

	m_jointCount = joint_count;
	m_frameCount = frame_count;
	m_name = motionName;
	m_totalLengthSeconds = totalSeconds;
	m_frameRate = frameRate;
	m_frameTime = frameTime;

	delete[] m_keyframes;
	m_keyframes = nullptr;
	m_keyframes = new mat44_fl[ frame_count * joint_count ];

	for ( int joint_idx = 0; joint_idx < joint_count; ++joint_idx )
	{
		mat44_fl *bone_keyframes = GetJointKeyFrames( joint_idx );

		for ( int frame_idx = 0; frame_idx < frame_count; ++frame_idx )
		{
			mat44_fl* bone_keyframe = bone_keyframes + frame_idx;

			mat44_fl thisJointMotion;
			reader.Read<mat44_fl>( &thisJointMotion );

			*bone_keyframe = thisJointMotion;
		}
	}
}