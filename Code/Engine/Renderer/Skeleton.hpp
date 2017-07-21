#pragma once

#include <string>
#include <vector>

#include "Engine/Math/Matrix4x4.hpp"


//-----------------------------------------------------------------------------------------------
class IBinaryWriter;
class IBinaryReader;


//-----------------------------------------------------------------------------------------------
struct Joint
{
	Joint() {}
	Joint( const std::string& jointName, int jointParentIndex, const mat44_fl& initialBoneSpace )
		: m_jointName( jointName )
		, m_jointParentIndex( jointParentIndex )
		, m_initialBoneSpace( initialBoneSpace)
	{
	}
	std::string m_jointName;
	int m_jointParentIndex;
	mat44_fl m_initialBoneSpace;

};


//-----------------------------------------------------------------------------------------------
class Skeleton
{
public:
	void AddJoint( const std::string& jointName, int jointParentIndex, const mat44_fl& initialModelSpace );
	int GetLastAddedIndex() const { return m_jointNames.size() - 1; }
	int GetJointCount() const { return m_jointNames.size(); }
	Vector3 GetJointPosition( int jointIndex ) const;
	Joint* GetJointByIndex( int jointIndex );
	int GetIndexOfParent( int jointIndex ) const;
	void SetJointWorldTransform( int jointIndex, mat44_fl modelTransform );
	std::vector< mat44_fl> const GetBoneMatrices();
	int GetJointIndexForNodeName( const std::string& nodeName );

	// File I/O
	void WriteToFile( const std::string& filename );
	void ReadFromFile( const std::string& filename );
	void WriteToStream( IBinaryWriter& writer );
	void ReadFromStream( IBinaryReader& reader );

public:
	std::vector< std::string > m_jointNames;
	std::vector< int > m_jointParentIndicies;
	std::vector< mat44_fl > m_modelToBoneSpace;	// For initial model space layout
	std::vector< mat44_fl > m_boneToModelSpace;	// For current bone's model space
	std::vector< Joint* > m_joints;
};