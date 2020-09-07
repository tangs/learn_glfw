//
// Created by tangs on 2020/9/5.
//

#include "model.h"

#include <string.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>

static GLuint texture_fromfile(const char *filename, const char* directory, bool gamma) {
    //string filename = string(path);
    //filename = directory + '/' + filename;
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", directory, filename);
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
        format = GL_RED;
        else if (nrComponents == 3)
        format = GL_RGB;
        else if (nrComponents == 4)
        format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
//        std::cout << "Texture failed to load at path: " << path << std::endl;
        fprintf(stderr, "Texture failed to load at path: %s\n", path);
        stbi_image_free(data);
    }
    return textureID;
}

void model_init(struct Model *model) {
    memset(model, 0, sizeof(*model));
//    model->mesh_head = g_list_alloc();
    model->meshes = g_array_new(FALSE, TRUE, 128);
}

void model_delete(struct Model *model) {
    assert(model);
    if (!model) return;
//    g_list_free(model->mesh_head);
    g_array_free(model->meshes, true);
    if (model->directory) free(model->directory);
}

void model_draw(struct Model *model, struct Shader shader) {
//    for (size_t i = 0; i < model->meshes_len; ++i)
//        mesh_draw(&model->meshes[i], shader);
//    for (GList *node = model->mesh_head->next; node->data; node = node->next) {
//        mesh_draw(node->data, shader);
//    }
    size_t len = g_array_get_element_size(model->meshes);
    for (size_t i = 0; i < len; ++i) {
        struct Mesh *mesh = g_array_index(model->meshes, struct Mesh*, i);
        mesh_draw(mesh, shader);
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
//        g_list_append(model->mesh_head, mesh);
        g_array_append_val(model->meshes, mesh);
    }
    for (size_t i = 0; i < node->mNumChildren; ++i) {
        model_process_node(model, node->mChildren[i], scene);
    }
}

struct Mesh* model_process_mesh(struct Model *model, struct aiMesh *mesh, struct aiScene *scene) {
    struct Mesh *ret = mesh_create(scene->mNumMeshes, 0, 0);
    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        struct aiVector3D *mesh_ver = &mesh->mVertices[i];
        struct aiVector3D *mesh_nor = &mesh->mNormals[i];
        struct Vertex *vertex = &g_array_index(ret->vertices, struct Vertex, i);
        vec3 *pos = vertex->position;
        vec3 *nor = vertex->normal;
        vec2 *coords = vertex->tex_coords;
        *pos[0] = mesh_ver->x;
        *pos[1] = mesh_ver->y;
        *pos[2] = mesh_ver->z;

        *nor[0] = mesh_nor->x;
        *nor[1] = mesh_nor->y;
        *nor[2] = mesh_nor->z;
        if (mesh->mTextureCoords[0]) {
            *coords[0] = mesh->mTextureCoords[0][i].x;
            *coords[1] = mesh->mTextureCoords[0][i].y;
        } else {
            *coords[0] = *coords[1] = 0.0f;
        }

    }
    for(size_t i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];
        g_array_append_vals(ret->indices, face.mIndices, face.mNumIndices);
    }
    // TODO 处理索引
    if (mesh->mMaterialIndex >= 0) {
        struct aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        size_t len;
        struct Texture* diffuse_maps = loadMaterialTextures(model, material, aiTextureType_DIFFUSE,
                "texture_diffuse", &len);
        g_array_append_vals(ret->textures, diffuse_maps, len);
        struct Texture* specular_maps = loadMaterialTextures(model, material, aiTextureType_SPECULAR,
                "texture_specular", &len);
        g_array_append_vals(ret->textures, specular_maps, len);
        free(diffuse_maps);
        free(specular_maps);
    }
    return ret;
}

struct Texture* loadMaterialTextures(struct Model *model, struct aiMaterial *mat, enum aiTextureType type,
                                     const char* type_name, size_t *len) {
    size_t cnt = aiGetMaterialTextureCount(mat, type);
    len = cnt;
    if (cnt <= 0) return NULL;
    struct Texture **textures = mat = malloc(sizeof(struct Texture *) * cnt);
    for(size_t i = 0; i < cnt; i++) {
        struct aiString str;
//        mat->GetTexture(type, i, &str);
        aiGetMaterialTexture(mat, type, i, &str, NULL, NULL,
                             NULL, NULL, NULL, NULL);
        struct Texture *texture = malloc(sizeof(struct Texture));
        texture->id = texture_fromfile(str.data, model->directory, false);
        texture->type = strdup(type_name);
        texture->path = strdup(str.data);
        textures[i] = texture;
    }
    return textures[0];
}
