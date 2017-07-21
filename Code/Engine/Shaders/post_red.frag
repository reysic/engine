#version 410 core

uniform sampler2D gTexDiffuse;
uniform sampler2D gTexDepth;

in vec2 passUV0;
in vec4 gl_FragCoord;

out vec4 outColor;

void main ( void )
{
	float depthHere = texture( gTexDepth, passUV0 ).r;

	if ( gl_FragCoord.x < 800 )
	{
		float red = texture( gTexDiffuse, passUV0 ).r;
		outColor = vec4( red, 0, 0, 1 );
	}
	else
	{
		outColor = vec4( vec3( depthHere ), 1 );
	}
}