//
// Created by tangs on 2020/9/5.
//

#include "mesh.h"

#include <string.h>

#include <assimp/cimport.h>

#include "../shader.h"

static void mesh_setup(struct Mesh *mesh) {
    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);

    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

    glBufferData(GL_ARRAY_BUFFER, g_array_get_element_size(mesh->vertices) * sizeof(*mesh->vertices), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_array_get_element_size(mesh->indices) * sizeof(*mesh->indices), mesh->indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*mesh->vertices),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(*mesh->vertices),
                          (void*)offsetof(struct Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(*mesh->vertices),
                          (void*)offsetof(struct Vertex, tex_coords));

    glBindVertexArray(0);
}

void mesh_init(struct Mesh *mesh, size_t num_vertexes, size_t num_indices, size_t num_textures) {
    memset(mesh, 0, sizeof(*mesh));
    mesh->vertices = g_array_sized_new(FALSE, FALSE, sizeof(struct Vertex), num_vertexes);
    // TODO
    mesh->indices = g_array_sized_new(FALSE, FALSE, sizeof(struct Texture), num_indices);
    mesh->textures = g_array_sized_new(FALSE, FALSE, sizeof(struct Texture), num_textures);
    mesh_setup(mesh);
}

struct Mesh* mesh_create(size_t num_vertexes, size_t num_indices, size_t num_textures) {
    struct Mesh *mesh = malloc(sizeof(struct Mesh));
    mesh_init(mesh, num_vertexes, num_indices, num_textures);
    return mesh;
}

void mesh_free(struct Mesh *mesh) {
    assert(mesh);
    if (!mesh) return;
    g_array_free(mesh->vertices, true);
    g_array_free(mesh->indices, true);
    g_array_free(mesh->textures, true);
    free(mesh);
}

void mesh_draw(struct Mesh *mesh, struct Shader shader) {
    GLuint diffuse_nr = 1;
    GLuint specular_nr = 1;
    char material[256];
    for (size_t i = 0; i < g_array_get_element_size(mesh->textures); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
//        const char *type = ((struct Texture*)(mesh->textures[i].data))->type;
        struct Texture *texture = &g_array_index(mesh->textures, struct Texture, i);
        const char *type = texture->type;
        GLuint number;
        if (!strcmp(type, "texture_diffuse"))
            number = diffuse_nr++;
        else if (!strcmp(type, "texture_specular"))
            number = specular_nr++;
        else
            continue;
        snprintf(material, sizeof(material), "material.%s%d", type, number);
        shader_set_float(shader, material, i);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, g_array_get_element_size(mesh->indices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
