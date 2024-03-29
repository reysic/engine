#version 410 core

uniform sampler2D gTexDiffuse;
uniform sampler2D gTexDepth;

in vec2 passUV0;

out vec4 outColor;

void main ( void )
{
	vec4 diffuse = texture( gTexDiffuse, passUV0 );
	outColor = diffuse;
}