//
// Created by tangs on 2020/9/5.
//

#include "mesh.h"

#include <string.h>

#include "../shader.h"

static void mesh_setup(struct Mesh *mesh) {
    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);

    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

    glBufferData(GL_ARRAY_BUFFER, mesh->vertices_len * sizeof(*mesh->vertices), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_len * sizeof(*mesh->indices), mesh->indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*mesh->vertices),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(*mesh->vertices),
                          (void*)offsetof(struct Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(*mesh->vertices),
                          (void*)offsetof(struct Vertex, tex_coords));

    glBindVertexArray(0);
}

void mesh_init(struct Mesh *mesh, struct Vertex *vertices, size_t vertices_len, GLuint *indices,
               size_t indices_len, struct Texture *textures, size_t textures_len) {
    memset(mesh, sizeof(*mesh), 0);
    mesh->vertices = vertices;
    mesh->vertices_len = vertices_len;
    mesh->indices = indices;
    mesh->indices_len = indices_len;
    mesh->textures = textures;
    mesh->textures_len = textures_len;
    mesh_setup(mesh);
}

void mesh_draw(struct Mesh *mesh, struct Shader *shader) {
    GLuint diffuse_nr = 1;
    GLuint specular_nr = 1;
    char material[256];
    for (size_t i = 0; i < mesh->textures_len; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        const char *type = mesh->textures[i].type;
        GLuint number;
        if (!strcmp(type, "texture_diffuse"))
            number = diffuse_nr++;
        else if (!strcmp(type, "texture_specular"))
            number = specular_nr++;
        else
            continue;
        snprintf(material, sizeof(material), "material.%s%b", type, number);
        shader_set_float(*shader, material, i);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indices_len, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
