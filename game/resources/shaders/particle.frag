#version 410 core

#define MAX_TEXTURE_COUNT 10

in vec3 aPos;
in vec4 aColor;
in vec2 aTexCoord;
flat in int aTextureIndex;

out vec4 outColor;

uniform sampler2D uTextures[MAX_TEXTURE_COUNT];

vec4 getTextureColor()
{
	switch (aTextureIndex)
	{
		case 0:
			return texture(uTextures[0], aTexCoord);
		
		case 1:
			return texture(uTextures[1], aTexCoord);

		case 2:
			return texture(uTextures[2], aTexCoord);

		case 3:
			return texture(uTextures[3], aTexCoord);

		case 4:
			return texture(uTextures[4], aTexCoord);

		default:
			return vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

void main()
{
	outColor = getTextureColor() * aColor;
} 
