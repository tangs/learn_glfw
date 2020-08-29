#version 330 core
out vec4 FragColor;

in vec3 outColor;
in vec2 outTexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

void main() {
    vec2 coord = vec2(1.0 - outTexCoord.x, outTexCoord.y);
//    FragColor = mix(texture(texture1, coord), texture(texture2, coord), mixValue);
    FragColor = texture(texture1, coord) * vec4(outColor, 1.0);
}