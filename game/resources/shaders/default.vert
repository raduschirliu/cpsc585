#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inTextureCoord;

out vec3 outPos;
out vec3 outColor;
out vec3 outNormal;
out vec2 outTextureCoord;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uNormalMatrix;

void main()
{
	// For the normals, we don't want to apply any non-uniform scaling
	// since that doesn't preserve a vector's direction
	vec4 modelPos = uModelViewProjMatrix * vec4(inPos, 1.0f);

	outPos = vec3(modelPos);
	outNormal = uNormalMatrix * vec4(inNormal, 1.0f);
	outColor = inColor;
	outTextureCoord = inTextureCoord;

	gl_Position = modelPos;
}
