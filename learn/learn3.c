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
static float delta_time_ = 0.0f;
static float last_frame_time_ = 0.0f;

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

//static float angle_ = 0.0f;
static mat4 trans_;
static mat4 projection_mat_;
static mat4 model_mat_;

static vec3 camera_pos_     = (vec3){0.0f, 0.0f, 10.0f};
static vec3 camera_front_   = (vec3){0.0f, 0.0f, -1.0f};
static vec3 camera_up_      = (vec3){0.0f, 1.0f,  0.0f};

static vec3 model_rot_ = (vec3){70.0f, 0.0f, 0.0f};

static float yaw_ = -90.0f;
static float pitch_ = 0.0f;
static float fov_ = 45.0f;

//static const float camera_speed_ = 0.05f;

static float last_x = WIN_WIDTH / 2;
static float last_y = WIN_HEIGHT / 2;
static const float sensitivity = 0.05f;
static bool first_mouse = true;

static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    fov_ -= y_offset;
    fov_ = glm_clamp(fov_, 1.0f, 45.0f);
}

static void mouse_callback(GLFWimage* window, double x_pos, double y_pos) {
    if (first_mouse) {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }
    float x_offset = x_pos - last_x;
    float y_offset = y_pos - last_y;
    last_x = x_pos;
    last_y = y_pos;

    x_offset *= sensitivity;
    y_offset *= sensitivity;

    yaw_ += x_offset;
    pitch_ += y_offset;

    pitch_ = glm_clamp(pitch_, -89.f, 89.f);
    camera_front_[0] = cos(glm_rad(pitch_)) * cos(glm_rad(yaw_));
    camera_front_[1] = sin(glm_rad(pitch_));
    camera_front_[2] = cos(glm_rad(pitch_)) * sin(glm_rad(yaw_));
    glm_normalize(camera_front_);
//    printf("%f, %f\n", pitch_, yaw_);
}

static void process_input(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vec3 speed;
        glm_vec3_fill(speed, 2.5f * delta_time_);
        glm_vec3_mul(speed, camera_front_, speed);
        glm_vec3_add(camera_pos_, speed, camera_pos_);
//        glm_vec3_print(camera_pos_, stdout);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        vec3 speed;
        glm_vec3_fill(speed, 2.5f * delta_time_);
        glm_vec3_mul(speed, camera_front_, speed);
        glm_vec3_sub(camera_pos_, speed, camera_pos_);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 speed, tmp;
        glm_vec3_fill(speed, 2.5f * delta_time_);
        glm_cross(camera_front_, camera_up_, tmp);
        glm_normalize(tmp);
        glm_vec3_mul(speed, tmp, speed);
        glm_vec3_sub(camera_pos_, speed, camera_pos_);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 speed, tmp;
        glm_vec3_fill(speed, 2.5f * delta_time_);
        glm_cross(camera_front_, camera_up_, tmp);
        glm_normalize(tmp);
        glm_vec3_mul(speed, tmp, speed);
        glm_vec3_add(camera_pos_, speed, camera_pos_);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
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
//    glm_lookat(camera_pos_, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, view_mat);
    vec3 center;
    glm_vec3_add(camera_pos_, camera_front_, center);
    glm_lookat(camera_pos_, center, camera_up_, view_mat);
    mat4 model_mat;
    glm_mat4_copy(model_mat_, model_mat);
    glm_translate_to(model_mat_, model_rot_, model_mat);

    glm_perspective(glm_rad(fov_), (float)WIN_WIDTH / WIN_HEIGHT,
                    0.1f, 100.0f, projection_mat_);

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
}

int main() {
    glm_mat4_identity(projection_mat_);
    glm_mat4_identity(model_mat_);
//    glm_mat4_identity(view_mat_);

    glm_perspective(glm_rad(45.0f), (float)WIN_WIDTH / WIN_HEIGHT,
                    0.1f, 100.0f, projection_mat_);
    glm_rotate(model_mat_, glm_rad(-55.0f), (vec3){1.0f, 0.0f, 0.0f});

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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    last_frame_time_ = glfwGetTime();
    while(!glfwWindowShouldClose(window)) {
        float time = glfwGetTime();
        delta_time_ = time - last_frame_time_;
        last_frame_time_ = time;
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

