#version 330 core

in vec3 aPos;
in vec3 aNormal;
in vec3 aColor;
in vec2 aTextureCoord;

out vec4 outColor;

void main()
{
	outColor = vec4(aColor, 1.0f);
} 
