#version 410 core

#define SHADOW_MAP_COUNT 2

in vec3 aPos;
in vec3 aNormal;
in vec2 aTextureCoord;

out vec4 outColor;

struct Material
{
    sampler2D albedoTexture;
    vec3 albedoColor;
    vec3 specularColor;
    float shininess;
};

struct Light
{
    vec3 pos;
    vec3 diffuse;
};

uniform Material uMaterial;
uniform Light uLight;

uniform mat4 uViewMatrix;
uniform sampler2D uShadowMaps[SHADOW_MAP_COUNT];
uniform mat4 uLightSpaceMatrices[SHADOW_MAP_COUNT];

uniform vec3 uAmbientLight;
uniform vec3 uCameraPos;

const float MAX_SHADOW_BIAS = 0.05f;
const float MIN_SHADOW_BIAS = 0.005f;


float getShadowAmount(vec3 normal, vec3 lightDir)
{
    // Select which shadow map to use
    int layer = 0;
    vec4 fragPosViewSpace = uViewMatrix * vec4(aPos, 1.0f);
    float depthViewSpace = abs(fragPosViewSpace.z);

    if (depthViewSpace > 79.0f)
    {
        layer = 1;
    }

    vec4 posLightSpace = uLightSpaceMatrices[layer] * vec4(aPos, 1.0f);

    // Perform perspective division
    vec3 projectedCoords = posLightSpace.xyz / posLightSpace.w;

    // Transform from NDC to range [0, 1]
    projectedCoords = projectedCoords * 0.5f + 0.5f;

    if (projectedCoords.z > 1.0f)
    {
        return 0.0f;
    }

    float mappedDepth = texture(uShadowMaps[layer], projectedCoords.xy).r;
    float currentDepth = projectedCoords.z;

    float bias = max(MAX_SHADOW_BIAS * (1.0f - dot(normal, lightDir)), MIN_SHADOW_BIAS);
    return currentDepth - bias > mappedDepth ? 1.0f : 0.0f;
}

void main()
{
    vec3 normal = normalize(aNormal);
    vec3 lightDir = normalize(uLight.pos - aPos);
    float shadow = 1.0f - getShadowAmount(normal, lightDir);

    vec4 albedoTexture = texture(uMaterial.albedoTexture, aTextureCoord);
    vec3 albedo = albedoTexture.rgb * uMaterial.albedoColor;

    vec3 ambient = albedo * uAmbientLight;

    float diffuseFactor = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = uLight.diffuse * (albedo * diffuseFactor);
    
    vec3 viewDir = normalize(uCameraPos - aPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0f), uMaterial.shininess);
    vec3 specular = uMaterial.specularColor * (uLight.diffuse * specularFactor);

    vec3 result = ambient + (shadow * (diffuse + specular));
	outColor = vec4(result, 1.0f);
}