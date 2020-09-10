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
//    printf("path:%s, %d\n", path, texture_id);
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
    mat4 matrix;
    glm_mat4_identity(matrix);
    model_process_node(mode, scene->mRootNode, scene);

    struct aiBone **bones = scene->mMeshes[0]->mBones;
    for (int i = 0; i < scene->mMeshes[0]->mNumBones; ++i) {
        struct aiBone *bone = bones[i];
        printf("bone:%s\n", bone->mName.data);
    }

    return true;
}

void model_process_node(struct Model *model, struct aiNode *node, const struct aiScene *scene) {
//    node->mTransformation
    printf("node:%s\n", node->mName.data);
    for (size_t i = 0; i < node->mNumMeshes; ++i) {
        struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        model->mesh_head = g_slist_append(model->mesh_head, model_process_mesh(model, node, mesh, scene));
    }
    for (size_t i = 0; i < node->mNumChildren; ++i) {
        model_process_node(model, node->mChildren[i], scene);
    }
}

static void to_mat4(float* src, mat4 dest) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            dest[i][j] = src[j * 4 + i];
        }
    }
}

struct Mesh* update_mesh(struct Mesh *dest_mesh, struct aiMesh *mesh, const struct aiScene *scene,
        int anim_idx, int frame) {
    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        mat4 matrix;
        glm_mat4_identity(matrix);

        struct Vertex *vertex = &g_array_index(dest_mesh->vertices, struct Vertex, i);

        struct aiAnimation *anim = scene->mAnimations[anim_idx];
        struct aiBone **bones = mesh->mBones;

//        glm_translate(matrix, (float*)&mesh->mVertices[i]);

        int bone_idx = 0;
        for (int j = 0; j < anim->mNumChannels; ++j) {
            struct aiNodeAnim *channel = anim->mChannels[j];
            for (int k = 0; k < mesh->mNumBones; ++k) {
                struct aiBone *bone = bones[k];
                if (!strcmp(channel->mNodeName.data, bone->mName.data)) {
                    for (int l = 0; l < bone->mNumWeights; ++l) {
                        if (bone->mWeights[l].mVertexId == i) {
                            printf("%zu,%d,%f,%s\n", i, bone_idx, bone->mWeights[l].mWeight, bone->mName.data);
                            vertex->bone_data.ids[bone_idx] = k;
                            vertex->bone_data.weights[bone_idx++] = bone->mWeights[l].mWeight;
                            vec3 scale, rotation, translation;
                            glm_vec3_copy((float*)&(channel->mScalingKeys[frame].mValue), scale);
                            glm_vec3_copy((float*)&(channel->mRotationKeys[frame].mValue.x), rotation);
                            glm_vec3_copy((float*)&(channel->mPositionKeys[frame].mValue), translation);
                            glm_vec3_scale(scale, bone->mWeights[l].mWeight, scale);
                            glm_vec3_scale(rotation, bone->mWeights[l].mWeight, rotation);
                            glm_vec3_scale(translation, bone->mWeights[l].mWeight, translation);
                            mat4 matrix1;
                            glm_mat4_identity(matrix1);

                            glm_translate(matrix1, translation);

//                            glm_rotate_x(matrix1, glm_rad(360 * rotation[0]), matrix1);
//                            glm_rotate_y(matrix1, glm_rad(360 * rotation[1]), matrix1);
//                            glm_rotate_z(matrix1, glm_rad(360 * rotation[2]), matrix1);
                            glm_rotate(matrix1, glm_rad(360 * rotation[0]), (vec3){1.0, 0.0, 0.0});
                            glm_rotate(matrix1, glm_rad(360 * rotation[1]), (vec3){0.0, 1.0, 0.0});
                            glm_rotate(matrix1, glm_rad(360 * rotation[2]), (vec3){0.0, 0.0, 1.0});
                            glm_scale(matrix1, scale);

                            glm_mat4_mul(matrix, matrix1, matrix);
                        }
                    }
                }
            }
        }
        vec4 pos;
        glm_vec3_copy((float*)&mesh->mVertices[i], pos);
        pos[3] = 0;

        glm_mat4_mulv(matrix, pos, pos);
//        glm_vec3_copy(matrix[3], pos);

//        glm_vec3_copy((float*)&mesh->mVertices[i], vertex.position);
        glm_vec3_copy(pos, vertex->position);

        if (mesh->mNormals) {
            glm_vec3_copy((float*)&mesh->mNormals[i], vertex->normal);
        }
        if (mesh->mTextureCoords[0]) {
            glm_vec2_copy((float*)&mesh->mTextureCoords[0][i], vertex->tex_coords);
        }
    }
    mesh_setup(dest_mesh);
}

struct Mesh* model_process_mesh(struct Model *model, struct aiNode *node, struct aiMesh *mesh,
        const struct aiScene *scene) {
    struct Mesh *dest_mesh = mesh_create(mesh->mNumVertices, 0, 0);
    dest_mesh->bones = g_array_sized_new(FALSE, FALSE, sizeof(struct Bone), mesh->mNumBones);
    for (size_t i = 0; i < mesh->mNumVertices; i++) {

        struct Vertex vertex;
        memset(&vertex, 0, sizeof(vertex));

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
//    mesh_setup(dest_mesh);
    model->dest_mesh = dest_mesh;
    model->mesh = mesh;
    model->scene = scene;
    update_mesh(dest_mesh, mesh, scene, 2, 2);
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
//        printf("size:%zu\n", size);
    }
//    return textures[0];
}

struct Mesh* model_anim(struct Model *model, int anim_idx, int frame) {
    update_mesh(model->dest_mesh, model->mesh, model->scene, anim_idx, frame);
}
