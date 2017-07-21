#version 410 core

uniform sampler2D gTexDiffuse;
uniform sampler2D gTexDepth;

in vec2 passUV0;

out vec4 outColor;

void main ( void )
{
	vec4 diffuse = texture( gTexDiffuse, passUV0 );

	vec4 toGray = vec4( 0.2126, 0.7152, 0.0722, 0.0 );
	float gray = dot( diffuse, toGray );

	vec3 r = vec3( 0.90, 0.20, 0.19 );
	vec3 p = vec3( 0.0, 0.20, 0.33 );
	vec3 l = vec3( 0.95, 0.8, 0.7 );
	vec3 b = vec3( 0.4, 0.7, 0.8 );

	vec3 color;
	if ( gray < 0.1 )
	{
		color = p;
	}
	else if ( gray < 0.4 )
	{
		color = r;
	}
	else if ( gray < 0.7 )
	{
		color = b;
	}
	else
	{
		color = l;
	}

	outColor = vec4( color, 1.0 );
}