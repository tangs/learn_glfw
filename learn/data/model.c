//
// Created by tangs on 2020/9/5.
//

#include "model.h"

#include <string.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "../utils/image_loader.h"

static GLuint texture_fromfile(const char *filename, const char* directory, bool gamma) {
    //string filename = string(path);
    //filename = directory + '/' + filename;
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", directory, filename);
    GLuint texture_id = 0;

    int width, height, nrComponents;
    unsigned char *data = image_loader_load(path, &width, &height, &nrComponents, 0);
//    unsigned char *data = stbi_load(path, &width, &height, &nr_channels, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        image_loader_image_free(data);
    } else {
        fprintf(stderr, "Texture failed to load at path: %s\n", path);
        image_loader_image_free(data);
    }
    return texture_id;
}

void model_init(struct Model *model) {
    memset(model, 0, sizeof(*model));
//    model->mesh_head = g_list_alloc();
//    model->meshes = g_array_new(FALSE, TRUE, 128);
}

void model_delete(struct Model *model) {
    assert(model);
    if (!model) return;
//    g_list_free(model->mesh_head);
//    g_array_free(model->meshes, true);
    if (model->mesh_head) g_slist_free(model->mesh_head);
    // TODO free meshes
    if (model->directory) free(model->directory);
}

void model_draw(struct Model *model, struct Shader shader) {
    for (GSList *node = model->mesh_head; node; node = node->next) {
        mesh_draw(node->data, shader);
    }
}

bool model_load(struct Model *mode, const char *path) {
    assert(path);
    if (!path) {
        fprintf(stderr, "path is null.\n");
        return false;
    }
    const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate |
            aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
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
        model->mesh_head = g_slist_append(model->mesh_head, model_process_mesh(model, mesh, scene));
//        g_array_append_val(model->meshes, mesh);
    }
    for (size_t i = 0; i < node->mNumChildren; ++i) {
        model_process_node(model, node->mChildren[i], scene);
    }
}

struct Mesh* model_process_mesh(struct Model *model, struct aiMesh *mesh, const struct aiScene *scene) {
//    size_t num_indices = 0;
//    size_t num_textures = 0;
//    for(size_t i = 0; i < mesh->mNumFaces; i++) {
//        num_indices += mesh->mFaces[i].mNumIndices;
//    }
//    if (mesh->mMaterialIndex >= 0) {
//        struct aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
//        num_textures += aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE);
//        num_textures += aiGetMaterialTextureCount(material, aiTextureType_SPECULAR);
//        num_textures += aiGetMaterialTextureCount(material, aiTextureType_NORMALS);
//        num_textures += aiGetMaterialTextureCount(material, aiTextureType_HEIGHT);
//    }
    struct Mesh *dest_mesh = mesh_create(mesh->mNumVertices, 0, 0);
    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        struct Vertex vertex;
        memset(&vertex, 0, sizeof(vertex));

        glm_vec3_copy(&mesh->mVertices[i], vertex.position);

        if (mesh->mNormals) {
            glm_vec3_copy(&mesh->mNormals[i], vertex.normal);
        }
        if (mesh->mTextureCoords[0]) {
            glm_vec2_copy(&mesh->mTextureCoords[0][i], vertex.tex_coords);

            glm_vec3_copy(&mesh->mTangents[i], vertex.tangent);
            glm_vec3_copy(&mesh->mBitangents[i], vertex.bitangent);
        }
        g_array_append_val(dest_mesh->vertices, vertex);
    }
    for (size_t i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];
        g_array_append_vals(dest_mesh->indices, face.mIndices, face.mNumIndices);
    }
    // TODO 处理索引
    if (mesh->mMaterialIndex >= 0) {
        struct aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        loadMaterialTextures(model, material, aiTextureType_DIFFUSE,
                             "texture_diffuse", dest_mesh->textures);
        loadMaterialTextures(model, material, aiTextureType_SPECULAR,
                             "texture_specular", dest_mesh->textures);
        loadMaterialTextures(model, material, aiTextureType_NORMALS,
                             "texture_normal", dest_mesh->textures);
        loadMaterialTextures(model, material, aiTextureType_HEIGHT,
                             "texture_height", dest_mesh->textures);
    }
    mesh_setup(dest_mesh);
    return dest_mesh;
}

void loadMaterialTextures(struct Model *model, struct aiMaterial *mat, enum aiTextureType type,
                          const char* type_name, GArray* textures) {
    size_t cnt = aiGetMaterialTextureCount(mat, type);
    for (size_t i = 0; i < cnt; i++) {
        struct aiString str;
        aiGetMaterialTexture(mat, type, i, &str, NULL, NULL,
                             NULL, NULL, NULL, NULL);
        struct Texture texture;
        texture.id = texture_fromfile(str.data, model->directory, false);
        texture.type = strdup(type_name);
        texture.path = strdup(str.data);
//        textures[i] = texture;
        g_array_append_val(textures, texture);
        size_t size = textures->len;
        printf("size:%zu\n", size);
    }
//    return textures[0];
}
