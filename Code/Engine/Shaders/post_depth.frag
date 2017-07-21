#version 410 core

uniform sampler2D gTexDiffuse;
uniform sampler2D gTexDepth;

in vec2 passUV0;

out vec4 outColor;

void main ( void )
{
	vec4 diffuse = texture( gTexDiffuse, passUV0 );
	diffuse = diffuse * 3;
	diffuse = floor( diffuse );
	diffuse = diffuse / 3;

	float depthHere = texture( gTexDepth, passUV0 ).r;
	float scale = 1.0f - ( depthHere * depthHere );
	float depthRight = texture( gTexDepth, passUV0 + scale * vec2( 0.05f, 0.0f) ).r;
	float depthLeft = texture( gTexDepth, passUV0 - scale * vec2( 0.05f, 0.0f) ).r;

	depthLeft = depthLeft * depthLeft;
	depthRight = depthRight * depthRight;
	float diff = abs( depthRight - depthLeft );
	if ( diff > 0.01f )
	{
		outColor = vec4( 0, 0, 0, 1 );
	}
	else
	{
		outColor = vec4( 1 );
	}

	//outColor = outColor * diffuse;

	outColor = outColor * ( mix( vec4( 1.0f), diffuse, 0.3f ));
}