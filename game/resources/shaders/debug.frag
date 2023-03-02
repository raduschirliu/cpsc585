#version 330 core

in vec3 aPos;
in vec3 aColor;

out vec4 outColor;

void main()
{
	outColor = vec4(aColor, 1.0f);
} 
