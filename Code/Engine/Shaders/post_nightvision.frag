#version 410 core

//Main reference: http://www.geeks3d.com/20091009/shader-library-night-vision-post-processing-filter-glsl/



in vec2 passUV0;
out vec4 outColor;

uniform sampler2D gTexDiffuse;
uniform sampler2D gTexNoise; 
uniform sampler2D gTexMask; 
uniform float gTime = 1.f; // seconds
uniform float luminanceThreshold = 0.2f; // 0.2
uniform float colorAmplification = 4.0; // 4.0
uniform float effectCoverage = 1.f; // 0.5
void main ()
{

  //outColor = texture2D(gNoiseTex, passUV0);
  //return;
    vec4 finalColor;
    // Set effectCoverage to 1.0 for normal use.  
    if (passUV0.x < effectCoverage) 
    {
      vec2 uv;           
      uv.x = 0.4*sin(gTime*50.0);                                 
      uv.y = 0.4*cos(gTime*50.0);                                 
      float m = texture2D(gTexMask, passUV0.st).r;
      vec3 n = texture2D(gTexNoise, 
                 (passUV0.st*3.5) + uv).rgb;
      vec3 c = texture2D(gTexDiffuse, passUV0.st 
                               + (n.xy*0.005)).rgb;
  
      float lum = dot(vec3(0.30, 0.59, 0.11), c);
      if (lum < luminanceThreshold)
          c *= colorAmplification; 
  
      vec3 visionColor = vec3(0.1, 0.95, 0.2);
      finalColor.rgb = (c + (n*0.2)) * visionColor * m;
     }
     else
     {
    finalColor = texture2D(gTexDiffuse, 
                   passUV0.st);
     }
    outColor.rgb = finalColor.rgb;
    outColor.a = 1.0;
} 