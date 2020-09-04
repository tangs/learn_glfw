//
// Created by tangs on 2020/9/3.
//

#ifndef GLFW_TEXTURE2D_H
#define GLFW_TEXTURE2D_H

#include <stdbool.h>

#include <glib.h>
#include <glad/glad.h>

static GHashTable *cache;

struct Texture2d {
    GLuint id;
    int ref;
    const char* path;
};

void texture2d_cache_init();

struct Texture2d* texture_create(const char *path);
void texture_retain(struct Texture2d* texture2d);
void texture_release(struct Texture2d* texture2d);

#endif //GLFW_TEXTURE2D_H
