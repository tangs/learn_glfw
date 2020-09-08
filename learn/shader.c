//
// Created by tangs on 2020/9/3.
//

#include "shader.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"

enum ErrType {
    COMPILE,
    LINK,
};

static int check_errors(unsigned int shader, enum ErrType type) {
    int success;
    char info_log[1024];
    if (type == COMPILE) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
            fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR of type: %d\n%s\n", type, info_log);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, sizeof(info_log), NULL, info_log);
            fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR of type %d\n%s\n", type, info_log);
        }
    }
    return success;
}

static GLuint compile_shader(const char *path, GLenum type) {
    char tmp[1024];
    int ret_code = read_file(path, tmp, sizeof(tmp));
    if (ret_code) {
        return 0;
    }
    const char* str = tmp;
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &str, NULL);
    glCompileShader(shader);
    if (!check_errors(shader, COMPILE)) {
        return 0;
    }
    return shader;
}

int shader_init(struct Shader* shader, const char *vertex_path, const char *fragment_path) {
    memset(shader, 0, sizeof(struct Shader));
    GLuint vertex_shader = compile_shader(vertex_path, GL_VERTEX_SHADER);
    if (!vertex_shader) {
        return 1;
    }
    GLuint frag_shader = compile_shader(fragment_path, GL_FRAGMENT_SHADER);
    if (!frag_shader) {
        return 2;
    }

    GLuint id = glCreateProgram();
    glAttachShader(id, vertex_shader);
    glAttachShader(id, frag_shader);
    glLinkProgram(id);
    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);
    if (!check_errors(id, LINK)) {
        return 3;
    }

    shader->id = id;
    return 0;
}

void shader_delete(struct Shader shader) {
    glDeleteShader(shader.id);
}

void shader_use(struct Shader shader) {
    glUseProgram(shader.id);
}

void shader_set_bool(struct Shader shader, const char *name, bool value) {
    glUniform1i(glGetUniformLocation(shader.id, name), value);
}

void shader_set_int(struct Shader shader, const char *name, int value) {
    glUniform1i(glGetUniformLocation(shader.id, name), value);
}

void shader_set_float(struct Shader shader, const char *name, float value) {
    glUniform1f(glGetUniformLocation(shader.id, name), value);
}

void shader_set_vec3(struct Shader shader, const char *name, vec3 value) {
    glUniform3f(glGetUniformLocation(shader.id, name), value[0], value[1], value[2]);
}

void shader_set_vec4(struct Shader shader, const char *name, vec4 value) {
    glUniform4f(glGetUniformLocation(shader.id, name), value[0], value[1], value[2], value[3]);
}

void shader_set_matrix(struct Shader shader, const char *name, mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(shader.id, name), 1, GL_FALSE, (const GLfloat*)value);
}