#pragma once
#ifndef __TOOLS_FBX__
#define __TOOLS_FBX__

#include <string>
#include <vector>

#include "Engine/Math/Matrix4x4.hpp"


//-----------------------------------------------------------------------------------------------
class MeshBuilder;
class Skeleton;
class Motion;


//-----------------------------------------------------------------------------------------------
void FbxListScene( std::string filename );


//-----------------------------------------------------------------------------------------------
class SceneImport
{
public:
	~SceneImport()
	{
		meshes.erase( meshes.begin(), meshes.end() );
		meshes.clear();
		skeletons.erase( skeletons.begin(), skeletons.end() );
		skeletons.clear();
	}
	uint32_t GetSkeletonCount() { return skeletons.size(); }
	Skeleton* GetSkeleton( int skelIndex ) { return skeletons[ skelIndex ]; }
public:
	std::vector< MeshBuilder* > meshes;
	std::vector< Skeleton* > skeletons;
	std::vector< Motion* > motions;
};


//-----------------------------------------------------------------------------------------------
SceneImport* FbxLoadSceneFromFile( std::string filename, 
	mat44_fl const &engine_basis,
	bool is_engine_basis_right_handed,
	mat44_fl const &transform = mat44_fl::identity );


#endif