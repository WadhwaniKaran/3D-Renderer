#version 330 core

#define NUM_DIFFUSE 3
#define NUM_SPECULAR 3
#define NUM_EMISSION 3
#define NUM_POINT_LIGHT 4

struct Material {
	sampler2D diffuse[NUM_DIFFUSE];
	sampler2D specular[NUM_SPECULAR];
	sampler2D emission[NUM_EMISSION];
	float shininess;
};

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct FlashLight {
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHT];
uniform FlashLight flashLight;
uniform vec3 viewPos;

out vec4 FragColor;

struct MaterialTex {
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
};

vec3 calc_dir_light(DirLight dirLight, MaterialTex mTex, vec3 viewVec, vec3 normal);
vec3 calc_point_light(PointLight pointLight, MaterialTex mTex, vec3 viewVec, vec3 normal);
vec3 calc_flash_light(FlashLight flashLight, MaterialTex mTex, vec3 viewVec, vec3 normal);

vec3 sum_diffuse();
vec3 sum_specular();
vec3 sum_emission();

void main(){
	vec3 result = vec3(0.0);
	
	MaterialTex mTex;
	mTex.diffuse = sum_diffuse();
	mTex.specular = sum_specular();
	mTex.emission = sum_emission();

	vec3 normal = normalize(Normal);
	vec3 viewVec = FragPos - viewPos; // towards fragment - not normalized

	for(int i = 0; i < NUM_POINT_LIGHT; i++) {
		result += calc_point_light(pointLights[i], mTex, viewVec, normal);
	}
	result /= NUM_POINT_LIGHT;

	result += calc_dir_light(dirLight, mTex, viewVec, normal);

	result += calc_flash_light(flashLight, mTex, viewVec, normal);

	//result += mTex.emission;

	FragColor = vec4(result, 1.0);
}

vec3 calc_dir_light(DirLight dirLight, MaterialTex mTex, 
				vec3 viewVec, vec3 normal) {
	vec3 viewDir = normalize(viewVec);
	
	vec3 ambient = dirLight.ambient * mTex.diffuse;

	float diff = max(dot(-dirLight.direction, normal), 0.0);
	vec3 diffuse = diff * dirLight.diffuse * mTex.diffuse;

	vec3 reflectDir = reflect(-dirLight.direction, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * dirLight.specular * mTex.specular;

	return (ambient + diffuse + specular);
}

vec3 calc_point_light(PointLight pointLight, MaterialTex mTex, 
				vec3 viewVec, vec3 normal) {
	vec3 lightDir = FragPos - pointLight.position;
	float distance = length(lightDir);
	vec3 viewDir = normalize(viewVec);
	lightDir = normalize(lightDir);

	float attenuation = 1 / (pointLight.constant + distance * pointLight.linear 
							+ pointLight.quadratic * distance * distance);

	vec3 ambient = pointLight.ambient * mTex.diffuse * attenuation;

	float diff = max(dot(-lightDir, normal), 0.0);
	vec3 diffuse = diff * pointLight.diffuse * mTex.diffuse * attenuation;

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
	vec3 specular = spec * pointLight.specular * mTex.specular * attenuation;

	return (ambient + diffuse + specular);
}

vec3 calc_flash_light(FlashLight flashLight, MaterialTex mTex, 
				vec3 viewVec, vec3 normal) {
	vec3 lightDir = FragPos - flashLight.position;
	float distance = length(lightDir);
	lightDir = normalize(lightDir);
	vec3 viewDir = normalize(viewVec);

	float theta = dot(-lightDir, normalize(flashLight.direction));
	if(theta <= flashLight.outerCutOff) {
		return vec3(0.0);
	}
	float epsilon = flashLight.cutOff - flashLight.outerCutOff;
	float intensity = clamp((theta - flashLight.outerCutOff) / epsilon, 0.0, 1.0);

	float attenuation = 1 / (flashLight.constant + distance * flashLight.linear 
							+ flashLight.quadratic * distance * distance);

	vec3 ambient = flashLight.ambient * mTex.diffuse * attenuation * intensity;

	float diff = max(dot(-lightDir, normal), 0.0);
	vec3 diffuse = diff * flashLight.diffuse * mTex.diffuse * attenuation * intensity;

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
	vec3 specular = spec * flashLight.specular * mTex.specular * attenuation * intensity;

	return (diffuse + specular);
}

vec3 sum_diffuse() {
	vec4 result = vec4(0.0);
	for(int i = 0; i < NUM_DIFFUSE; i++) {
		result += texture(material.diffuse[i], TexCoords);
	}
	return vec3(result);
}

vec3 sum_specular() {
	vec4 result = vec4(0.0);
	for(int i = 0; i < NUM_SPECULAR; i++) {
		result += texture(material.specular[i], TexCoords);
	}
	return vec3(result);
}

vec3 sum_emission() {
	vec4 result = vec4(0.0);
	for(int i = 0; i < NUM_EMISSION; i++) {
		result += texture(material.emission[i], TexCoords);
	}

	float distance = length(FragPos - viewPos);
	float attenuation = 1 / (1.0 + distance * 0.14 + distance * distance * 0.07);

	result *= attenuation / NUM_EMISSION;

	return vec3(result);
}