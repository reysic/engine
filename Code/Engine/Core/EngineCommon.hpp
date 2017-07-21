#pragma once

#include <cstdint>

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Tools/Logging/Logger.hpp"
#include "Engine/Tools/Memory/MemoryAnalytics.hpp"
#include "Engine/Tools/Profiling/Profiler.hpp"


//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
struct mat44_fl;
enum Endianness;
class Window;


//-----------------------------------------------------------------------------------------------
extern float		g_engineDeltaSeconds;
extern bool			g_effectState;
extern bool			g_drawOriginAxes;
extern bool			g_debugMode;
extern int			g_lightMode;
extern mat44_fl		g_engineBasis;
extern Endianness	g_engineEndianness;
extern Window*		g_appWindow;


//-------------------------------------------------------------------------------------------------
enum Blending
{
	Blending_NORMAL = 0,
	Blending_SUBTRACTIVE,
	Blending_ADDITIVE,
	Blending_INVERTED,
	NUM_BLENDING_MODES
};


//-------------------------------------------------------------------------------------------------
enum DrawMode
{
	DrawMode_FULL = 0,
	DrawMode_LINE,
	DrawMode_POINT,
	NUM_DRAW_MODES
};


//-------------------------------------------------------------------------------------------------
enum Direction
{
	DIRECTION_EAST = 0,
	DIRECTION_NORTHEAST,
	DIRECTION_NORTH,
	DIRECTION_NORTHWEST,
	DIRECTION_SOUTH,
	DIRECTION_SOUTHEAST,
	DIRECTION_WEST,
	DIRECTION_SOUTHWEST,
	DIRECTION_INVALID
};


//-----------------------------------------------------------------------------------------------
enum Endianness
{
	ENDIANNESS_BIG = 0,
	ENDIANNESS_LITTLE
};


//-----------------------------------------------------------------------------------------------
void InitializeEngineCommon();
void ShutdownEngineCommon();
void InitializeMemoryAnalytics();
void ShutdownMemoryAnalytics();
void InitializeLogger();
void ShutdownLogger();
void InitializeProfiler();
void ShutdownProfiler();
Endianness GetSystemEndianness();
void InitializeEngineSubsystems();
void ShutdownEngineSubsystems();
void UpdateEngineSubsystems( float deltaSeconds );
void RenderEngineSubsystems();