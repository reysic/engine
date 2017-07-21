#include "Engine/Renderer/RenderCommand.hpp"


RenderCommand::RenderCommand()
	: m_drawShape( DRAW_SHAPE_DEBUG_LINE )
	, m_startPosition( Vector3::ZERO )
	, m_endPosition( Vector3( 1.0f, 1.0f, 1.0f ) )
	, m_boundingBoxMins( Vector3::ZERO )
	, m_boundingBoxMaxs( Vector3::ZERO )
	, m_drawColor( Rgba::WHITE )
	, m_faceDrawColor( Rgba::WHITE )
	, m_drawDurationSeconds( -1.0f )
	, m_sphereRadius( 0.0f )
	, m_drawMode( DRAW_MODE_DEPTH_TESTING_ON )
	, m_renderingEnabled( true )
{
}