#version 410 core

uniform float gTime;
uniform sampler2D gTexDiffuse;
uniform sampler2D gTexDepth;

in vec2 passUV0;

out vec4 outColor;

void main ( void )
{
	vec2 offset = passUV0 - vec2( 0.5f );
	float d = length( offset );
	vec2 u = offset / d;

	float offsetAmount = 0.05f * -cos( (gTime * 4.0f ) + d * 100.0f );
	vec2 uv = passUV0 + u * offsetAmount;

	outColor = outColor + texture( gTexDiffuse, uv );
};