//
// Created by tangs on 2020/9/4.
//

#ifndef GLFW_IMAGE_H
#define GLFW_IMAGE_H

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

struct Image {
    GLuint texture;
};

const char* Image_COMP_KEY = "Image";

#endif //GLFW_IMAGE_H
