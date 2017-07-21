#pragma once

#include "Engine/Core/EngineCommon.hpp"


//-------------------------------------------------------------------------------------------------
struct RenderState
{
public:
	bool m_backfaceCullingEnabled;
	bool m_depthTestingEnabled;
	bool m_depthWritingEnabled;
	Blending m_blendingMode;
	DrawMode m_drawMode;
	bool m_windingClockwise;
	float m_lineWidth;
	//GrpahicsDevice,
	//IsDisposed,
	//AntiAlias,
	//Name,
	//ScissorTestEnable,
	//SlopeScaleDepthBias,
	//Tag,

public:
	static const RenderState BASIC_3D;
	static const RenderState BASIC_2D;

public:
	RenderState( bool backfaceCulling, bool depthTesting, bool depthWriting, Blending const &blendingMode, DrawMode const &drawMode, bool windingClockwise );
	~RenderState();
};