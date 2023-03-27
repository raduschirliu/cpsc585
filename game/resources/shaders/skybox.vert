#version 330 core

layout (location = 0) in vec3 inPos;

out vec3 aTextureCoord;

uniform mat4 uViewProjMatrix;

void main()
{
	aTextureCoord = inPos;

	vec4 pos = uViewProjMatrix * vec4(inPos, 1.0f);
	gl_Position = pos.xyww;
}
