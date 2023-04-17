#version 410 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec4 inColor;
layout (location = 3) in mat4 inModelMatrix;
/* uses layout 3-6 */
layout (location = 7) in int inTextureIndex;

out vec3 aPos;
out vec4 aColor;
out vec2 aTexCoord;
flat out int aTextureIndex;

uniform mat4 uViewProjMatrix;

void main()
{
	vec4 worldPos = inModelMatrix * vec4(inPos, 1.0);

	aPos = vec3(worldPos.xyz);
	aColor = inColor;
	aTexCoord = inTexCoord;
	aTextureIndex = inTextureIndex;

	gl_Position = uViewProjMatrix * worldPos;
}
