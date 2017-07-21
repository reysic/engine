// #include "Engine/Renderer/Meshes/MeshRenderer.hpp"
// #include "Engine/Core/ErrorWarningAssert.hpp"
// #include "Engine/Math/Matrix4x4.hpp"
// #include "Engine/Renderer/Material.hpp"
// #include "Engine/Renderer/Meshes/Mesh.hpp"
// #include "Engine/Renderer/Renderer.hpp"
// #include "Engine/Renderer/Shaders/Uniform.hpp"
// #include "Engine/Renderer/Shaders/Attribute.hpp"
// #include "Engine/Renderer/Texture.hpp"
// #include "Engine/Renderer/Lights/Light.hpp"
// 
// 
// //-------------------------------------------------------------------------------------------------
// MeshRenderer::MeshRenderer( MeshShape const &meshShape, Transform const &transform /*= Transform( )*/, RenderState const &renderState /*= RenderState::BASIC_3D*/ )
// 	: m_transform( transform )
// 	, m_renderState( renderState )
// 	, m_vaoID( NULL )
// 	, m_mesh( nullptr )
// 	, m_material( nullptr )
// {
// 	meshShape;
// 	//m_mesh = Mesh::GetMeshShape(meshShape);
// 	//m_material = Material::GetDefault();
// 
// 	//Create VAO
// 	glGenVertexArrays( 1, &m_vaoID );
// 	ASSERT_OR_DIE( m_vaoID != NULL, "VAO didn't generate." );
// 
// 	g_theRenderer->BindMeshToVAO( m_vaoID, m_mesh, m_material );
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// MeshRenderer::MeshRenderer( Mesh const *mesh, Transform const &transform /*= Transform( )*/, RenderState const &renderState /*= RenderState::BASIC_3D */ )
// 	: m_transform( transform )
// 	, m_renderState( renderState )
// 	, m_vaoID( NULL )
// 	, m_mesh( mesh )
// 	, m_material( nullptr )
// {
// 	//m_material = Material::GetDefault();
// 
// 	//Create VAO
// 	glGenVertexArrays( 1, &m_vaoID );
// 	ASSERT_OR_DIE( m_vaoID != NULL, "VAO didn't generate." );
// 
// 	g_theRenderer->BindMeshToVAO( m_vaoID, m_mesh, m_material );
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// MeshRenderer::MeshRenderer( Mesh const *mesh, Material *material, Transform const &transform /*= Transform() */, RenderState const &renderState /*= RenderState::BASIC_3D*/ )
// 	: m_transform( transform )
// 	, m_renderState( renderState )
// 	, m_vaoID( NULL )
// 	, m_mesh( mesh )
// 	, m_material( material )
// {
// 	//Create VAO
// 	glGenVertexArrays( 1, &m_vaoID );
// 	ASSERT_OR_DIE( m_vaoID != NULL, "VAO didn't generate." );
// 
// 	g_theRenderer->BindMeshToVAO( m_vaoID, m_mesh, m_material );
// 
// 	//Clear Matrices
// 	SetUniform( "gModel", GetModelMatrix() );
// 	SetUniform( "gView", mat44_fl::identity );
// 	SetUniform( "gProj", mat44_fl::identity );
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// MeshRenderer::~MeshRenderer()
// {
// 	glDeleteVertexArrays( 1, &m_vaoID );
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::Update()
// {
// 	g_theRenderer->SetMatrixUniforms( this );
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// RenderState MeshRenderer::GetRenderState() const
// {
// 	return m_renderState;
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetLineWidth( float lineWidth )
// {
// 	m_renderState.m_lineWidth = lineWidth;
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// // #TODO: Could pre-calculate for each time the model is transformed
// mat44_fl MeshRenderer::GetModelMatrix() const
// {
// 	return m_transform.GetModelMatrix();
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// unsigned int MeshRenderer::GetGPUProgramID() const
// {
// 	return m_material->GetGPUProgramID();
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// unsigned int MeshRenderer::GetSamplerID() const
// {
// 	return m_material->GetSamplerID();
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// unsigned int MeshRenderer::GetVAOID() const
// {
// 	return m_vaoID;
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// unsigned int MeshRenderer::GetIBOID() const
// {
// 	return m_mesh->GetIBOID();
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// unsigned int MeshRenderer::GetDrawMode() const
// {
// 	return m_mesh->GetDrawMode();
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// int MeshRenderer::GetVertCount() const
// {
// 	return m_mesh->GetIBOorVertCount();
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// int MeshRenderer::GetIndexBufferCount() const
// {
// 	return m_mesh->GetIBOorVertCount();
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetPosition( Vector3 const &pos )
// {
// 	m_transform.SetPosition( pos );
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetPosition( float xPos, float yPos, float zPos )
// {
// 	m_transform.SetPosition( Vector3( xPos, yPos, zPos ) );
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetTransform( Transform const &transform )
// {
// 	m_transform = transform;
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// std::map<std::string, Uniform*>& MeshRenderer::GetUniformList()
// {
// 	return m_uniforms;
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// std::map<std::string, Uniform*>& MeshRenderer::GetMaterialUniformList()
// {
// 	return m_material->GetUniformList();
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// std::map<std::string, Attribute*> MeshRenderer::GetMaterialAttributeList() const
// {
// 	return m_material->GetAttributeList();
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// std::vector<DrawInstruction> const & MeshRenderer::GetDrawInstructions() const
// {
// 	return m_mesh->GetDrawInstructions();
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, unsigned int uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	//First time being set
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			( ( Uniform* ) uniform )->m_size,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			new int( uniformValue ) //Set new value
// 		);
// 	}
// 	//Updating value
// 	else if ( memberUniform != nullptr )
// 	{
// 		*( ( int* ) memberUniform->m_data ) = uniformValue;
// 	}
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, int uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	//First time being set
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			( ( Uniform* ) uniform )->m_size,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			new int( uniformValue ) //Set new value
// 		);
// 	}
// 	//Updating value
// 	else if ( memberUniform != nullptr )
// 	{
// 		*( ( int* ) memberUniform->m_data ) = uniformValue;
// 	}
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, float uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			( ( Uniform* ) uniform )->m_size,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			new float( uniformValue ) //Set new value
// 		);
// 	}
// 	else if ( memberUniform != nullptr )
// 	{
// 		*( ( float* ) memberUniform ) = uniformValue;
// 	}
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, Vector2 const &uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			( ( Uniform* ) uniform )->m_size,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			new Vector2( uniformValue ) //Set new value
// 		);
// 	}
// 	else if ( memberUniform != nullptr )
// 	{
// 		*( ( Vector2* ) memberUniform->m_data ) = uniformValue;
// 	}
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, Vector3 const &uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			( ( Uniform* ) uniform )->m_size,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			new Vector3( uniformValue ) //Set new value
// 		);
// 	}
// 	else if ( memberUniform != nullptr )
// 	{
// 		*( ( Vector3* ) memberUniform->m_data ) = uniformValue;
// 	}
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, Vector4 const &uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			( ( Uniform* ) uniform )->m_size,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			new Vector4( uniformValue ) //Set new value
// 		);
// 	}
// 	else if ( memberUniform != nullptr )
// 	{
// 		*( ( Vector4* ) memberUniform->m_data ) = uniformValue;
// 	}
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, mat44_fl const &uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			( ( Uniform* ) uniform )->m_size,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			new mat44_fl( uniformValue ) //Set new value
// 		);
// 	}
// 	else if ( memberUniform != nullptr )
// 	{
// 		*( ( mat44_fl* ) memberUniform->m_data ) = uniformValue;
// 	}
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, Rgba const &uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			( ( Uniform* ) uniform )->m_size,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			new Vector4( uniformValue.FloatRepresentation() ) //Set new value
// 		);
// 	}
// 	else if ( memberUniform != nullptr )
// 	{
// 		*( ( Vector4* ) memberUniform->m_data ) = uniformValue.FloatRepresentation();
// 	}
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, std::string const &uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	Texture const *generatedTexture = Texture::CreateOrGetTexture( uniformValue );//Texture::CreateOrLoadTexture(uniformValue);
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			( ( Uniform* ) uniform )->m_size,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			new unsigned int( generatedTexture->m_openglTextureID ) //Set new value
// 		);
// 	}
// 	else if ( memberUniform != nullptr )
// 	{
// 		*( ( unsigned int* ) memberUniform->m_data ) = generatedTexture->m_openglTextureID;
// 	}
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, int *uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	//Doesn't exist yet, lets make space for it and assign it to the data
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( int ) * uniformSize;
// 		int *data = ( int* ) malloc( dataSize );
// 		memcpy( data, uniformValue, dataSize );
// 
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			uniformSize,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			data //Setting new value
// 		);
// 	}
// 
// 	//update the data
// 	else if ( memberUniform != nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( int ) * uniformSize;
// 		memcpy( memberUniform->m_data, uniformValue, dataSize );
// 	}
// 
// 	//That's not real
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, float *uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	//Doesn't exist yet, lets make space for it and assign it to the data
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( float ) * uniformSize;
// 		float *data = ( float* ) malloc( dataSize );
// 		memcpy( data, uniformValue, dataSize );
// 
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			uniformSize,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			data
// 		);
// 	}
// 
// 	//update the data
// 	else if ( memberUniform != nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( float ) * uniformSize;
// 		memcpy( memberUniform->m_data, uniformValue, dataSize );
// 	}
// 
// 	//That's not real
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, Vector2 *uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	//Doesn't exist yet, lets make space for it and assign it to the data
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( Vector2 ) * uniformSize;
// 		Vector2 *data = ( Vector2* ) malloc( dataSize );
// 		memcpy( data, uniformValue, dataSize );
// 
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			uniformSize,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			data
// 		);
// 	}
// 
// 	//update the data
// 	else if ( memberUniform != nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( Vector2 ) * uniformSize;
// 		memcpy( memberUniform->m_data, uniformValue, dataSize );
// 	}
// 
// 	//That's not real
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, Vector3 *uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	//Doesn't exist yet, lets make space for it and assign it to the data
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( Vector3 ) * uniformSize;
// 		Vector3 *data = ( Vector3* ) malloc( dataSize );
// 		memcpy( data, uniformValue, dataSize );
// 
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			uniformSize,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			data
// 		);
// 	}
// 
// 	//update the data
// 	else if ( memberUniform != nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( Vector3 ) * uniformSize;
// 		memcpy( memberUniform->m_data, uniformValue, dataSize );
// 	}
// 
// 	//That's not real
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, Vector4 *uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	//Doesn't exist yet, lets make space for it and assign it to the data
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( Vector4 ) * uniformSize;
// 		Vector4 *data = ( Vector4* ) malloc( dataSize );
// 		memcpy( data, uniformValue, dataSize );
// 
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			uniformSize,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			data
// 		);
// 	}
// 
// 	//update the data
// 	else if ( memberUniform != nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( Vector4 ) * uniformSize;
// 		memcpy( memberUniform->m_data, uniformValue, dataSize );
// 	}
// 
// 	//That's not real
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-------------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::vector< Light > const &uniformLights, int lightCount )
// {
// 	SetUniform( "gLightCount", lightCount );
// 	Vector4 lightColor[ 16 ];
// 	Vector3 lightPosition[ 16 ];
// 	Vector3 lightDirection[ 16 ];
// 	float lightIsDirectional[ 16 ];
// 	float lightDistanceMin[ 16 ];
// 	float lightDistanceMax[ 16 ];
// 	float lightStrengthAtMin[ 16 ];
// 	float lightStrengthAtMax[ 16 ];
// 	float lightInnerAngle[ 16 ];
// 	float lightOuterAngle[ 16 ];
// 	float lightStrengthInside[ 16 ];
// 	float lightStrengthOutside[ 16 ];
// 	for ( unsigned int lightIndex = 0; lightIndex < uniformLights.size(); ++lightIndex )
// 	{
// 		//lightColor[lightIndex] = uniformLights[lightIndex].m_lightColor.FloatRepresentation();
// 		lightColor[ lightIndex ] = uniformLights[ lightIndex ].m_color;
// 		lightPosition[ lightIndex ] = uniformLights[ lightIndex ].m_position;
// 		lightDirection[ lightIndex ] = uniformLights[ lightIndex ].m_direction;
// 		//lightIsDirectional[lightIndex] = uniformLights[lightIndex].m_isLightDirectional;
// 		lightIsDirectional[ lightIndex ] = uniformLights[ lightIndex ].m_directionalInterp;
// 		lightDistanceMin[ lightIndex ] = uniformLights[ lightIndex ].m_minLightDistance;
// 		lightDistanceMax[ lightIndex ] = uniformLights[ lightIndex ].m_maxLightDistance;
// 		lightStrengthAtMin[ lightIndex ] = uniformLights[ lightIndex ].m_powerAtMin;
// 		lightStrengthAtMax[ lightIndex ] = uniformLights[ lightIndex ].m_powerAtMax;
// 		lightInnerAngle[ lightIndex ] = uniformLights[ lightIndex ].m_thetaInner;
// 		lightOuterAngle[ lightIndex ] = uniformLights[ lightIndex ].m_thetaOuter;
// 		lightStrengthInside[ lightIndex ] = uniformLights[ lightIndex ].m_thetaInnerPower;
// 		lightStrengthOutside[ lightIndex ] = uniformLights[ lightIndex ].m_thetaOuterPower;
// 	}
// 
// 	SetUniform( "gLightPosition", lightPosition );
// 	SetUniform( "gLightDirection", lightDirection );
// 	SetUniform( "gLightColor", lightColor );
// 	SetUniform( "gLightDirectionFactor", lightIsDirectional );
// 
// 	SetUniform( "gNearDistance", lightDistanceMin );
// 	SetUniform( "gFarDistance", lightDistanceMax );
// 	SetUniform( "gNearFactor", lightStrengthAtMin );
// 	SetUniform( "gFarFactor", lightStrengthAtMax );
// 
// 	SetUniform( "gInnerAngle", lightInnerAngle );
// 	SetUniform( "gOuterAngle", lightOuterAngle );
// 	SetUniform( "gInnerFactor", lightStrengthInside );
// 	SetUniform( "gOuterFactor", lightStrengthOutside );
// }
// 
// 
// //-----------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName, const mat44_fl *uniformValue )
// {
// 	std::map<std::string, Uniform*>& uniformList = m_material->GetUniformList();
// 	Uniform* uniform = uniformList[ uniformName ];
// 	Uniform* memberUniform = m_uniforms[ uniformName ];
// 
// 	//Doesn't exist yet, lets make space for it and assign it to the data
// 	if ( uniform != nullptr && memberUniform == nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( mat44_fl ) * uniformSize;
// 		mat44_fl *data = ( mat44_fl* ) malloc( dataSize );
// 		memcpy( data, uniformValue, dataSize );
// 
// 		m_uniforms[ uniformName ] = new Uniform
// 		(
// 			( ( Uniform* ) uniform )->m_bindPoint,
// 			( ( Uniform* ) uniform )->m_length,
// 			uniformSize,
// 			( ( Uniform* ) uniform )->m_type,
// 			( ( Uniform* ) uniform )->m_name,
// 			data
// 		);
// 	}
// 
// 	//update the data
// 	else if ( memberUniform != nullptr )
// 	{
// 		int uniformSize = ( ( Uniform* ) uniform )->m_size;
// 		size_t dataSize = sizeof( mat44_fl ) * uniformSize;
// 		memcpy( memberUniform->m_data, uniformValue, dataSize );
// 	}
// 
// 	//That's not real
// 	else
// 	{
// 		ASSERT_OR_DIE( false, "Uniform doesn't exist on Material" );
// 	}
// }
// 
// 
// //-----------------------------------------------------------------------------------------------
// void MeshRenderer::SetUniform( std::string const &uniformName,
// 	std::vector< mat44_fl > const &arrayOfMatrices, int numOfElements )
// {
// 	SetUniform( "gBoneMatrices", &arrayOfMatrices[ 0 ] );
// }