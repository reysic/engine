#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Tools/fbx.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Animation/Motion.hpp"
#include "Engine/Math/Matrix4x4Stack.hpp"
#include "Engine/Math/MathUtils.hpp"


#if defined(TOOLS_BUILD)
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk-md.lib")
	//For TOOLS only


//-----------------------------------------------------------------------------------------------
typedef std::map< int, FbxNode* > IndexToNodeMap;
typedef std::pair< int, FbxNode* > IndexToNodePair;


//-----------------------------------------------------------------------------------------------
static IndexToNodeMap s_nodeMap;


//-----------------------------------------------------------------------------------------------
static Vector4 ToEngineVec4( FbxDouble4 const &v )
{
	return Vector4( ( float ) v.mData[ 0 ],
		( float ) v.mData[ 1 ],
		( float ) v.mData[ 2 ],
		( float ) v.mData[ 3 ] );
}


//-----------------------------------------------------------------------------------------------
static mat44_fl ToEngineMatrix( FbxMatrix const &fbx_mat )
{
	mat44_fl mat;
	MatrixSetRows( &mat,
		ToEngineVec4( fbx_mat.mData[ 0 ] ),
		ToEngineVec4( fbx_mat.mData[ 1 ] ),
		ToEngineVec4( fbx_mat.mData[ 2 ] ),
		ToEngineVec4( fbx_mat.mData[ 3 ] ) );

	return mat;
}


//-----------------------------------------------------------------------------------------------
static mat44_fl GetGeometricTransform( FbxNode *node )
{
	mat44_fl ret = mat44_fl::identity;
	//MatrixMakeIdentity(&ret);

	if ( ( node != nullptr ) && ( node->GetNodeAttribute() != nullptr ) ) {
		FbxVector4 const geo_trans = node->GetGeometricTranslation( FbxNode::eSourcePivot );
		FbxVector4 const geo_rot = node->GetGeometricRotation( FbxNode::eSourcePivot );
		FbxVector4 const geo_scale = node->GetGeometricScaling( FbxNode::eSourcePivot );

		FbxMatrix geo_mat;
		geo_mat.SetTRS( geo_trans, geo_rot, geo_scale );

		ret = ToEngineMatrix( geo_mat );
	}

	return ret;
}


//-----------------------------------------------------------------------------------------------
static char const* GetAttributeType( FbxNodeAttribute::EType type )
{
	switch ( type ) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}


//-----------------------------------------------------------------------------------------------
static void PrintAttribute( FbxNodeAttribute *attribute, int depth )
{
	if ( nullptr == attribute )
	{
		return;
	}

	FbxNodeAttribute::EType type =
		attribute->GetAttributeType();

	char const *type_name = GetAttributeType( type );
	char const *attrib_name = attribute->GetName();

	DebuggerPrintf( "%*s- type='%s', name='%s'", depth, " ", type_name, attrib_name );
	g_theDeveloperConsole->ConsolePrint( Stringf( "%*s- type='%s', name='%s'", depth, " ", type_name, attrib_name ) );

}


//-----------------------------------------------------------------------------------------------
static void PrintNode( FbxNode *node, int depth )
{
	DebuggerPrintf( "%*sNode [%s] \n", depth, " ", node->GetName() );
	g_theDeveloperConsole->ConsolePrint( Stringf( "%*sNode [%s] \n", depth, " ", node->GetName() ) );

	for ( int i = 0; i < node->GetNodeAttributeCount(); ++i )
	{
		PrintAttribute( node->GetNodeAttributeByIndex( i ), depth );
	}

	for ( int32_t i = 0; i < node->GetChildCount(); ++i )
	{
		PrintNode( node->GetChild( i ), depth + 1 );
	}
}


//-----------------------------------------------------------------------------------------------
void FbxListScene( std::string filename )
{
	FbxManager *fbx_manager = FbxManager::Create();
	if ( nullptr == fbx_manager )
	{
		DebuggerPrintf( "Could not create fbx manager." );
		g_theDeveloperConsole->ConsolePrint( "Could not create fbx manager." );
		return;
	}

	FbxIOSettings *io_settings = FbxIOSettings::Create( fbx_manager, IOSROOT );
	fbx_manager->SetIOSettings( io_settings );

	//Create importer
	FbxImporter *importer = FbxImporter::Create( fbx_manager, "" );

	bool result = importer->Initialize( filename.c_str(), -1, fbx_manager->GetIOSettings() );

	if ( result )
	{
		//we have imported the FBX
		FbxScene *scene = FbxScene::Create( fbx_manager, "" );

		bool import_successful = importer->Import( scene );
		if ( import_successful )
		{
			FbxNode *root = scene->GetRootNode();
			PrintNode( root, 0 );
		}
		else
		{
			DebuggerPrintf( "Could not import scene: %s", filename );
			g_theDeveloperConsole->ConsolePrint( Stringf( "Could not import scene: %s", filename ) );
		}

		FBX_SAFE_DESTROY( scene );
	}
	else
	{
		DebuggerPrintf( "Could not initialize FBX importer for file: %s", filename );
		g_theDeveloperConsole->ConsolePrint( Stringf( "Could not initialize FBX importer for file: %s", filename ) );
	}

	FBX_SAFE_DESTROY( importer );
	FBX_SAFE_DESTROY( io_settings );
	FBX_SAFE_DESTROY( fbx_manager );
}


//-----------------------------------------------------------------------------------------------
void FbxList( Command& args )
{

	std::string filename = args.m_argList[ 1 ];
	FbxListScene( filename );
}

#define ASSERT_RETURN(e) if(!e) {DebugBreak();return;}


//-----------------------------------------------------------------------------------------------
static Vector3 ToVec3( FbxVector4 const &fbx_vec4 )
{
	return Vector3( ( float ) fbx_vec4.mData[ 0 ],
		( float ) fbx_vec4.mData[ 1 ],
		( float ) fbx_vec4.mData[ 2 ] );
}


//-----------------------------------------------------------------------------------------------
static bool GetPosition( Vector3 *outPosition,
	mat44_fl const &transform,
	FbxMesh *mesh,
	int polyIndex,
	int vertIndex )
{
	FbxVector4 fbx_pos;
	int control_index = mesh->GetPolygonVertex( polyIndex, vertIndex );

	fbx_pos = mesh->GetControlPointAt( control_index );

	*outPosition = ( Vector4( ToVec3( fbx_pos ), 1.0f ) * transform ).GetXYZ();
	return true;
}


//---------------------------------------------------------------
template <typename ElemType, typename VarType >
static bool GetObjectFromElement( FbxMesh *mesh,
	int poly_idx,
	int vert_idx,
	ElemType *elem,
	VarType *out_var )
{
	if ( nullptr == elem ) {
		return false;
	}

	switch ( elem->GetMappingMode() ) {
	case FbxGeometryElement::eByControlPoint:
	{
		int control_idx = mesh->GetPolygonVertex( poly_idx, vert_idx );
		switch ( elem->GetReferenceMode() ) {
		case FbxGeometryElement::eDirect:
			if ( control_idx < elem->GetDirectArray().GetCount() ) {
				*out_var = elem->GetDirectArray().GetAt( control_idx );
				return true;
			}
			break;

		case FbxGeometryElement::eIndexToDirect:
			if ( control_idx < elem->GetIndexArray().GetCount() ) {
				int index = elem->GetIndexArray().GetAt( control_idx );
				*out_var = elem->GetDirectArray().GetAt( index );
				return true;
			}
			break;

		default:
			break;
		}
	} break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		int direct_vert_idx = ( poly_idx * 3 ) + vert_idx;
		switch ( elem->GetReferenceMode() )
		{
		case FbxGeometryElement::eDirect:
			if ( direct_vert_idx < elem->GetDirectArray().GetCount() ) {
				*out_var = elem->GetDirectArray().GetAt( direct_vert_idx );
				return true;
			}
			break;

		case FbxGeometryElement::eIndexToDirect:
			if ( direct_vert_idx < elem->GetIndexArray().GetCount() ) {
				int index = elem->GetIndexArray().GetAt( direct_vert_idx );
				*out_var = elem->GetDirectArray().GetAt( index );
				return true;
			}
			break;

		default:
			break;
		}
	} break;

	default:
		//ASSERT_MSG( "Undefined mapping mode." );
		break;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
static bool GetUV( Vector2 *out_uv,
	FbxMesh *mesh,
	int polyIndex,
	int vertIndex,
	int uvIndex )
{
	FbxVector2 uv;
	FbxGeometryElementUV *uvs = mesh->GetElementUV( uvIndex );
	if ( GetObjectFromElement( mesh, polyIndex, vertIndex, uvs, &uv ) ) {
		*out_uv = Vector2( ( float ) uv.mData[ 0 ], ( float ) uv.mData[ 1 ] );
		return true;
	}

	return false;
}



//---------------------------------------------------------------
static bool GetNormal( Vector3 *out_normal,
	mat44_fl const &transform,
	FbxMesh *mesh,
	int poly_idx,
	int vert_idx )
{
	FbxVector4 normal;
	FbxGeometryElementNormal *uvs = mesh->GetElementNormal( 0 );
	if ( GetObjectFromElement( mesh, poly_idx, vert_idx, uvs, &normal ) ) 
	{
		Vector3 n = ToVec3( normal );
		//*out_normal = ( transform * Vector4( n, 0.0f ) ).GetXYZ();
		*out_normal = ( Vector4( n, 0.0f ) * transform ).GetXYZ();
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
static bool GetColor( Vector4 *out_color,
	mat44_fl const &transform,
	FbxMesh *mesh,
	int poly_idx,
	int vert_idx )
{
	UNUSED( transform );

	FbxColor color;
	FbxGeometryElementVertexColor *colors = mesh->GetElementVertexColor( 0 );
	if ( GetObjectFromElement( mesh, poly_idx, vert_idx, colors, &color ) ) 
	{
		Vector4 vec4color = Vector4( ( float ) color.mRed, ( float ) color.mGreen, ( float ) color.mBlue, ( float ) color.mAlpha );
		*out_color = vec4color;
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
static void ImportVertex( MeshBuilder* meshbuilder, const mat44_fl& transform, FbxMesh* mesh, int polyIndex, int vertIndex,
	std::vector< SkinWeight > &skinWeights )
{
	// Set vertex normal
	Vector3 normal;
	if ( GetNormal( &normal, transform, mesh, polyIndex, vertIndex ) ) 
	{
		meshbuilder->SetNormal( normal );

		// #TODO: Replace with MikkT
		Vector3 bitangent = Vector3( 0.f, 0.f, 1.f );
		//if ( normal == bitangent )
		if ( IsCollinearVector3( normal, bitangent) )
		{
			bitangent = Vector3( 0.f, 1.f, 0.f );
		}
		Vector3 tangent = CrossProductVector3( bitangent, normal );
		bitangent = CrossProductVector3( normal, tangent );

		meshbuilder->SetTangent( tangent );
		meshbuilder->SetBitangent( bitangent );
	}

	// Set vertex color
	Vector4 color;
	if ( GetColor( &color, transform, mesh, polyIndex, vertIndex ) ) 
	{
		unsigned char red = ( unsigned char ) RangeMap( color.x, 0.0f, 1.0f, 0.0f, 255.0f );
		unsigned char green = ( unsigned char ) RangeMap( color.y, 0.0f, 1.0f, 0.0f, 255.0f );
		unsigned char blue = ( unsigned char ) RangeMap( color.z, 0.0f, 1.0f, 0.0f, 255.0f );
		unsigned char alpha = ( unsigned char ) RangeMap( color.w, 0.0f, 1.0f, 0.0f, 255.0f );
		meshbuilder->SetColor( Rgba( red, green, blue, alpha ) );
	}

	// Set vertex UV
	Vector2 uv;
	if ( GetUV( &uv, mesh, polyIndex, vertIndex, 0 ) ) 
	{
		meshbuilder->SetUV0( uv );
	}


	// Set bone indices and weights
	unsigned int control_idx = mesh->GetPolygonVertex( polyIndex, vertIndex );
	if ( control_idx < skinWeights.size() ) 
	{
		meshbuilder->SetBoneWeights( skinWeights[ control_idx ].indices, skinWeights[ control_idx ].weights );
		meshbuilder->RenormalizeSkinWeights();
	}
	else 
	{
		meshbuilder ->ClearBoneWeights();
	}


	// Set vertex position
	Vector3 position;
	if ( GetPosition( &position, transform, mesh, polyIndex, vertIndex ) )
	{
		meshbuilder->AddVertex( position );
	}
}


//-----------------------------------------------------------------------------------------------
// Check if a mesh contains skin weights
static bool HasSkinWeights( FbxMesh const *mesh )
{
	int deformer_count = mesh->GetDeformerCount( FbxDeformer::eSkin );
	return ( deformer_count > 0 );
};


//-----------------------------------------------------------------------------------------------
static void AddHighestWeight( SkinWeight* skinWeight, int jointIndex, float weight )
{
	// EXERCISE TO THE READER
	// Go through skinWeight, find lowest and kick out of list and replace it
	// Replace the lowest value with what was sent in, IF you are higher
	// than the lower value. Don't add yourself in if you're already lower than
	// everybody else

	float* lowestWeight = &skinWeight->weights.x;
	unsigned int* lowestIndex = &skinWeight->indices.x;

	if ( skinWeight->weights.y < *lowestWeight )
	{
		lowestWeight = &skinWeight->weights.y;
		lowestIndex = &skinWeight->indices.y;
	}

	if ( skinWeight->weights.z < *lowestWeight )
	{
		lowestWeight = &skinWeight->weights.z;
		lowestIndex = &skinWeight->indices.z;
	}

	if ( skinWeight->weights.w < *lowestWeight )
	{
		lowestWeight = &skinWeight->weights.w;
		lowestIndex = &skinWeight->indices.w;
	}

	if ( weight > *lowestWeight )
	{
		*lowestWeight = weight;
		*lowestIndex = jointIndex;
	}
};


//-----------------------------------------------------------------------------------------------
static void GetSkinWeights( SceneImport *import, std::vector< SkinWeight > &skinWeights,
	FbxMesh const *mesh )
{
	UNUSED( import ); // #TODO: Really?

	size_t control_point_count = mesh->GetControlPointsCount();
	UNUSED( control_point_count ); // #TODO: Really?

	for ( unsigned int i = 0; i < skinWeights.size(); ++i )
	{
		skinWeights[ i ].indices = UIntVector4( 0, 0, 0, 0 );
		skinWeights[ i ].weights = Vector4( 0.0f, 0.0f, 0.0f, 0.0f );
	}

	int deformerCount = mesh->GetDeformerCount( FbxDeformer::eSkin );
	ASSERT_OR_DIE( deformerCount == 1, "deformerCount != 1" );
	for ( int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex )
	{
		FbxSkin *skin = ( FbxSkin* ) mesh->GetDeformer( deformerIndex, FbxDeformer::eSkin );

		if ( nullptr == skin )
		{
			continue;
		}

		// Clusters are a link between this skin object, bones,
		// and the verts that bone affects.
		int clusterCount = skin->GetClusterCount();
		for ( int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex )
		{
			FbxCluster *cluster = skin->GetCluster( clusterIndex );
			FbxNode const *linkNode = cluster->GetLink();

			// Not associated with a bone - ignore it, we
			// don't care about it.
			if ( nullptr == linkNode )
			{
				continue;
			}

			IndexToNodeMap::iterator iter = s_nodeMap.begin();
			int jointIndex = -1;

			for ( iter = s_nodeMap.begin(); iter != s_nodeMap.end(); ++iter )
			{
				if ( linkNode == iter->second )
				{
					jointIndex = iter->first;
				}
			}

			if ( jointIndex == -1 ) {
				continue;
			}

			int *controlPointIndicies = cluster->GetControlPointIndices();
			int indexCount = cluster->GetControlPointIndicesCount();
			double* weights = cluster->GetControlPointWeights();

			for ( int i = 0; i < indexCount; ++i )
			{
				int controlIndex = controlPointIndicies[ i ];
				double weight = weights[ i ];

				SkinWeight* skinWeight = &skinWeights[ controlIndex ];
				AddHighestWeight( skinWeight, jointIndex, ( float ) weight );
			}
		}
	}

// 	for ( unsigned int i = 0; i < skin_weights.size(); ++i )
// 	{
// 		// Renormalize all the skin weights
// 		// If not skin weights were added, then go back to the defaults
// 		// If you have weights of (0, 0, 0, 0) change to (1, 0, 0, 0)
// 		// If skin_weights were never added - make sure
// 		// you set its weights to (1, 0, 0, 0)
// 	}
};


//-----------------------------------------------------------------------------------------------
// ImportSkeletons() must happen before meshes
static void ImportMesh( SceneImport *import,
	FbxMesh *mesh,
	Matrix4x4Stack &mat_stack )
{
	MeshBuilder *mb = new MeshBuilder();

	ASSERT_RETURN( mesh->IsTriangleMesh() );

	//This somehow breaks the model
	mat44_fl geo_transform = GetGeometricTransform( mesh->GetNode() );
	mat_stack.Push( geo_transform );

	mat44_fl transform = mat_stack.GetTop();

	//////////////////////////////////// SKINNING /////////////////////////////
	int controlPointCount = mesh->GetControlPointsCount();

	std::vector< SkinWeight > skinWeights;
	skinWeights.resize( controlPointCount );
	if ( HasSkinWeights( mesh ) )
	{
		GetSkinWeights( import, skinWeights, mesh );
	}
	else
	{
		FbxNode *node = mesh->GetNode();

		UNUSED( node );
		// walk tree up till node associated with joint

		// Find first parent node that has a joint associated with it
		// all vertices (fully weighted)
		// all skin_weights = indicies{ joint_idx, 0, 0, 0 }  weights{ 1.0f, 0.0f, 0.0f, 0.0f };
	}
	//////////////////////////////////// SKINNING /////////////////////////////

	int poly_count = mesh->GetPolygonCount();
	for ( int poly_idx = 0; poly_idx < poly_count; ++poly_idx ) 
	{
		int vert_count = mesh->GetPolygonSize( poly_idx );
		//ASSERT_OR_DIE(vert_count == 3);
		for ( int vert_idx = 0; vert_idx < vert_count; ++vert_idx ) 
		{
			// Pass skin_weights into this now too
			ImportVertex( mb, transform, mesh, poly_idx, vert_idx, skinWeights );
		}
	}

	mb->End();
	mat_stack.Pop();
	//Import the mesh

	import->meshes.push_back( mb );
}


//-----------------------------------------------------------------------------------------------
static void ImportNodeAttribute( SceneImport *import,
	FbxNodeAttribute *attrib,
	Matrix4x4Stack &mat_stack )
{
	if ( attrib == nullptr )
	{
		return;
	}

	switch ( attrib->GetAttributeType() )
	{
		case FbxNodeAttribute::eMesh:
		{
			ImportMesh( import, ( FbxMesh* ) attrib, mat_stack );
			break;
		}
		default:
		{
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
static mat44_fl GetNodeTransform( FbxNode *node )
{
	FbxMatrix fbx_local_matrix = node->EvaluateLocalTransform();
	return ToEngineMatrix( fbx_local_matrix );
}


//-----------------------------------------------------------------------------------------------
static void ImportSceneNode( SceneImport *import,
	FbxNode *node,
	Matrix4x4Stack &mat_stack )
{
	if ( nullptr == node )
	{
		return;
	}

	mat44_fl node_local_transform = GetNodeTransform( node );
	mat_stack.Push( node_local_transform );

	//Load Mesh
	int attrib_count = node->GetNodeAttributeCount();
	for ( int attrib_index = 0; attrib_index < attrib_count; ++attrib_index )
	{
		ImportNodeAttribute( import, node->GetNodeAttributeByIndex( attrib_index ), mat_stack );
	}

	//Import Children
	int childCount = node->GetChildCount();
	for ( int childIndex = 0; childIndex < childCount; ++childIndex )
	{
		ImportSceneNode( import, node->GetChild( childIndex ), mat_stack );
	}

	mat_stack.Pop();
}


//-----------------------------------------------------------------------------------------------
static void TriangulateScene( FbxScene *scene )
{
	FbxGeometryConverter converter( scene->GetFbxManager() );
	converter.Triangulate( scene, true/*replaces nodes with a traigulated mesh*/ );
}


//-----------------------------------------------------------------------------------------------
static Skeleton* ImportJoint( SceneImport *import,
	Matrix4x4Stack &mat_stack,
	Skeleton *cur_skeleton,
	int parent_joint_idx,
	FbxSkeleton *fbx_skeleton )
{
	Skeleton *ret_skeleton = nullptr;

	if ( fbx_skeleton->IsSkeletonRoot() ) {
		// NEW SKELETON!
		ret_skeleton = new Skeleton();
		import->skeletons.push_back( ret_skeleton );
	}
	else {
		ret_skeleton = cur_skeleton;
		ASSERT_OR_DIE( ret_skeleton != nullptr, nullptr );
	}

	// Same as we did for meshes.  Usually this is the identity, but some 
	// modeling programs use it.
	mat44_fl geo_transform = GetGeometricTransform( fbx_skeleton->GetNode() );
	mat_stack.Push( geo_transform );

	mat44_fl model_matrix = mat_stack.GetTop();
	ret_skeleton->AddJoint( fbx_skeleton->GetNode()->GetName(),
		parent_joint_idx,
		model_matrix );

	mat_stack.Pop();
	return ret_skeleton;
}


//-----------------------------------------------------------------------------------------------
static void ImportSkeletons( SceneImport *import,
	FbxNode *node,
	Matrix4x4Stack &matrix_stack,
	Skeleton *cur_skeleton, 	// Skeleton I'm adding to
	int parent_joint_idx,		// Last Joint Added
	IndexToNodeMap &map )
{
	// Most of this will look familiar
	if ( nullptr == node ) {
		return;
	}

	mat44_fl mat = GetNodeTransform( node );
	matrix_stack.Push( mat );

	int attrib_count = node->GetNodeAttributeCount();
	for ( int attrib_idx = 0; attrib_idx < attrib_count; ++attrib_idx ) {
		FbxNodeAttribute *attrib = node->GetNodeAttributeByIndex( attrib_idx );
		if ( ( attrib != nullptr )
			&& ( attrib->GetAttributeType() == FbxNodeAttribute::eSkeleton ) ) {

			Skeleton *new_skeleton = ImportJoint( import,
				matrix_stack,
				cur_skeleton,
				parent_joint_idx,
				( FbxSkeleton* ) attrib );
			IndexToNodePair pair( new_skeleton->m_joints.size() - 1, attrib->GetNode() );
			map.insert( pair );
			// new_skeleton will either be the same skeleton
			// passed in, no skeleton, or a new skeleton
			// in the case of a new skeleton or same skeleton
			// it will become the skeleton we pass to all our
			// children.
			if ( nullptr != new_skeleton ) {
				cur_skeleton = new_skeleton;
				parent_joint_idx = cur_skeleton->GetLastAddedIndex();
			}
		}
	}

	// Go down the tree!
	int child_count = node->GetChildCount();
	for ( int child_idx = 0; child_idx < child_count; ++child_idx ) {
		ImportSkeletons( import,
			node->GetChild( child_idx ),
			matrix_stack,
			cur_skeleton,
			parent_joint_idx,
			map );
		if ( cur_skeleton != nullptr )
		{
			IndexToNodePair childPair( cur_skeleton->m_joints.size() - 1, node->GetChild( child_idx ) );
			map.insert( childPair );
		}
	}

	matrix_stack.Pop();
}


//-----------------------------------------------------------------------------------------------
static mat44_fl GetNodeWorldTransformAtTime( FbxNode *node, FbxTime time, mat44_fl const &import_transform )
{
	if ( node == nullptr ) {
		return mat44_fl::identity;
	}

	FbxMatrix fbx_mat = node->EvaluateGlobalTransform( time );
	mat44_fl engine_matrix = ToEngineMatrix( fbx_mat );
	return import_transform * engine_matrix;
}


//-----------------------------------------------------------------------------------------------
void ImportMotions( SceneImport *import,
	mat44_fl &import_transform,
	IndexToNodeMap &map,
	FbxScene *scene,
	float framerate )
{
	// Get number of animations
	int animation_count = scene->GetSrcObjectCount<FbxAnimStack>();
	if ( 0 == animation_count ) {
		return;
	}

	if ( 0 == import->GetSkeletonCount() ) {
		return;
	}

	// Timing information for animation in this scene
	FbxGlobalSettings &settings = scene->GetGlobalSettings();
	FbxTime::EMode time_mode = settings.GetTimeMode();
	double scene_framerate;
	if ( time_mode == FbxTime::eCustom ) {
		scene_framerate = settings.GetCustomFrameRate();
	}
	else {
		scene_framerate = FbxTime::GetFrameRate( time_mode );
	}

	// Only supporting one skeleton for now - update when needed.
	uint32_t skeleton_count = import->GetSkeletonCount();
	Skeleton *skeleton = import->GetSkeleton( 0 );
	ASSERT_OR_DIE( skeleton_count == 1, "skeleton_count != 1" );

	// Time between frames
	FbxTime advance;
	advance.SetSecondDouble( ( double ) ( 1.0f / framerate ) );

	for ( int anim_index = 0; anim_index < animation_count; ++anim_index ) {
		// Import Motions
		FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>();
		if ( nullptr == anim ) {
			continue;
		}

		// Get duration of this specific animation.
		//FbxTime start_time = anim->LocalStart;
		FbxTime start_time = ( FbxLongLong ) 0.0f; // Using this to avoid hitching in loop
		FbxTime end_time = anim->LocalStop;
		FbxTime duration = end_time - start_time;

		scene->SetCurrentAnimationStack( anim );

		char const *motion_name = anim->GetName();
		float time_span = ( float ) duration.GetSecondDouble();

		Motion *motion = new Motion( motion_name, time_span, framerate, skeleton );

		int joint_count = skeleton->GetJointCount();
		for ( int joint_idx = 0; joint_idx < joint_count; ++joint_idx ) {
			FbxNode *node = map.at( joint_idx ); // map<FbxNode*, Joint*> &map, 

											  // Extracting World Position
											  // Local, you would need to grab parent as well
			mat44_fl *bone_keyframes = motion->GetJointKeyFrames( joint_idx );

			FbxTime eval_time = FbxTime( 0 );
			for ( uint32_t frame_idx = 0; frame_idx < ( uint32_t ) motion->m_frameCount; ++frame_idx ) {
				mat44_fl *bone_keyframe = bone_keyframes + frame_idx;
				mat44_fl bone_transform = GetNodeWorldTransformAtTime( node, eval_time, import_transform );
				*bone_keyframe = bone_transform;

				eval_time += advance;
			}
		}

		import->motions.push_back( motion );
	}
}


//-----------------------------------------------------------------------------------------------
static void ImportScene( SceneImport *import, FbxScene *scene, Matrix4x4Stack &mat_stack )
{
	TriangulateScene( scene );

	IndexToNodeMap index_to_node;
	FbxNode *node = scene->GetRootNode();
	ImportSkeletons( import, node, mat_stack, nullptr, -1, index_to_node );
	s_nodeMap = index_to_node;
	ImportSceneNode( import, node, mat_stack );
	// Top contains just our change of basis and scale matrices at this point
	ImportMotions( import, mat_stack.GetTop(), index_to_node, scene, 30.0f );
}


//-----------------------------------------------------------------------------------------------
static mat44_fl GetSceneBasis( FbxScene *scene)
{
	fbxsdk::FbxAxisSystem axis_system = scene->GetGlobalSettings().GetAxisSystem();

	FbxAMatrix mat;
	axis_system.GetMatrix( mat );

	return ToEngineMatrix( mat );
}


//-----------------------------------------------------------------------------------------------
SceneImport* FbxLoadSceneFromFile( std::string filename,
	mat44_fl const &engine_basis, 
	bool is_engine_basis_right_handed,
	mat44_fl const &transform )
{
	FbxManager *fbx_manager = FbxManager::Create();
	if ( nullptr == fbx_manager )
	{
		DebuggerPrintf( "Could not create FbxManager." );
		g_theDeveloperConsole->ConsolePrint( "Could not create FbxManager." );
		return nullptr;
	}

	FbxIOSettings *io_settings = FbxIOSettings::Create( fbx_manager, IOSROOT );
	fbx_manager->SetIOSettings( io_settings );

	//Create importer
	FbxImporter *importer = FbxImporter::Create( fbx_manager, "" );

	bool result = importer->Initialize( filename.c_str(), -1, fbx_manager->GetIOSettings() );

	if ( result )
	{
		//we have imported the FBX
		FbxScene *scene = FbxScene::Create( fbx_manager, "" );

		bool import_successful = importer->Import( scene );
		if ( import_successful )
		{
			FbxNode *root = scene->GetRootNode();

			UNUSED( root );

			SceneImport *import = new SceneImport();
			Matrix4x4Stack mat_stack;

			mat_stack.Push( transform );
			mat_stack.Push( engine_basis );

			// Set up initial transform
			mat44_fl scene_basis = GetSceneBasis( scene );
			MatrixTranspose( &scene_basis );

			if ( !is_engine_basis_right_handed )
			{
				Vector3 forward = MatrixGetForward( &scene_basis );
				MatrixSetForward( &scene_basis, -forward );
			}

			mat_stack.Push( scene_basis );

			ImportScene( import, scene, mat_stack );

			// Cleanup
			FBX_SAFE_DESTROY( importer );
			FBX_SAFE_DESTROY( io_settings );
			FBX_SAFE_DESTROY( fbx_manager );
			//FBX_SAFE_DESTROY( scene );

			return import;
		}
		else
		{
			DebuggerPrintf( "Could not import scene from FBX file: %s", filename );
			g_theDeveloperConsole->ConsolePrint( Stringf( "Could not import scene from FBX file: %s", filename ) );

			// Cleanup
			FBX_SAFE_DESTROY( importer );
			FBX_SAFE_DESTROY( io_settings );
			FBX_SAFE_DESTROY( fbx_manager );
			//FBX_SAFE_DESTROY( scene );

			return nullptr;
		}
	}
	else
	{
		DebuggerPrintf( "Could not initialize FBX importer for file: %s", filename );
		g_theDeveloperConsole->ConsolePrint( Stringf( "Could not initialize FBX importer for file: %s", filename ) );

		// Cleanup
		FBX_SAFE_DESTROY( importer );
		FBX_SAFE_DESTROY( io_settings );
		FBX_SAFE_DESTROY( fbx_manager );

		return nullptr;
	}
}
#else //!defined (TOOLS_BUILD)
void FbxListScene( std::string filename ) {}

SceneImport* FbxLoadSceneFromFile( std::string filename ) { return nullptr; }

#endif