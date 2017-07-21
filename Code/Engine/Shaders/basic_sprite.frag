#version 410 core

// uniforms 
uniform sampler2D gTexDiffuse;
//uniform vec4 gLayerTint;

// inputs
in vec2 passUV;
in vec4 passTint;

// ouputs
out vec4 fragmentColor;

// main
void main()
{
  vec4 diffuseColor = texture( gTexDiffuse, passUV );
  
  //fragmentColor = passTint * diffuseColor * gLayerTint;
  fragmentColor = passTint * diffuseColor;
}
