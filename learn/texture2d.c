//
// Created by tangs on 2020/9/3.
//

#include "texture2d.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>

#include "utils/image_loader.h"

void texture2d_cache_init() {
    cache = g_hash_table_new(g_str_hash, g_str_equal);
//    stbi_set_flip_vertically_on_load(1);
    image_loader_init();
}

struct Texture2d* texture_create(const char *path) {
    assert(path);
    gpointer key, value;
    gboolean ret = g_hash_table_lookup_extended(cache, path, &key, &value);
    if (ret == TRUE) {
        return (struct Texture2d*)value;
    }
    int width, height, nr_channels;
    unsigned char *data = image_loader_load(path, &width, &height, &nr_channels, 0);

    if (!data) {
        fprintf(stderr, "load image fail.");
        return NULL;
    }
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    struct Texture2d *texture2d = malloc(sizeof(struct Texture2d));
    image_loader_image_free(data);
    texture2d->id = texture;
    texture2d->ref = 0;
    texture2d->path = strdup(path);

    g_hash_table_insert(cache, strdup(path), (gpointer)&texture2d);
    return texture2d;
}

void texture_retain(struct Texture2d* texture2d) {
    assert(texture2d);
    texture2d->ref++;
}

void texture_release(struct Texture2d* texture2d) {
    assert(texture2d);
    if (--texture2d->ref <= 0) {
        gpointer key, value;
        assert(texture2d->path);
        gboolean ret = g_hash_table_lookup_extended(cache, texture2d->path, &key, &value);
        if (ret == TRUE) {
            g_hash_table_remove(cache, key);
            free(value);
            free(key);
        }
    }
}