#version 410 core

in vec3 aPos;
in vec4 aColor;
in vec2 aTexCoord;
flat in int aTextureIndex;

out vec4 outColor;

void main()
{
	outColor = aColor;
} 
