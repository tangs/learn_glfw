#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord1;
layout (location = 2) in vec2 aTexCoord2;

out vec2 outTexCoord1;
out vec2 outTexCoord2;

//uniform float offX;
//uniform float offY;
//uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    outTexCoord1 = aTexCoord1;
    outTexCoord2 = aTexCoord2;
}