#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec4 inColor;

out vec3 aPos;
out vec3 aColor;

uniform mat4 uViewProjMatrix;

void main()
{
	aPos = inPos;
    // PhysX colors are a u32, with components in order: argb
	aColor = vec3(inColor.y, inColor.z, inColor.w);

	gl_Position = uViewProjMatrix * vec4(inPos, 1.0f);
}
