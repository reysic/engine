#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
void Skeleton::AddJoint( const std::string& jointName, int parentJointIndex, const mat44_fl& initialModelSpace )
{
	m_jointNames.push_back( jointName );
	m_jointParentIndicies.push_back( parentJointIndex );
	m_boneToModelSpace.push_back( initialModelSpace );

	mat44_fl modelToBoneMatrix = initialModelSpace;
	MatrixInvert( &modelToBoneMatrix );
	m_modelToBoneSpace.push_back( modelToBoneMatrix );

	Joint* newJoint = new Joint();
	newJoint->m_jointName = jointName;
	newJoint->m_initialBoneSpace = initialModelSpace;
	newJoint->m_jointParentIndex = parentJointIndex;

	m_joints.push_back( newJoint );
}


//-----------------------------------------------------------------------------------------------
Vector3 Skeleton::GetJointPosition( int jointIndex ) const
{
	mat44_fl boneToModelSpace = m_boneToModelSpace[ jointIndex ];
	return Vector3( boneToModelSpace.data[ 3 ], boneToModelSpace.data[ 7 ], boneToModelSpace.data[ 11 ] );
}


//-----------------------------------------------------------------------------------------------
Joint* Skeleton::GetJointByIndex( int jointIndex )
{
	if ( ( unsigned int ) jointIndex < m_joints.size() )
	{
		return m_joints[ jointIndex ];
	}

	return nullptr;
}


//-----------------------------------------------------------------------------------------------
int Skeleton::GetIndexOfParent( int jointIndex ) const
{
	return m_jointParentIndicies[ jointIndex ];
}


//-----------------------------------------------------------------------------------------------
void Skeleton::SetJointWorldTransform( int jointIndex, mat44_fl modelTransform )
{
	m_boneToModelSpace[ jointIndex ] = modelTransform;
}


//-----------------------------------------------------------------------------------------------
std::vector< mat44_fl> const Skeleton::GetBoneMatrices()
{
	std::vector< mat44_fl > boneMatrices;

	for ( unsigned int jointIndex = 0; jointIndex < m_joints.size(); ++jointIndex )
	{

		mat44_fl thisBoneMatrix = mat44_fl::identity;
		//MatrixMakeScale( &thisBoneMatrix, g_theGame->m_modelScale );
		thisBoneMatrix = m_boneToModelSpace[ jointIndex ] * thisBoneMatrix * m_modelToBoneSpace[ jointIndex ];
		MatrixTranspose( &thisBoneMatrix );
		boneMatrices.push_back( thisBoneMatrix );
	}

	return boneMatrices;
}


//-----------------------------------------------------------------------------------------------
int Skeleton::GetJointIndexForNodeName( const std::string& nodeName )
{
	for ( unsigned int jointNameIndex = 0; jointNameIndex < m_jointNames.size(); ++jointNameIndex )
	{
		if ( m_jointNames[ jointNameIndex ] == nodeName )
		{
			return jointNameIndex;
		}
	}

	return -1;
}


//-----------------------------------------------------------------------------------------------
void Skeleton::WriteToFile( const std::string& filename )
{
	FileBinaryWriter writer;
	if ( writer.Open( filename ) )
	{
		WriteToStream( writer );
		writer.Close();
	}
}


//-----------------------------------------------------------------------------------------------
void Skeleton::ReadFromFile( const std::string& filename )
{
	FileBinaryReader reader;
	if ( reader.Open( filename ) )
	{
		ReadFromStream( reader );
		reader.Close();
	}
}


//-----------------------------------------------------------------------------------------------
void Skeleton::WriteToStream( IBinaryWriter& writer )
{
	int jointCount = m_jointNames.size();
	writer.Write<int>( jointCount );

	for ( std::string str : m_jointNames )
	{
		writer.WriteString( str );
	}

	for ( int index : m_jointParentIndicies )
	{
		writer.Write<int>( index );
	}

	for ( mat44_fl modelToBoneMat : m_modelToBoneSpace )
	{
		writer.Write<mat44_fl>( modelToBoneMat );
	}

	for ( mat44_fl boneToModelMat : m_boneToModelSpace )
	{
		writer.Write<mat44_fl>( boneToModelMat );
	}
}


//-----------------------------------------------------------------------------------------------
void Skeleton::ReadFromStream( IBinaryReader& reader )
{
	uint32_t joint_count = 0;

	reader.Read( &joint_count );

	std::vector<std::string> jointNames;
	std::vector< int > jointParentIndicies;
	std::vector< mat44_fl > modelToBoneSpace;
	std::vector< mat44_fl > boneToModelSpace;
	jointNames.reserve( joint_count );
	jointParentIndicies.reserve( joint_count );
	modelToBoneSpace.reserve( joint_count );
	boneToModelSpace.reserve( joint_count );

	for ( unsigned int i = 0; i < joint_count; i++ )
	{
		std::string thisJointStr = "";
		reader.ReadString( &thisJointStr );
		jointNames.push_back( thisJointStr );
	}

	for ( unsigned int i = 0; i < joint_count; i++ )
	{
		int thisParentIndex = -1;
		reader.Read<int>( &thisParentIndex );
		jointParentIndicies.push_back( thisParentIndex );
	}

	for ( unsigned int i = 0; i < joint_count; i++ )
	{
		mat44_fl thisModelToBoneMat = mat44_fl::identity;
		reader.Read<mat44_fl>( &thisModelToBoneMat );
		modelToBoneSpace.push_back( thisModelToBoneMat );
	}

	for ( unsigned int i = 0; i < joint_count; i++ )
	{
		mat44_fl thisBoneToModelMat = mat44_fl::identity;
		reader.Read<mat44_fl>( &thisBoneToModelMat );
		boneToModelSpace.push_back( thisBoneToModelMat );
	}

	m_jointNames = jointNames;
	m_jointParentIndicies = jointParentIndicies;
	m_modelToBoneSpace = modelToBoneSpace;
	m_boneToModelSpace = boneToModelSpace;
}