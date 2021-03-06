//
// Created by tangs on 2020/9/5.
//

#ifndef GLFW_MESH_H
#define GLFW_MESH_H

#include <glib.h>

#include <cglm/cglm.h>
#include <glad/glad.h>

#define NUM_BONES_PER_VEREX 4

struct Shader;

struct VertexBoneData {
    uint ids[NUM_BONES_PER_VEREX];
    float weights[NUM_BONES_PER_VEREX];
};

struct Bone {

};

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 tex_coords;
    struct VertexBoneData bone_data;
//    vec3 tangent;
//    vec3 bitangent;
};

struct Texture {
    GLuint id;
    const char *type;
    const char *path;
};

struct Mesh {
    GArray *vertices;
    GArray *indices;
    GArray *textures;
    GArray *bones;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};
void mesh_setup(struct Mesh *mesh);
void mesh_init(struct Mesh *mesh, size_t num_vertexes, size_t num_indices, size_t num_textures);
struct Mesh* mesh_create(size_t num_vertexes, size_t num_indices, size_t num_textures);
void mesh_free(struct Mesh *mesh);
void mesh_draw(struct Mesh *mesh, struct Shader shader);

#endif //GLFW_MESH_H
