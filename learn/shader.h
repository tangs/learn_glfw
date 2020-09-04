//
// Created by tangs on 2020/9/3.
//

#ifndef GLFW_SHADER_H
#define GLFW_SHADER_H

#include <stdbool.h>

#include <glad/glad.h>
#include <cglm/cglm.h>

struct Shader {
    GLuint id;
};

// ret: 0 is success.
int shader_init(struct Shader* shader, const char *vertex_path, const char *fragment_path);
void shader_delete(struct Shader shader);
void shader_use(struct Shader shader);
void shader_set_bool(struct Shader shader, const char *name, bool value);
void shader_set_int(struct Shader shader, const char *name, int value);
void shader_set_float(struct Shader shader, const char *name, float value);
void shader_set_vec3(struct Shader shader, const char *name, vec3 value);
void shader_set_vec4(struct Shader shader, const char *name, vec4 value);
void shader_set_matrix(struct Shader shader, const char *name, mat4 value);

#endif //GLFW_SHADER_H
