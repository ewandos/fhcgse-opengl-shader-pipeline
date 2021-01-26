#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform float alpha;
uniform vec3 viewPos;
uniform Light light;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main() {
    // ambient light
    vec3 ambient = light.ambient * texture(texture_diffuse, TexCoords).rgb;

    // diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse, TexCoords).rgb;

    // specular light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specular * spec * texture(texture_specular, TexCoords).rgb;

    // put it all together
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, alpha);
}