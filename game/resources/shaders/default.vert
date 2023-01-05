#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inTextureCoord;

out vec3 aPos;
out vec3 aColor;
out vec3 aNormal;
out vec2 aTextureCoord;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main()
{
	// For the normals, we don't want to apply any non-uniform scaling
	// since that doesn't preserve a vector's direction
	mat3 normalMatrix = mat3(transpose(inverse(uModelMatrix)));
	vec4 modelPos = uModelMatrix * vec4(inPos, 1.0f);

	aPos = vec3(modelPos);
	aNormal = normalMatrix * inNormal;
	aColor = inColor;
	aTextureCoord = inTextureCoord;

	gl_Position = uProjectionMatrix * uViewMatrix * modelPos;
}
