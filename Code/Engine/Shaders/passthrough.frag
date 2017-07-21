#version 410 core

uniform vec4 gColor;

in vec4 passColor;
out vec4 outColor;

void main ( void )
{
	vec2 rb = gl_FragCoord.xy;

	float len = length( vec2( 800, 600 ) - rb );

	//outColor = vec4 (sin(len / 20.0f), 0.0f, 0.0f, 1.0f);
	outColor = gColor;
}
