#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
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
    vec3 diffuse;
    vec3 specular;
    float cutoff;
    float outerCutoff;

    float constant;
    float linear;
    float quadratic;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform DirLight dir_light;
#define NMBR_POINT_LIGHTS 4
uniform PointLight point_lights[NMBR_POINT_LIGHTS];
uniform FlashLight flash_light;
uniform vec3 viewPos; 

vec3 calc_dir_light(DirLight dir_light, vec3 view_dir, vec3 normal);
vec3 calc_point_light(PointLight point_light, vec3 view_dir, vec3 normal);
vec3 calc_flash_light(FlashLight flash_light, vec3 normal);

out vec4 FragColor;

void main() {

    vec3 result = vec3(0.0);
    vec3 normal = normalize(Normal);
    vec3 view_dir = normalize(FragPos - viewPos);  // towards fragment

    result += calc_dir_light(dir_light, view_dir, normal);

    for(int i = 0; i < NMBR_POINT_LIGHTS; i++) {
        result += calc_point_light(point_lights[i], view_dir, normal);
    }

    result += calc_flash_light(flash_light, normal);

    result += vec3(texture(material.emission, TexCoords));

    FragColor = vec4(result, 1.0);
}

vec3 calc_dir_light(DirLight dir_light, vec3 view_dir, vec3 normal) {
    vec3 ambient = dir_light.ambient * vec3(texture(material.diffuse, TexCoords));

    float diff = max(dot(-dir_light.direction, normal), 0.0);
    vec3 diffuse = diff * dir_light.diffuse * vec3(texture(material.diffuse, TexCoords));

    vec3 reflect_dir = reflect(-dir_light.direction, normal);
    float spec = pow(max(dot(-view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = spec * dir_light.specular * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);
}

vec3 calc_point_light(PointLight point_light, vec3 view_dir, vec3 normal) {
    vec3 point_light_dir = FragPos - point_light.position;                      // towards fragment
    float distance = length(point_light_dir);
    point_light_dir = normalize(point_light_dir);

    float attenuation = 1 / (point_light.constant + distance * point_light.linear +
                        distance * distance * point_light.quadratic);

    vec3 ambient = point_light.ambient * vec3(texture(material.diffuse, TexCoords));
    ambient *= attenuation;

    float diff = max(dot(-point_light_dir, normal), 0.0);
    vec3 diffuse = diff * point_light.diffuse * vec3(texture(material.diffuse, TexCoords));
    diffuse *= attenuation;

    vec3 reflect_dir = reflect(-point_light_dir, normal);
    float spec = pow(max(dot(-view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = spec * point_light.specular *  vec3(texture(material.specular, TexCoords));
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calc_flash_light(FlashLight flash_light, vec3 normal) {
    vec3 fl_dir = FragPos - flash_light.position;
    float distance = length(fl_dir);
    fl_dir = normalize(fl_dir);

    float theta = dot(fl_dir, normalize(flash_light.direction));
    if(theta <= flash_light.outerCutoff) {
        return vec3(0.0);
    }
    float epsilon = flash_light.cutoff - flash_light.outerCutoff;
    float intensity = clamp((theta - flash_light.outerCutoff) / epsilon, 0.0, 1.0);

    float attenuation = 1 / (flash_light.constant + distance * flash_light.linear +
                        distance * distance * flash_light.quadratic);

    float diff = max(dot(-fl_dir, normal), 0.0);
    vec3 diffuse = diff * flash_light.diffuse * attenuation * intensity *
                    vec3(texture(material.diffuse, TexCoords));

    vec3 reflect_dir = reflect(-fl_dir, normal);
    float spec = pow(max(dot(-fl_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = spec * flash_light.specular * attenuation * intensity *
                    vec3(texture(material.specular, TexCoords));

    return (diffuse + specular);
}