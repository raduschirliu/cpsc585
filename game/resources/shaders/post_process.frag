#version 410 core

in vec2 aTexCoord;

out vec4 oColor;

uniform sampler2D uScreenTexture;

void main()
{
    oColor = texture(uScreenTexture, aTexCoord);
}
