#version 410 core

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;

in vec3 inPosition;
in vec2 inUV0;

out vec2 passUV0;
out vec3 passPosition;

void main ( void )
{
    vec4 pos = vec4( inPosition, 1.0f );
    passPosition =  ( gModel * pos ).xyz;

	passUV0 = inUV0;

	pos = gProj * gView * gModel * pos;

	gl_Position = pos;
}
