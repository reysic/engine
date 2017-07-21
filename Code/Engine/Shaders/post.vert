#version 410 core

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;

in vec3 inPosition;
in vec2 inUV0;

out vec2 passUV0;

void main( void )
{
	passUV0 = inUV0;
	passUV0.y = 1.0 - passUV0.y;

	vec4 pos = vec4( inPosition, 1.0f );
	pos = gProj * gView * gModel * pos;

	gl_Position = pos;
}