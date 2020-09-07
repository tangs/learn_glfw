//
// Created by tangs on 2020/9/5.
//

#include "model.h"

#include <string.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

void model_init(struct Model *model) {
    memset(model, 0, sizeof(*model));
    model->mesh_head = g_list_alloc();
}

void model_delete(struct Model *model) {
    assert(model);
    if (!model) return;
    g_list_free(model->mesh_head);
    if (model->directory) free(model->directory);
}

void model_draw(struct Model *model, struct Shader shader) {
//    for (size_t i = 0; i < model->meshes_len; ++i)
//        mesh_draw(&model->meshes[i], shader);
    for (GList *node = model->mesh_head; node; node = node->next) {
        mesh_draw(node->data, shader);
    }
}

bool model_load(struct Model *mode, const char *path) {
    assert(path);
    if (!path) {
        fprintf(stderr, "path is null.\n");
        return false;
    }
    const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        fprintf(stderr, "load mode err:%s\n", aiGetErrorString());
        return false;
    }
    mode->directory = strdup(path);
    char *idx = strrchr(mode->directory, '/');
    if (!idx) {
        free(mode->directory);
        return false;
    }
    *idx = 0;
    model_process_node(mode, scene->mRootNode, scene);
    return true;
}


void model_process_node(struct Model *model, struct aiNode *node, const struct aiScene *scene) {
    for (size_t i = 0; i < node->mNumMeshes; ++i) {
        struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        g_list_append(model->mesh_head, mesh);
    }
    for (size_t i = 0; i < node->mNumChildren; ++i) {
        model_process_node(model, node->mChildren[i], scene);
    }
}

struct Mesh* model_process_mesh(struct Model *model, struct aiMesh *mesh, struct aiScene *scene) {
    struct Mesh *ret = mesh_create();
    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        struct Vertex *vertex = malloc(sizeof(struct Vertex));
        // TODO
//        g_list_append(vertices, vertex);
        g_array_append_val(ret->vertices, vertex);
    }
    // TODO 处理索引
    if (mesh->mMaterialIndex >= 0) {

    }
    return ret;
}

struct Texture* loadMaterialTextures(struct Model *model, struct aiMaterial *mat, enum aiTextureType type,
                                     const char* type_name, size_t *len) {

}
