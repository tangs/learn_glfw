#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];

void main() {
    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];

    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
//    gl_Position = projection * view * model * (BoneTransform * vec4(aPos, 1.0));
    vec4 NormalL = vec4(aNormal, 0.0);
    Normal = vec3( (model * NormalL));
    Normal = vec3(BoneTransform * (model * NormalL));
//    Normal = BoneTransform * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
}