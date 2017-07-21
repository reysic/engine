uniform sampler2D gTexDiffuse;
uniform float gTime;

in vec2 passUV0;
out vec4 outColor;

float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

void main (void)
{
  vec3 tc = vec3(1.0, 0.0, 0.0);
  if ( gl_FragCoord.x < 800 )
  {
    vec2 uv = passUV0.xy;
    tc = texture2D(gTexDiffuse, uv).rgb * weight[0];
    for (int i=1; i<3; i++) 
    {
      tc += texture2D(gTexDiffuse, uv + vec2(0.0, offset[i])/900.0).rgb \
              * weight[i];
      tc += texture2D(gTexDiffuse, uv - vec2(0.0, offset[i])/900.0).rgb \
             * weight[i];
    }
  }
  else if (gl_FragCoord.x>=(800.0+0.01))
  {
    tc = texture2D(gTexDiffuse, passUV0.xy).rgb;
  }
  outColor = vec4(tc, 1.0);
}