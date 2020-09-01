#version 330 core
out vec4 FragColor;

in vec2 outTexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

void main() {
    vec2 coord = vec2(outTexCoord.x, outTexCoord.y);
    FragColor = texture(texture1, coord);
//    FragColor = mix(texture(texture1, coord), texture(texture2, coord), 0.3);
}