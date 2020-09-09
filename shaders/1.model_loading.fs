#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main() {
    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec4 diffuseTexture = texture(texture_diffuse1, TexCoords);
    vec4 specularTexture = texture(texture_specular1, TexCoords);

//     FragColor = (diffuseTexture + specularTexture) * vec4((ambient + diffuse), 1.0f);
    FragColor = (diffuseTexture) * vec4((ambient + diffuse), 1.0f);

//     vec4 diffuse = texture(texture_diffuse1, TexCoords);
// 	vec4 specular = texture(texture_specular1, TexCoords);
//     FragColor = diffuse + specular;
//     FragColor = texture(texture_diffuse1, TexCoords);
}