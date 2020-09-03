//
// Created by tangs on 2020/8/27.
//

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include "camera.h"
#include "shader.h"
#include "texture2d.h"

static int frame_ = 0;
static float delta_time_ = 0.0f;
static float last_frame_time_ = 0.0f;

static const int WIN_WIDTH = 1280;
static const int WIN_HEIGHT = 720;

static struct Shader shader_;

static GLuint vbo_ = 0;
static GLuint vao_ = 0;
static GLuint ebo_ = 0;

static struct Texture2d *texture1_;
static struct Texture2d *texture2_;

static float vertices_[] = {
//     ---- 位置 ----         - 纹理坐标 -
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,             0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,     0.333f, 0.0f,           1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,     0.333f, 0.333f,         1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,     0.333f, 0.333f,         1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 0.333f,           0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,             0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,    0.333f, 0.0f,           0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,     0.666f, 0.0f,           1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,     0.666f, 0.333f,         1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,     0.666f, 0.333f,         1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.333f, 0.333f,         0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.333f, 0.0f,           0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,             1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 0.333f,           1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.666f, 0.333f,         0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.666f, 0.333f,         0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.666f, 0.0f,           0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,             1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,     0.333f, 0.333f,         1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,     0.333f, 0.666f,         1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,     0.0f, 0.666f,           0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,     0.0f, 0.666f,           0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,     0.0f, 0.333f,           0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,     0.333f, 0.333f,         1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,    0.333f, 0.666f,         0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,     0.666f, 0.666f,         1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,     0.666f, 0.333f,         1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,     0.666f, 0.333f,         1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.333f, 0.333f,         0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.333f, 0.666f,         0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,    0.666f, 0.666f,         0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,     1.0f, 0.666f,           1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,     1.0f, 0.333f,           1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,     1.0f, 0.333f,           1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.666f, 0.333f,         0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.666f, 0.666f,         0.0f, 1.0f,
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

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);

    glBindVertexArray(vao_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

static void init_image_data() {
    texture1_ = texture_create("cube_met.png");
    texture2_ = texture_create("awesomface.png");
    texture_retain(texture1_);
    texture_retain(texture2_);
}

//static float angle_ = 0.0f;
static mat4 trans_;
static mat4 projection_mat_;
static mat4 model_mat_;
static vec3 model_rot_ = (vec3){70.0f, 0.0f, 0.0f};


static float last_x = WIN_WIDTH / 2;
static float last_y = WIN_HEIGHT / 2;
static bool first_mouse = true;

static struct Camera camera_;

static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    camera_process_mouse_scroll(&camera_, y_offset);
}

static void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    if (first_mouse) {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }
    float x_offset = x_pos - last_x;
    float y_offset = y_pos - last_y;
    last_x = x_pos;
    last_y = y_pos;

    camera_process_mouse_movement(&camera_, x_offset, y_offset, true);
}

static void process_input(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_process_keyboard(&camera_, FORWARD, delta_time_);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_process_keyboard(&camera_, BACKWARD, delta_time_);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_process_keyboard(&camera_, LEFT, delta_time_);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_process_keyboard(&camera_, RIGHT, delta_time_);
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

static void update_mat() {
    mat4 view_mat;
    camera_get_view_matrix(&camera_, view_mat);

    glm_perspective(glm_rad(camera_.zoom), (float)WIN_WIDTH / WIN_HEIGHT,
                    0.1f, 100.0f, projection_mat_);

    shader_set_matrix(shader_, "view", view_mat);
    shader_set_matrix(shader_, "projection", projection_mat_);
}

static void render() {
    update_mat();
    shader_use(shader_);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, texture1_->id);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, texture2_->id);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

    for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); ++i) {
        mat4 model_mat;
        glm_translate_to(model_mat_, cubePositions[i], model_mat);
        if (i) glm_rotate(model_mat, glm_rad((int)(glfwGetTime() * 250) % 360), cubeRotAis[i]);
        shader_set_matrix(shader_, "model", model_mat);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

int main() {
    texture2d_cache_init();
    camera_init(&camera_, (vec3){0.0f, 0.0f, 10.0f}, (vec3){0.0f, 1.0f,  0.0f}, -90.0f, 0.0f);
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
    if (shader_init(&shader_,
                "../Shaders/pos_col_vertex_img.shader",
                "../Shaders/pos_col_frag_img.shader")) {
        return -3;
    }

    // init VAO
    init_vao_data();
    init_image_data();

    shader_use(shader_);
    shader_set_int(shader_, "texture1", 0);
    shader_set_int(shader_, "texture2", 1);
    shader_set_float(shader_, "mixValue", 0.5f);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1_->id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2_->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

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
