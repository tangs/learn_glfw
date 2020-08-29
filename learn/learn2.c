//
// Created by tangs on 2020/8/27.
//

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "utils.h"

static int frame = 0;

static GLuint vertex_shader = 0;
static GLuint frag_shader = 0;
static GLuint program = 0;

static GLuint vao = 0;

static GLuint texture = 0;

static float vertices[] = {
    // 位置              // 颜色
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // 右下
    0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
    0.0f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f    // 顶部
};

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("frame buffer size:%d, %d\n", width, height);
    glViewport(0, 0, width, height);
}

static float off_x = 0;
static float off_y = 0;

static void init_vao_data() {
    GLuint vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

static void init_image_data() {
    int width, height, nr_channels;
    unsigned char *data = stbi_load("p1tank.png", &width, &height, &nr_channels, 0);

    if (!data) {
        fprintf(stderr, "load image fail.");
        return;
    }
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

static void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        off_x -= 0.01;
        GLint location = glGetUniformLocation(program, "offX");
        glUniform1f(location, off_x);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        off_x += 0.01;
        GLint location = glGetUniformLocation(program, "offX");
        glUniform1f(location, off_x);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        off_y += 0.01;
        GLint location = glGetUniformLocation(program, "offY");
        glUniform1f(location, off_y);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        off_y -= 0.01;
        GLint location = glGetUniformLocation(program, "offY");
        glUniform1f(location, off_y);
    }
}

static GLuint compile_shader(const char *path, GLenum type) {
    char tmp[512];
    int ret_code = read_file(path, tmp, sizeof(tmp));
    if (ret_code) {
        return 0;
    }
    const char* str = tmp;
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &str, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED:%s\n", info_log);
        return 1;
    }
    return shader;
}

static void render(GLFWwindow *window) {
    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
//    double time = glfwGetTime();
//    float offX = sin(time);
//    glUniform1f(glGetUniformLocation(program, "offX"), offX);
}

int main() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) exit(EXIT_FAILURE);
//    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -2;
    }

    // init shader.
    vertex_shader = compile_shader("../Shaders/pos_col_vertex.shader", GL_VERTEX_SHADER);
    if (!vertex_shader) {
        return -3;
    }
    frag_shader = compile_shader("../Shaders/pos_col_frag.shader", GL_FRAGMENT_SHADER);
    if (!frag_shader) {
        return -4;
    }
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);

    // init VAO
    init_vao_data();
    init_image_data();

    while(!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
        ++frame;
    }

    glfwTerminate();
    return 0;
}

