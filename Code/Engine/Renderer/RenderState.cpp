#include "Engine/Renderer/RenderState.hpp"


//-------------------------------------------------------------------------------------------------
const RenderState RenderState::BASIC_3D( true, true, true, Blending_NORMAL, DrawMode_FULL, false );
const RenderState RenderState::BASIC_2D( false, false, true, Blending_NORMAL, DrawMode_FULL, false );


//-------------------------------------------------------------------------------------------------
RenderState::RenderState( bool backfaceCulling, bool depthTesting, bool depthWriting, Blending const &blendingMode, DrawMode const &drawMode, bool windingClockwise )
	: m_backfaceCullingEnabled( backfaceCulling )
	, m_depthTestingEnabled( depthTesting )
	, m_depthWritingEnabled( depthWriting )
	, m_blendingMode( blendingMode )
	, m_drawMode( drawMode )
	, m_windingClockwise( windingClockwise )
	, m_lineWidth( 1.f )
{
}


//-------------------------------------------------------------------------------------------------
RenderState::~RenderState()
{
}