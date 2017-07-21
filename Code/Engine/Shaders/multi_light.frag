#version 410 core

// Camera position - needed for specular
uniform vec3 gCameraPosition;

// Surface Information : Textures
uniform sampler2D gDiffuseTex;
uniform sampler2D gNormalTex;
uniform sampler2D gSpecularTex;
uniform sampler2D gEmissiveTex;

// Surface Information : Specular
uniform float gSpecularPower;
uniform float gSpecularIntensity;

// Global Light Values
uniform vec4 gAmbientLight;

// Surface Informat

// Global Fog Values
uniform float gFogNear = 10.0f;
uniform float gFogFar = 100.0f;
uniform float gFogNearFactor = 0.0f;
uniform float gFogFarFactor = 1.0f;
uniform vec3 gFogColor;

// Light Values
uniform int gLightCount = 0;

// Arrays are referenced by "gLightPosition[0]", not "gLightPosition"
uniform vec3 gLightPosition[16];
uniform vec3 gLightDirection[16];
uniform vec4 gLightColor[16];
uniform float gLightDirectionFactor[16];

// Attenutation Values
uniform float gNearDistance[16];
uniform float gFarDistance[16];
uniform float gNearFactor[16];
uniform float gFarFactor[16];

uniform float gInnerAngle[16];
uniform float gOuterAngle[16];
uniform float gInnerFactor[16];
uniform float gOuterFactor[16];

//Time
uniform float gAge;

// Surface Information
in vec3 passPosition;
in vec3 passTangent;
in vec3 passBitangent;
in vec2 passUV0;

// Output Channels (Just the default one)
out vec4 outColor;


//------------------------------------------------------------------------
// Used for passing information back from my lighting equation
struct surface_light_t
{
   vec3 surface;
   vec3 specular;
};

//------------------------------------------------------------------------
vec4 VectorAsColor( vec3 vec ) 
{
	return vec4( (vec + vec3(1.0f)) * vec3(.5f), 1.0f );
}

//------------------------------------------------------------------------
surface_light_t CalculateSurfaceLight( vec3 surface_position, vec3 surface_normal,
   vec3 dir_to_eye, 
   float distance_to_eye,
   vec3 light_position, 
   vec3 light_direction,
   vec4 light_color,
   float light_dir_factor,
   float spec_power,
   float spec_intensity,
   float near_distance, float far_distance,
   float near_power, float far_power, 
   float inner_angle, float outer_angle,
   float inner_power, float outer_power ) 
{

   surface_light_t ret2;
      // Calculate some intermediate values, like the half vector
   vec3 vector_to_light = light_position - surface_position;
   float distance_to_light = length(vector_to_light);

   // so if light_dir_factor is 1 - our vector to light is the back along the lights direction
   // otherwise, it's the actual vector TO the light
   vec3 dir_to_light = mix( vector_to_light / distance_to_light, -light_direction, light_dir_factor ); 

   // Distance for "directional lights" is the planar distance, instead of the full vector distance
   distance_to_light = mix( distance_to_light, dot( vector_to_light, -light_direction ), light_dir_factor ); 

   vec3 half_vector = normalize(dir_to_light + dir_to_eye);
   float angle = dot( light_direction, -dir_to_light ); 

   // Calculate falloff due to cone angle & distance
   float distance_attenuation = mix( near_power, far_power, 
      smoothstep( near_distance, far_distance, distance_to_light ) );

   float angle_attenuation = mix( inner_power, outer_power, 
      smoothstep( inner_angle, outer_angle, angle ) );

   float attenuation = angle_attenuation * distance_attenuation;


   // Dot3 Lighting
   float dot3_factor = max( dot( surface_normal, dir_to_light ), 0.0f ) * attenuation;
   vec3 dot3_color = light_color.xyz * dot3_factor;
   //ret2.surface = vec3(dot3_factor);
   //return ret2;


   // Specular Lighting
   float spec_factor = max( dot( surface_normal, half_vector ), 0.0f );
   spec_factor = pow( spec_factor, spec_power ) * spec_intensity * attenuation;
   vec3 spec_color = light_color.xyz * spec_factor;

   // Return it
   surface_light_t ret;
   ret.surface = dot3_color;
   ret.specular = spec_color;

   return ret;
}

//------------------------------------------------------------------------
void main()
{
   // Get surface information from textures
   vec4 diffuse = texture( gDiffuseTex, passUV0 );
	vec3 normal = texture( gNormalTex, passUV0 ).xyz;
	
   vec4 emissive = texture( gEmissiveTex, passUV0 );
   float spec_intensity = texture( gSpecularTex, passUV0 ).r;

   spec_intensity = spec_intensity * gSpecularIntensity;

   // Ambient is "World Light"
   // Emissive is Light emitted by the surface itself.
   vec3 ambient_color = gAmbientLight.rgb * gAmbientLight.a;
   vec3 emissive_color = emissive.rgb * emissive.a;
   emissive_color = emissive_color * gAge;

   // Create the TBN matrix from passes tangent and bitangent
	vec3 surface_tan = normalize(passTangent);
	vec3 surface_bitangent = normalize(passBitangent);
	vec3 surface_normal = cross( surface_tan, surface_bitangent );
   surface_bitangent = cross(surface_normal, surface_tan);
	mat3 tbn = mat3( surface_tan, surface_bitangent, surface_normal );
	tbn = transpose( tbn );

   //outColor = (VectorAsColor(surface_tan) + diffuse) / 2.f;
   //return;

	// Move the normal to world space
   normal = (normal * vec3(2.0f, 2.0f, 1.0f)) - vec3(1.0f, 1.0f, 0.0f);
   normal = normalize(normal);
	normal = normal * tbn;
	
   vec3 vector_to_eye = gCameraPosition - passPosition;
   float distance_to_eye = length(vector_to_eye);
   vec3 dir_to_eye = vector_to_eye / distance_to_eye;

   vec3 dot3_light = vec3(0.0f);
   vec3 spec_light = vec3(0.0f);

   // Go through each light, and add the light factor.
   for (int i = 0; i < gLightCount; ++i) {
      surface_light_t light = CalculateSurfaceLight( passPosition, 
         normal,
         dir_to_eye,
         distance_to_eye, 
         gLightPosition[i],
         gLightDirection[i],
         gLightColor[i],
         gLightDirectionFactor[i],
         gSpecularPower,
         spec_intensity,
         gNearDistance[i], gFarDistance[i],
         gNearFactor[i], gFarFactor[i],
         gInnerAngle[i], gOuterAngle[i],
         gInnerFactor[i], gOuterFactor[i] );

      dot3_light += light.surface;
      spec_light += light.specular;
      //outColor = vec4(spec_light + ambient_color, 1.f);
      //return;
   }

   // The clamp is optional - but if you remove it, it allows your lights to "wash out" your surface
   // which *might* be an effect you want?
   vec3 surface_light =  (ambient_color + dot3_light) + emissive_color;

   surface_light = clamp( surface_light, vec3(0.0f), vec3(1.0f) );


   // Final lighting equation - add it all together, and clamp again to put us in our valid light range
   vec4 final_color = diffuse * vec4(surface_light, 1.0f) + vec4( spec_light, 0.0f );
   final_color = clamp( final_color, vec4(0.0f), vec4(1.0f) );

   // Add in fog
   float fog_factor = mix( gFogNearFactor, gFogFarFactor, 
      smoothstep( gFogNear, gFogFar, distance_to_eye ) );
   final_color = mix( final_color, vec4(gFogColor, 1.0f), fog_factor );

   outColor = final_color;
   //outColor = vec4(gLightColor[1]);
}