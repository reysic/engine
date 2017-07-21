#version 410 core

uniform vec4 gColor;
uniform sampler2D gDiffuseTex;

uniform int gEffectState;
uniform float gEffectTime;

in vec4 passColor;
in vec2 passUV0;
out vec4 outColor;

void main ( void )
{
	vec4 diffuse = texture( gDiffuseTex, passUV0 );

	if ( gEffectState == 1 )
	{
	   outColor = vec4 ( sin( passColor.x * gEffectTime), cos( passColor.y * gEffectTime), sin( passColor.z * gEffectTime) * cos( passColor.z * gEffectTime), 1.0f) * gColor * diffuse;
	}
	else
	{
	outColor = passColor * gColor * diffuse;
	}
}
