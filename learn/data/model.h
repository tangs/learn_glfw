//
// Created by tangs on 2020/9/5.
//

#ifndef GLFW_MODEL_H
#define GLFW_MODEL_H

#include "mesh.h"

struct Model {
    struct Mesh *meshes;
    size_t meshes_len;
    char *directory;
};

void model_init(struct Model *mode);

#endif //GLFW_MODEL_H
