//
// Created by tangs on 2020/8/27.
//

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "utils.h"

static int frame = 0;

static GLuint vertex_shader = 0;
static GLuint frag_shader = 0;
static GLuint program = 0;

static GLuint vbo = 0;
static GLuint vao = 0;
static GLuint ebo = 0;

static GLuint texture = 0;
static GLuint texture2 = 0;

static float vertices[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
    0.1f,  0.1f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
    0.1f, -0.1f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
    -0.1f, -0.1f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
    -0.1f,  0.1f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 左上
};

unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
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
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

static void bind_texture(GLuint *texture, const char *path) {
    int width, height, nr_channels;
    unsigned char *data = stbi_load(path, &width, &height, &nr_channels, 0);

    if (!data) {
        fprintf(stderr, "load image fail.");
        return;
    }
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

static void init_image_data() {
    stbi_set_flip_vertically_on_load(1);
    bind_texture(&texture, "p1tank.png");
    bind_texture(&texture2, "awesomface.png");
}

static mat4 trans;
float angle = 0.0f;

static void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    bool change_angle = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        off_x -= 0.01;
        GLint location = glGetUniformLocation(program, "offX");
        glUniform1f(location, off_x);
        angle = 90.0f;
        change_angle = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        off_x += 0.01;
        GLint location = glGetUniformLocation(program, "offX");
        glUniform1f(location, off_x);
        angle = 270.0f;
        change_angle = true;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        off_y += 0.01;
        GLint location = glGetUniformLocation(program, "offY");
        glUniform1f(location, off_y);
        angle = 0.0f;
        change_angle = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        off_y -= 0.01;
        GLint location = glGetUniformLocation(program, "offY");
        glUniform1f(location, off_y);
        angle = 180.0f;
        change_angle = true;
    }
    glUniform1f(glGetUniformLocation(program, "mixValue"), (off_x + 1.0) / 2);

    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){0.5f, -0.5f, 0.0f});
    glm_rotate(trans, (float)glfwGetTime(), (vec3){0.0f, 0.0f, 1.0f});
    float scale = sin(glfwGetTime());
    glm_scale(trans, (vec3){scale, scale, scale});
    glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, trans);
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glUniform1i(glGetUniformLocation(program, "texture1"), 0);
    glUniform1i(glGetUniformLocation(program, "texture2"), 1);
//    glBindVertexArray(vao);
//    glBindVertexArray(ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, trans);

//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    double time = glfwGetTime();
//    float offX = sin(time);
//    glUniform1f(glGetUniformLocation(program, "offX"), offX);
}

int main() {
//    vec4 vec = {1.0f, 0.f, 0.0f, 1.0f};
//    glm_mat4_identity(trans);
//    glm_translate(trans, (vec3){1.0f, 1.0f, 0.0f});
//    glm_mat4_mulv(trans, vec, vec);
//    printf("%f %f %f\n", vec[0], vec[1], vec[2]);
    glm_mat4_identity(trans);
    glm_rotate(trans, glm_rad(0.0f), (vec3){0.0f, 0.0f, 1.0f});
    glm_scale(trans, (vec3){0.5f, 0.5f, 0.5f});

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) exit(EXIT_FAILURE);
//    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnOpenGL", NULL, NULL);
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
    vertex_shader = compile_shader("../Shaders/pos_col_vertex_img.shader", GL_VERTEX_SHADER);
    if (!vertex_shader) {
        return -3;
    }
    frag_shader = compile_shader("../Shaders/pos_col_frag_img.shader", GL_FRAGMENT_SHADER);
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

    glUniform1f(glGetUniformLocation(program, "mixValue"), 0.5);

//    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while(!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.91f, 0.91f, 0.91f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
        ++frame;
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwTerminate();
    return 0;
}

