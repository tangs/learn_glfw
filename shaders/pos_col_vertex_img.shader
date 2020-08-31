#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 outColor;
out vec2 outTexCoord;

uniform float offX;
uniform float offY;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0) + vec4(offX, offY, 0, 0);
//    outColor = vec3(gl_Position.x + 0.5, gl_Position.y + 0.5, gl_Position.z + 0.5);
    outColor = aColor;
    outTexCoord = aTexCoord;
}