#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 outColor;

uniform float offX;
uniform float offY;

void main() {
    gl_Position = vec4(aPos.x + offX, aPos.y + offY, aPos.z, 1.0);
    outColor = vec3(gl_Position.x + 0.5, gl_Position.y + 0.5, gl_Position.z + 0.5);
}