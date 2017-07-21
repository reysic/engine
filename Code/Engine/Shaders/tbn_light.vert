#version 410 core

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;

in vec3 inPosition;
in vec3 inTangent;
in vec3 inBitangent;
in vec2 inUV0;

out vec2 passUV0;
out vec3 passPosition;
out vec3 passTangent;
out vec3 passBitangent;

void main ( void )
{
    vec4 pos = vec4( inPosition, 1.0f );
    passPosition =  ( gModel * pos ).xyz;

    // Pass Tangent and Bitanget to the Fragment Shader
    passTangent = ( vec4( inTangent, 0.0f ) * gModel ).xyz;
    passBitangent = ( vec4( inBitangent, 0.0f ) * gModel).xyz;

	passUV0 = inUV0;

	pos = gProj * gView * gModel * pos;

	gl_Position = pos;
}
