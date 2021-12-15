#version 440 core

in vec3 normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material{
	//Most of the time, ambient = diffuse, so just use one variable to represent these two
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirectionalLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;
};

struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;	

	float constant;
	float linear;
	float quadratic;
};


struct Spotlight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	vec3 direction;

	float constant;
	float linear;
	float quadratic;

	float cutoff;
	float outerCutoff;
};

uniform vec3 viewPos;

#define NR_POINT_LIGHTS 1
uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Spotlight spotlight;

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main(){
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	//Directional Light
	vec3 result = calcDirLight(dirLight, norm, viewDir);

	//Point Light
	for (int i = 0; i < NR_POINT_LIGHTS; i++){
		result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
	}
	
	//Spotlight
	result += calcSpotLight(spotlight, norm, FragPos, viewDir);
	
	FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);

	//Diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	//Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff *vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);

	//Diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	//Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	//Attenuation
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

	//Combine
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)) * attenuation;
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)) * attenuation;
	vec3 specular = light.specular * spec * vec3(texture(material.diffuse, TexCoords)) * attenuation;

	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);

	//Diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	//Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	//Spotlight
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

	//Attenuation
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

	//Combine
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)) * attenuation;
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)) * attenuation * intensity;
	vec3 specular = light.specular * spec * vec3(texture(material.diffuse, TexCoords)) * attenuation * intensity;

	return (ambient + diffuse + specular);
}