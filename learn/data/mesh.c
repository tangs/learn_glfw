//
// Created by tangs on 2020/9/5.
//

#include "mesh.h"

#include <string.h>

#include <assimp/cimport.h>

#include "../shader.h"

void mesh_setup(struct Mesh *mesh) {
    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);

    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices->len * sizeof(struct Vertex),
                 mesh->vertices->data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices->len * sizeof(GLuint),
                 mesh->indices->data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
            (void*)offsetof(struct Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
            (void*)offsetof(struct Vertex, tex_coords));
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(struct VertexBoneData),
            (void*)offsetof(struct Vertex, bone_data));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(struct VertexBoneData),
            (void*)offsetof(struct Vertex, bone_data) + offsetof(struct VertexBoneData, weights));
//    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
//                          (void*)offsetof(struct Vertex, tangent));
//    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
//                          (void*)offsetof(struct Vertex, bitangent));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

void mesh_init(struct Mesh *mesh, size_t num_vertexes, size_t num_indices, size_t num_textures) {
    memset(mesh, 0, sizeof(*mesh));
    mesh->vertices = g_array_sized_new(FALSE, FALSE, sizeof(struct Vertex), num_vertexes);
    mesh->indices = g_array_sized_new(FALSE, FALSE, sizeof(GLuint), num_indices);
    mesh->textures = g_array_sized_new(FALSE, FALSE, sizeof(struct Texture), num_textures);
//    mesh_setup(mesh);
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
    GLuint diffuse_nr   = 1;
    GLuint specular_nr  = 1;
    GLuint normal_nr    = 1;
    GLuint height_nr    = 1;
    char material[256];
    for (size_t i = 0; i < mesh->textures->len; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        struct Texture *texture = &g_array_index(mesh->textures, struct Texture, i);
        const char *type = texture->type;
        GLuint number;
        if (!strcmp(type, "texture_diffuse"))
            number = diffuse_nr++;
        else if (!strcmp(type, "texture_specular"))
            number = specular_nr++;
        else if(!strcmp(type, "texture_normal"))
            number = normal_nr++; // transfer unsigned int to stream
        else if(!strcmp(type, "texture_height"))
            number = height_nr++;
        else
            continue;
        snprintf(material, sizeof(material), "%s%d", type, number);
        shader_set_int(shader, material, i);
        glBindTexture(GL_TEXTURE_2D, texture->id);
    }

    glBindVertexArray(mesh->vao);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glDrawElements(GL_TRIANGLES, mesh->indices->len, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}
