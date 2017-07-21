#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Rgba.hpp"


//-----------------------------------------------------------------------------------------------
enum DebugDrawShape
{
	DRAW_SHAPE_DEBUG_POINT,
	DRAW_SHAPE_DEBUG_LINE,
	DRAW_SHAPE_DEBUG_ARROW,
	DRAW_SHAPE_DEBUG_AABB3,
	DRAW_SHAPE_DEBUG_SPHERE // Can be drawn as three circles around each axis
};


//-----------------------------------------------------------------------------------------------
enum DebugDrawMode
{
	DRAW_MODE_DEPTH_TESTING_ON,
	DRAW_MODE_DEPTH_TESTING_OFF,
	DRAW_MODE_DUAL_MODE
};


//-----------------------------------------------------------------------------------------------
class RenderCommand
{
public:
	RenderCommand();

public:
	DebugDrawShape m_drawShape;
	Vector3 m_startPosition;
	Vector3 m_endPosition;
	Vector3 m_boundingBoxMins;
	Vector3 m_boundingBoxMaxs;
	Rgba m_drawColor;
	Rgba m_faceDrawColor;
	float m_drawDurationSeconds;
	float m_sphereRadius;
	DebugDrawMode m_drawMode;
	bool m_renderingEnabled;
};