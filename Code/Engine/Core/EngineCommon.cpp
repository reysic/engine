#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/UI/UISystem.hpp"
#include "Engine/UI/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Cameras/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/DeveloperConsole.hpp"


//-----------------------------------------------------------------------------------------------
float			g_engineDeltaSeconds;
bool			g_effectState;
bool			g_drawOriginAxes;
bool			g_debugMode;
int				g_lightMode;
mat44_fl		g_engineBasis;
Endianness		g_engineEndianness;
std::thread*	g_loggingThread;
Window*			g_appWindow;


//-----------------------------------------------------------------------------------------------
void InitializeEngineCommon()
{
	g_engineDeltaSeconds = 0.0f;
	g_effectState = false;
	g_drawOriginAxes = true;
	g_debugMode = false;
	g_lightMode = 0;

	MatrixGenerateBasis( &g_engineBasis, Vector3( 0.0f, -1.0f, 0.0f ) );

	g_engineEndianness = GetSystemEndianness();

	InitializeLogger();

	InitializeMemoryAnalytics();

	InitializeProfiler();
}


//-----------------------------------------------------------------------------------------------
void ShutdownEngineCommon()
{
	ShutdownMemoryAnalytics();

	ShutdownLogger();

	ShutdownProfiler();
}


//-----------------------------------------------------------------------------------------------
void InitializeMemoryAnalytics()
{
	MemoryAnalyticsStartup();
}


//-----------------------------------------------------------------------------------------------
void ShutdownMemoryAnalytics()
{
	MemoryAnalyticsShutdown();
}


//-----------------------------------------------------------------------------------------------
void InitializeLogger()
{
	g_loggerIsRunning = true;
	g_loggingThread = new std::thread( LoggingThread, std::ref( g_messageQueue ) );
}


//-----------------------------------------------------------------------------------------------
void ShutdownLogger()
{
	g_loggerIsRunning = false;
	g_loggingThread->join();
	delete g_loggingThread;
}


//-----------------------------------------------------------------------------------------------
void InitializeProfiler()
{
	ProfilerSystemStartup();
}


//-----------------------------------------------------------------------------------------------
void ShutdownProfiler()
{
	ProfilerSystemShutdown();
}


//-----------------------------------------------------------------------------------------------
Endianness GetSystemEndianness()
{
	union
	{
		int data;
		unsigned char buffer[ 4 ];
	} test;

	test.data = 1;
	if ( test.buffer[ 0 ] == 1 )
	{
		return ENDIANNESS_LITTLE;
	}
	else
	{
		return ENDIANNESS_BIG;
	}
}


//-----------------------------------------------------------------------------------------------
void InitializeEngineSubsystems()
{
	g_theRenderer = new Renderer();
	g_theCamera = new Camera();
	g_theAudioSystem = new AudioSystem();
	g_theInputSystem = new InputSystem();
	g_theDeveloperConsole = new DeveloperConsole( 1600, 900 );
	g_theUISystem = new UISystem();
	g_appWindow = new Window( 1600, 900, NULL );
}


//-----------------------------------------------------------------------------------------------
void ShutdownEngineSubsystems()
{
	delete g_theRenderer;
	g_theRenderer = nullptr;
	delete g_theCamera;
	g_theCamera = nullptr;
	delete g_theInputSystem;
	g_theInputSystem = nullptr;
	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;
	delete g_theDeveloperConsole;
	g_theDeveloperConsole = nullptr;
	delete g_theUISystem;
	g_theUISystem = nullptr;
}


//-----------------------------------------------------------------------------------------------
void UpdateEngineSubsystems( float deltaSeconds )
{
	g_theInputSystem->Update( deltaSeconds );
	g_theAudioSystem->Update( deltaSeconds );
	g_theDeveloperConsole->Update( deltaSeconds );
	g_theUISystem->Update( deltaSeconds );
	g_appWindow->Update( deltaSeconds );
	g_theInputSystem->UpdateKeyJustPressedState();
	MemoryAnalyticsUpdate( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void RenderEngineSubsystems()
{
	g_theDeveloperConsole->Render();
	g_theUISystem->Render();
	MemoryAnalyticsRender();
}
