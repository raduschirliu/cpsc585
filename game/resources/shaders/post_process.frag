#version 410 core

in vec2 aTexCoord;

out vec4 oColor;

uniform sampler2D uScreenTexture;
uniform float uGamma = 2.2;
uniform float uExposure = 1.0;

void main()
{
    vec3 hdrColor = texture(uScreenTexture, aTexCoord).rgb;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * uExposure);

    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / uGamma));  
    oColor = vec4(mapped, 1.0);
}
