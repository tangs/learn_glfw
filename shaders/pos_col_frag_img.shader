#version 330 core
out vec4 FragColor;

in vec2 outTexCoord1;
in vec2 outTexCoord2;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main() {
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    FragColor = mix(texture(texture1, outTexCoord1), texture(texture2, outTexCoord2), mixValue) * vec4((ambient + diffuse), 1.0f);
}