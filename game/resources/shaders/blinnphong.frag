#version 330 core

in vec3 aPos;
in vec3 aNormal;
in vec3 aColor; // TODO(radu): remove
in vec2 aTextureCoord;

out vec4 outColor;

struct Material
{
    vec3 specularColor;
    float shininess;
    // sampler2D albedoTexture;
    vec3 albedoColor;
};

struct Light
{
    vec3 pos;
    vec3 color;
};

uniform Material uMaterial;
uniform Light uLight;

uniform vec3 uAmbientLight;
uniform vec3 uCameraPos;

void main()
{
    vec3 normal = normalize(aNormal);
    vec3 lightDir = normalize(uLight.pos - aPos);
    // vec3 albedo = texture(uMaterial.albedoTexture, aTextureCoord);
    vec3 albedo = uMaterial.albedoColor;

    float diffuseFactor = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = uLight.color * albedo * diffuseFactor;
    
    vec3 viewDir = normalize(uCameraPos - aPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0f), uMaterial.shininess);
    vec3 specular = uMaterial.specularColor * uLight.color * specularFactor;

    vec3 result = uAmbientLight + diffuse + specular;
	outColor = vec4(result, 1.0f);
}