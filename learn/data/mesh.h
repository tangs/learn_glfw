//
// Created by tangs on 2020/9/5.
//

#ifndef GLFW_MESH_H
#define GLFW_MESH_H

#include <cglm/cglm.h>
#include <glad/glad.h>

struct Shader;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 tex_coords;
};

struct Texture {
    GLuint id;
    char *type;
};

struct Mesh {
    struct Vertex *vertices;
    size_t vertices_len;
    GLuint *indices;
    size_t indices_len;
    struct Texture *textures;
    size_t textures_len;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

void mesh_init(struct Mesh *mesh, struct Vertex *vertices, size_t vertices_len, GLuint *indices,
               size_t indices_len, struct Texture *textures, size_t textures_len);
void mesh_draw(struct Mesh *mesh, struct Shader *shader);

#endif //GLFW_MESH_H
