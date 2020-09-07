//
// Created by tangs on 2020/9/5.
//

#ifndef GLFW_MODEL_H
#define GLFW_MODEL_H

#include <assimp/material.h>

#include <glib.h>

#include "mesh.h"
#include "../shader.h"

struct aiNode;
struct aiMesh;
struct aiScene;
enum aiTextureType;

struct Model {
    GList *mesh_head;
    size_t meshes_len;
    char *directory;
};

void model_init(struct Model *model);
void model_delete(struct Model *model);
void model_draw(struct Model *model, struct Shader shader);
bool model_load(struct Model *model, const char *path);
void model_process_node(struct Model *model, struct aiNode *node, const struct aiScene *scene);
struct Mesh* model_process_mesh(struct Model *model, struct aiMesh *mesh, struct aiScene *scene);
struct Texture* loadMaterialTextures(struct Model *model, struct aiMaterial *mat, enum aiTextureType type,
        const char* type_name, size_t *len);

#endif //GLFW_MODEL_H
