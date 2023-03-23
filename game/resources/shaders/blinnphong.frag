#version 330 core

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

uniform vec3 uAmbientLight;
uniform vec3 uCameraPos;

void main()
{
    vec3 normal = normalize(aNormal);
    vec3 lightDir = normalize(uLight.pos - aPos);
    vec4 albedoTexture = texture(uMaterial.albedoTexture, aTextureCoord);
    vec3 albedo = albedoTexture.rgb * uMaterial.albedoColor;

    vec3 ambient = albedo * uAmbientLight;

    float diffuseFactor = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = uLight.diffuse * (albedo * diffuseFactor);
    
    vec3 viewDir = normalize(uCameraPos - aPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0f), uMaterial.shininess);
    vec3 specular = uMaterial.specularColor * (uLight.diffuse * specularFactor);

    vec3 result = ambient + diffuse + specular;
	outColor = vec4(result, 1.0f);
}