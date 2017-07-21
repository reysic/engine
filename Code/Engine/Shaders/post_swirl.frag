uniform sampler2D gTexDiffuse;
uniform float gTime;

uniform float maxRadius = 400.0;
uniform float minRadius = 200.0;

in vec2 passUV0;
out vec4 outColor; 
 
vec4 PostFX()
{
  float radiusDiff = maxRadius - minRadius;
  float renderRadius = minRadius + ( sin (gTime) * radiusDiff );
  vec2 center = vec2(800.0, 450.0);

  vec2 texSize = vec2(1600.0, 900.0);
  vec2 tc = passUV0 * texSize;
  tc -= center;
  float dist = length(tc);
  if (dist < renderRadius) 
  {
    float percent = (renderRadius - dist) / renderRadius;
    float theta = percent * percent * 0.8 * 8.0;
    float s = sin(theta * gTime);
    float c = cos(theta * gTime);
    tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
  }
  tc += center;
  vec3 color = texture2D(gTexDiffuse, tc / texSize).rgb;
  return vec4(color, 1.0);
}
 
void main (void)
{
  //vec2 uv = passUV0;
  //outColor = PostFX(gTexDiffuse, uv, time);

  outColor = PostFX();
}