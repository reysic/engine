#version 410 core

uniform sampler2D gTexDiffuse;
uniform sampler2D gTexNormal;
uniform sampler2D gTexSpecular;

uniform vec4 gAmbientLight;

uniform vec3 gLightPosition;
uniform vec4 gLightColor;
uniform float gLightIntensity;

uniform float gSpecularPower;

uniform vec3 gCameraPosition;

uniform float gMinDistanceFromCamera;
uniform float gMaxDistanceFromCamera;
uniform vec4 gFogColor;

in vec2 passUV0;
in vec3 passPosition;

out vec4 outColor;

vec3 CalculateSpecularFactor( vec3 normal )
{
	float specularIntensity = texture( gTexSpecular, passUV0 ).r;

	vec3 vecToLight = gLightPosition - passPosition;
	float power = dot( vecToLight, vecToLight );

	vecToLight = normalize( vecToLight );
	vec3 vecToEye = normalize( gCameraPosition - passPosition );
	vec3 halfVector = vecToLight + vecToEye;
	halfVector = normalize( halfVector );

	float halfDotNormal = clamp( dot( halfVector, normal ), 0.0f, 1.0f );
	float intensity = pow( halfDotNormal, gSpecularPower ) * specularIntensity;

	power = gLightIntensity / power;

	vec4 color = intensity * gLightColor * power;
	return color.rgb;
}

vec4 AddFog( vec4 color )
{
	vec3 vecToEye= gCameraPosition - passPosition;
	float vecLenSquared = dot( vecToEye, vecToEye );
	float vecLen = sqrt( vecLenSquared );

	if ( vecLen < gMinDistanceFromCamera )
	{
		return color;
	}
	else if ( vecLen < gMaxDistanceFromCamera )
	{
		vecLen -= gMinDistanceFromCamera;
		float diff = gMaxDistanceFromCamera - gMinDistanceFromCamera;
		float fracDist = vecLen / diff;

		return vec4( gFogColor * fracDist ) + vec4( color * ( 1.0f - fracDist) );
	}
	else
	{
		return gFogColor;
	}
}

vec4 VectorAsColor( vec3 vec )
{
	return vec4( (vec + vec3( 1.0f) ) * vec3( 0.5f ), 1.0f );
}

void main( void )
{
	vec4 color = texture( gTexDiffuse, passUV0 );
	vec3 normal = texture( gTexNormal, passUV0 ).xyz;

	normal = normal * vec3( 2.0f, 2.0f, 1.0f) - vec3( 1.0f, 1.0f, 0.0f );

	vec3 finalNormal = vec3( normal.x, -normal.z, normal.y );
	finalNormal = normalize( finalNormal );

	// vec3 lightIntensity = CalculateLightFactor( finalNormal );
	// vec3 specularFactor = CalculateSpecularFactor( finalNormal );

	// Calculation of light intensity
	vec3 lightIntensity = gAmbientLight.rgb * gAmbientLight.a;

	vec3 vecToLight = gLightPosition - passPosition;
	float invLen = dot( vecToLight, vecToLight );
	invLen = inversesqrt( invLen );

	vecToLight = vecToLight * invLen;
	float power = gLightIntensity * ( invLen * invLen );

	float nDotL = clamp( dot( finalNormal, vecToLight ), 0.0f, 1.0f );
	lightIntensity += nDotL  * gLightColor.rgb * power;

	lightIntensity = clamp( lightIntensity, vec3( 0.0f ), vec3( 1.0f) );

	// Calculation of specular factor
	vec3 specularFactor = CalculateSpecularFactor( finalNormal );

	// Finished calculation of light intensity and specular factor
	outColor = color * vec4( lightIntensity, 1.0f ) + vec4( specularFactor, 1.0f );
	outColor = AddFog( outColor );
	outColor = clamp( outColor, vec4( 0.0f ), vec4( 1.0f ) );
}

// MaterialSetColor( basicLightProgram, "gAmbientLight", color_fl( 1.0f, 1.0f, 1.0f, 0.05f ) );
