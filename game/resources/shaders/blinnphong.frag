#version 330 core

in vec3 aPos;
in vec4 aPosLightSpace;
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
uniform sampler2D uDepthMap;

uniform vec3 uAmbientLight;
uniform vec3 uCameraPos;

const float MAX_SHADOW_BIAS = 0.05f;
const float MIN_SHADOW_BIAS = 0.005f;

float getShadowAmount(vec3 normal, vec3 lightDir)
{
    // Perform perspective division
    vec3 projectedCoords = aPosLightSpace.xyz / aPosLightSpace.w;

    // Transform from NDC to range [0, 1]
    projectedCoords = projectedCoords * 0.5f + 0.5f;

    if (projectedCoords.z > 1.0f)
    {
        return 0.0f;
    }

    float mappedDepth = texture(uDepthMap, projectedCoords.xy).r;
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