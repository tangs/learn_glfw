//
// Created by tangs on 2020/9/4.
//

#ifndef GLFW_TRANSFORM_H
#define GLFW_TRANSFORM_H

#include <cglm/cglm.h>

struct Transform {
    vec3 position;
    vec2 size;
    vec3 rotation;
    vec3 scale;
};

const char* TRANSFORM_COMP_KEY = "Transform";

#endif //GLFW_TRANSFORM_H
