// #pragma once
// 
// #include <map>
// #include "Engine/Renderer/Transform.hpp"
// #include "Engine/Renderer/Meshes/Mesh.hpp" //Need MeshShape enum
// #include "Engine/Renderer/RenderState.hpp"
// 
// 
// //-------------------------------------------------------------------------------------------------
// class Material;
// class Rgba;
// struct mat44_fl;
// struct Uniform;
// struct Attribute;
// class Light;
// 
// 
// //-------------------------------------------------------------------------------------------------
// class MeshRenderer
// {
// private:
// 	Transform m_transform;
// 	RenderState m_renderState;
// 	unsigned int m_vaoID;
// 	Mesh const *m_mesh;
// 	Material *m_material;
// 	std::map<std::string, Uniform*> m_uniforms; //Uniforms individual to this MeshRenderer. Overwrites Material
// 
// public:
// 	MeshRenderer( MeshShape const &meshShape, Transform const &transform = Transform(), RenderState const &renderState = RenderState::BASIC_3D );
// 	MeshRenderer( Mesh const *mesh, Transform const &transform = Transform(), RenderState const &renderState = RenderState::BASIC_3D );
// 	MeshRenderer( Mesh const *mesh, Material *material, Transform const &transform = Transform(), RenderState const &renderState = RenderState::BASIC_3D );
// 	~MeshRenderer();
// 
// 	void Update();
// 	RenderState GetRenderState() const;
// 	void SetLineWidth( float lineWidth );
// 	mat44_fl GetModelMatrix() const;
// 	unsigned int GetGPUProgramID() const;
// 	unsigned int GetSamplerID() const;
// 	unsigned int GetVAOID() const;
// 	unsigned int GetIBOID() const;
// 	unsigned int GetDrawMode() const;
// 	int GetVertCount() const;
// 	int GetIndexBufferCount() const;
// 	void SetPosition( Vector3 const &pos );
// 	void SetPosition( float xPos, float yPos, float zPos );
// 	void SetTransform( Transform const &transform );
// 	std::map<std::string, Uniform*>& GetUniformList();
// 	std::map<std::string, Uniform*>& GetMaterialUniformList();
// 	std::map<std::string, Attribute*> GetMaterialAttributeList() const;
// 	std::vector< DrawInstruction > const & GetDrawInstructions() const;
// 
// 	void SetUniform( std::string const &uniformName, unsigned int uniformValue );
// 	void SetUniform( std::string const &uniformName, int uniformValue );
// 	void SetUniform( std::string const &uniformName, float uniformValue );
// 	void SetUniform( std::string const &uniformName, Vector2 const &uniformValue );
// 	void SetUniform( std::string const &uniformName, Vector3 const &uniformValue );
// 	void SetUniform( std::string const &uniformName, Vector4 const &uniformValue );
// 	void SetUniform( std::string const &uniformName, mat44_fl const &uniformValue );
// 	void SetUniform( std::string const &uniformName, Rgba const &uniformValue );
// 	void SetUniform( std::string const &uniformName, std::string const &uniformValue );
// 
// 	void SetUniform( std::string const &uniformName, int *uniformValue );
// 	void SetUniform( std::string const &uniformName, float *uniformValue );
// 	void SetUniform( std::string const &uniformName, Vector2 *uniformValue );
// 	void SetUniform( std::string const &uniformName, Vector3 *uniformValue );
// 	void SetUniform( std::string const &uniformName, Vector4 *uniformValue );
// 
// 	void SetUniform( std::vector< Light > const &uniformLights, int lightCount );
// 
// 	void SetUniform( std::string const &uniformName, const mat44_fl *uniformValue );
// 	void SetUniform( std::string const &uniformName, std::vector< mat44_fl > const &arrayOfMatrices, int numOfMatrices );
// };