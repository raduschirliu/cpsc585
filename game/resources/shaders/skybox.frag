#version 410 core

in vec3 aTextureCoord;

out vec4 outColor;

uniform samplerCube uSkybox;

void main()
{
	outColor = texture(uSkybox, aTextureCoord);
} 
