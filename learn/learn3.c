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

static int frame_ = 0;

static const int WIN_WIDTH = 1280;
static const int WIN_HEIGHT = 720;

static GLuint vertex_shader_ = 0;
static GLuint frag_shader_ = 0;
static GLuint program_ = 0;

static GLuint vbo_ = 0;
static GLuint vao_ = 0;
static GLuint ebo_ = 0;

static GLuint texture_ = 0;
static GLuint texture2_ = 0;

static float vertices_[] = {
//     ---- 位置 ----         - 纹理坐标 -
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.333f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.333f, 0.333f,
        0.5f,  0.5f, -0.5f,  0.333f, 0.333f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.333f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.333f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.666f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.666f, 0.333f,
        0.5f,  0.5f,  0.5f,  0.666f, 0.333f,
        -0.5f,  0.5f,  0.5f,  0.333f, 0.333f,
        -0.5f, -0.5f,  0.5f,  0.333f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.333f,
        -0.5f, -0.5f, -0.5f,  0.666f, 0.333f,
        -0.5f, -0.5f, -0.5f,  0.666f, 0.333f,
        -0.5f, -0.5f,  0.5f,  0.666f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  0.333f, 0.333f,
        0.5f,  0.5f, -0.5f,  0.333f, 0.666f,
        0.5f, -0.5f, -0.5f,  0.0f, 0.666f,
        0.5f, -0.5f, -0.5f,  0.0f, 0.666f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.333f,
        0.5f,  0.5f,  0.5f,  0.333f, 0.333f,

        -0.5f, -0.5f, -0.5f,  0.333f, 0.666f,
        0.5f, -0.5f, -0.5f,  0.666f, 0.666f,
        0.5f, -0.5f,  0.5f,  0.666f, 0.333f,
        0.5f, -0.5f,  0.5f,  0.666f, 0.333f,
        -0.5f, -0.5f,  0.5f,  0.333f, 0.333f,
        -0.5f, -0.5f, -0.5f,  0.333f, 0.666f,

        -0.5f,  0.5f, -0.5f,  0.666f, 0.666f,
        0.5f,  0.5f, -0.5f,  1.0f, 0.666f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.333f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.333f,
        -0.5f,  0.5f,  0.5f,  0.666f, 0.333f,
        -0.5f,  0.5f, -0.5f,  0.666f, 0.666f,
};

unsigned int indices_[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
};

static float off_x_ = 0;
static float off_y_ = 0;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("frame buffer size:%d, %d\n", width, height);
    glViewport(0, 0, width, height);
}

static void init_vao_data() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);

    glBindVertexArray(vao_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
//    glEnableVertexAttribArray(2);
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
    bind_texture(&texture2_, "p1tank.png");
//    bind_texture(&texture_, "awesomface.png");
    bind_texture(&texture_, "cube_met.png");
}

float angle_ = 0.0f;
static mat4 trans_;
mat4 projection_mat_;
mat4 model_mat_;
//mat4 view_mat_;
vec3 camera_pos_ = (vec3){0.0f, 0.0f, 10.0f};
vec3 model_rot_ = (vec3){70.0f, 0.0f, 0.0f};

static void process_input(GLFWwindow *window) {
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//        glfwSetWindowShouldClose(window, GL_TRUE);
//    }
//
//    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
//        off_x_ -= 0.01;
//        GLint location = glGetUniformLocation(program_, "offX");
//        glUniform1f(location, off_x_);
//        angle_ = 90.0f;
//    }
//    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
//        off_x_ += 0.01;
//        GLint location = glGetUniformLocation(program_, "offX");
//        glUniform1f(location, off_x_);
//        angle_ = 270.0f;
//    }
//    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
//        off_y_ += 0.01;
//        GLint location = glGetUniformLocation(program_, "offY");
//        glUniform1f(location, off_y_);
//        angle_ = 0.0f;
//    }
//    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
//        off_y_ -= 0.01;
//        GLint location = glGetUniformLocation(program_, "offY");
//        glUniform1f(location, off_y_);
//        angle_ = 180.0f;
//    }
//    glUniform1f(glGetUniformLocation(program_, "mixValue"), (off_x_ + 1.0) / 2);
//
//    glm_mat4_identity(trans_);
//    glm_translate(trans_, (vec3){0.5f, -0.5f, 0.0f});
//    glm_rotate(trans_, (float)glfwGetTime(), (vec3){0.0f, 0.0f, 1.0f});
//    float scale = sin(glfwGetTime());
//    glm_scale(trans_, (vec3){scale, scale, scale});
//    glUniformMatrix4fv(glGetUniformLocation(program_, "transform"), 1, GL_FALSE, (const GLfloat *)trans_);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
//        glm_translate(view_mat_, (vec3){-0.01f, 0.0f, 0.0f});
        glm_vec3_add(camera_pos_, (vec3){-0.05f, 0.0f, 0.0f}, camera_pos_);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
//        glm_translate(view_mat_, (vec3){0.01f, 0.0f, 0.0f});
        glm_vec3_add(camera_pos_, (vec3){0.05f, 0.0f, 0.0f}, camera_pos_);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//        glm_translate(view_mat_, (vec3){0.0f, -0.01f, 0.0f});
        glm_vec3_add(camera_pos_, (vec3){0.0f, -0.05f, 0.0f}, camera_pos_);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
//        glm_translate(view_mat_, (vec3){0.0f, 0.01f, 0.0f});
        glm_vec3_add(camera_pos_, (vec3){0.0f, 0.05f, 0.0f}, camera_pos_);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
//        glm_translate(view_mat_, (vec3){0.0f, 0.0f, -0.05f});
        glm_vec3_add(camera_pos_, (vec3){0.0f, 0.0f, -0.05f}, camera_pos_);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
//        glm_translate(view_mat_, (vec3){0.0f, 0.0f, 0.05f});
        glm_vec3_add(camera_pos_, (vec3){0.0f, 0.0f, 0.05f}, camera_pos_);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        glm_rotate(model_mat_, glm_rad(-1.0f), (vec3){1.0f, 0.0f, 0.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        glm_rotate(model_mat_, glm_rad(1.0f), (vec3){1.0f, 0.0f, 0.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        glm_rotate(model_mat_, glm_rad(-1.0f), (vec3){0.0f, 1.0f, 0.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        glm_rotate(model_mat_, glm_rad(1.0f), (vec3){0.0f, 1.0f, 0.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        glm_rotate(model_mat_, glm_rad(-1.0f), (vec3){0.0f, 0.0f, 1.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        glm_rotate(model_mat_, glm_rad(1.0f), (vec3){0.0f, 0.0f, 1.0f});
    }
//    float radius = 10.0f;
//    camera_pos_[0] = sin(glfwGetTime()) * radius;
//    camera_pos_[2] = cos(glfwGetTime()) * radius;

    mat4 view_mat;
    glm_lookat(camera_pos_, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, view_mat);
    mat4 model_mat;
    glm_mat4_copy(model_mat_, model_mat);
    glm_translate_to(model_mat_, model_rot_, model_mat);

    glUniformMatrix4fv(glGetUniformLocation(program_, "model"), 1, GL_FALSE, (const GLfloat *)model_mat);
    glUniformMatrix4fv(glGetUniformLocation(program_, "view"), 1, GL_FALSE, (const GLfloat *)view_mat);
    glUniformMatrix4fv(glGetUniformLocation(program_, "projection"), 1, GL_FALSE, (const GLfloat *)projection_mat_);
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

static vec3 cubePositions[] = {
        { 0.0f,  0.0f,  0.0f},
        { 3.0f,  0.0f,  0.0f},
        {-3.0f,  0.0f,  0.0f},
        { 0.0f,  3.0f,  0.0f},
        { 0.0f, -3.0f,  0.0f},
        { 0.0f,  0.0f,  3.0f},
        { 0.0f,  0.0f, -3.0f},
};

static vec3 cubeRotAis[] = {
        { 0.0f,  0.0f,  0.0f},
        {-1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f, -1.0f},
};

static void render() {
    glUseProgram(program_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2_);
    glUniform1i(glGetUniformLocation(program_, "texture1"), 0);
    glUniform1i(glGetUniformLocation(program_, "texture2"), 1);
//    glBindVertexArray(vao);
//    glBindVertexArray(ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); ++i) {
        mat4 model_mat, model_rot;
        glm_translate_to(model_mat_, cubePositions[i], model_mat);
        if (i) glm_rotate(model_mat, glm_rad((int)(glfwGetTime() * 250) % 360), cubeRotAis[i]);
        glUniformMatrix4fv(glGetUniformLocation(program_, "model"), 1, GL_FALSE, (const GLfloat *)model_mat);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
//    glm_ortho()
    glm_mat4_identity(projection_mat_);
    glm_mat4_identity(model_mat_);
//    glm_mat4_identity(view_mat_);

    glm_perspective(glm_rad(45.0f), (float)WIN_WIDTH / WIN_HEIGHT,
                    0.1f, 100.0f, projection_mat_);
    glm_rotate(model_mat_, glm_rad(-55.0f), (vec3){1.0f, 0.0f, 0.0f});
//    glm_translate(view_mat_, (vec3){0.0f, 0.0f, -20.0f});

//    glm_mat4_identity(trans_);
//    glm_rotate(trans_, glm_rad(0.0f), (vec3){0.0f, 0.0f, 1.0f});
//    glm_scale(trans_, (vec3){0.5f, 0.5f, 0.5f});

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) exit(EXIT_FAILURE);
//    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    vertex_shader_ = compile_shader("../Shaders/pos_col_vertex_img.shader", GL_VERTEX_SHADER);
    if (!vertex_shader_) {
        return -3;
    }
    frag_shader_ = compile_shader("../Shaders/pos_col_frag_img.shader", GL_FRAGMENT_SHADER);
    if (!frag_shader_) {
        return -4;
    }
    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader_);
    glAttachShader(program_, frag_shader_);
    glLinkProgram(program_);
    glDeleteShader(vertex_shader_);
    glDeleteShader(frag_shader_);

    // init VAO
    init_vao_data();
    init_image_data();

    glUniform1f(glGetUniformLocation(program_, "mixValue"), 0.5);

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while(!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.91f, 0.91f, 0.91f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
        ++frame_;
    }

    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);

    glfwTerminate();
    return 0;
}

