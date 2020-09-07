//
// Created by tangs on 2020/9/5.
//

#ifndef GLFW_MESH_H
#define GLFW_MESH_H

#include <glib.h>

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
    GArray *vertices;
    GArray *indices;
    GArray *textures;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

void mesh_init(struct Mesh *mesh);
struct Mesh* mesh_create();
void mesh_free(struct Mesh *mesh);
void mesh_draw(struct Mesh *mesh, struct Shader shader);

#endif //GLFW_MESH_H
