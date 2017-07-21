#version 410 core

// uniforms
uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;

// inputs
in vec2 inPosition;
in vec2 inUV;
in vec4 inTint;

// outputs
out vec2 passUV;
out vec4 passTint;

// main
void main()
{
	passUV = inUV;
	passTint = inTint;
  
	vec4 pos = vec4( inPosition, 0.f, 1.f );
	pos = pos * gModel * gView * gProj;

	// gl_Position is always a vec4 - clip space vector
	gl_Position = vec4( 1.f );
}