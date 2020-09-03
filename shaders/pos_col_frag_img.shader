#version 330 core
out vec4 FragColor;

in vec2 outTexCoord1;
in vec2 outTexCoord2;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

void main() {
//    vec2 coord = vec2(outTexCoord1.x, outTexCoord2.y);
//    FragColor = texture(texture2, coord);
    FragColor = mix(texture(texture1, outTexCoord1), texture(texture2, outTexCoord2), mixValue);
}