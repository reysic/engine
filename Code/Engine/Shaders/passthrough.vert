#version 410 core

in vec3 inPosition;
in vec4 inColor;

out vec4 passColor;

void main (void)
{
	passColor = inColor;
	gl_Position = vec4(inPosition, 1.f);
}
