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

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include <Nuklear/nuklear.h>
#include <Nuklear/nuklear_glfw_gl3.h>

static int frame_ = 0;
static float delta_time_ = 0.0f;
static float last_frame_time_ = 0.0f;

static const int WIN_WIDTH = 1280;
static const int WIN_HEIGHT = 720;

static struct Shader shader_;
static struct Shader shader_white_;

static GLuint vbo_ = 0;
static GLuint vao_ = 0;
static GLuint light_vao_ = 0;
//static GLuint ebo_ = 0;

static struct Texture2d *texture1_;
static struct Texture2d *texture2_;

static float vertices_[] = {
//        ---- 位置 ----         -法向量-                - 纹理坐标 -
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f,   0.0f,           0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.333f, 0.0f,           1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.333f, 0.333f,         1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.333f, 0.333f,         1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f,   0.333f,         0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f,   0.0f,           0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.333f, 0.0f,           0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.666f, 0.0f,           1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.666f, 0.333f,         1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.666f, 0.333f,         1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.333f, 0.333f,         0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.333f, 0.0f,           0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f,   0.0f,           1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   1.0f,   0.333f,         1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.666f, 0.333f,         0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.666f, 0.333f,         0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.666f, 0.0f,           0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f,   0.0f,           1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.333f, 0.333f,         1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.333f, 0.666f,         1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f,   0.666f,         0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f,   0.666f,         0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f,   0.333f,         0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.333f, 0.333f,         1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.333f, 0.666f,         0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.666f, 0.666f,         1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.666f, 0.333f,         1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.666f, 0.333f,         1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.333f, 0.333f,         0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.333f, 0.666f,         0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.666f, 0.666f,         0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   1.0f,   0.666f,         1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f,   0.333f,         1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f,   0.333f,         1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   0.666f, 0.333f,         0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.666f, 0.666f,         0.0f, 1.0f,
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
    glGenVertexArrays(1, &light_vao_);
    glBindVertexArray(light_vao_);

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
//    glGenBuffers(1, &ebo_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);

    glBindVertexArray(vao_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(8 * sizeof(float)));
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
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
static vec3 model_pos_ = (vec3){0.0f, 0.0f, 0.0f};


static float last_x_ = WIN_WIDTH / 2;
static float last_y_ = WIN_HEIGHT / 2;
static bool first_mouse_ = true;

static struct Camera camera_;

// camera movable
static bool movable_ = false;
static struct nk_colorf light_color_ = {1.0f, 1.0f, 0.0f, 1.0f};

static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    camera_process_mouse_scroll(&camera_, y_offset);
}

static void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    if (first_mouse_) {
        last_x_ = x_pos;
        last_y_ = y_pos;
        first_mouse_ = false;
    }
    float x_offset = x_pos - last_x_;
    float y_offset = y_pos - last_y_;
    last_x_ = x_pos;
    last_y_ = y_pos;

    if (movable_)
        camera_process_mouse_movement(&camera_, x_offset, y_offset, true);
}

static bool space_pressed_ = false;
static void process_input(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!space_pressed_) {
            movable_ = !movable_;
        }
        space_pressed_ = true;
    } else if (space_pressed_) {
//        movable_ = !movable_;
        space_pressed_ = false;
    }

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
        model_pos_[1] += 0.05f;
//        glm_rotate(model_mat_, glm_rad(-1.0f), (vec3){1.0f, 0.0f, 0.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        model_pos_[1] += -0.05f;
//        glm_rotate(model_mat_, glm_rad(1.0f), (vec3){1.0f, 0.0f, 0.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        model_pos_[0] += -0.05f;
//        glm_rotate(model_mat_, glm_rad(-1.0f), (vec3){0.0f, 1.0f, 0.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        model_pos_[0] += 0.05f;
//        glm_rotate(model_mat_, glm_rad(1.0f), (vec3){0.0f, 1.0f, 0.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        model_pos_[2] += 0.05f;
//        glm_rotate(model_mat_, glm_rad(-1.0f), (vec3){0.0f, 0.0f, 1.0f});
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        model_pos_[2] -= 0.05f;
//        glm_rotate(model_mat_, glm_rad(1.0f), (vec3){0.0f, 0.0f, 1.0f});
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

    shader_use(shader_);
    shader_set_matrix(shader_, "view", view_mat);
    shader_set_matrix(shader_, "projection", projection_mat_);
    shader_set_vec3(shader_, "lightColor", (vec3){light_color_.r, light_color_.g, light_color_.b});
//    shader_set_vec3(shader_, "lightPos", cubePositions[0]);

    shader_use(shader_white_);
    shader_set_matrix(shader_white_, "view", view_mat);
    shader_set_matrix(shader_white_, "projection", projection_mat_);
    shader_set_vec4(shader_white_, "color", (vec4){light_color_.r, light_color_.g, light_color_.b, light_color_.a});
}

static void render() {
    update_mat();
    shader_use(shader_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1_->id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2_->id);
    glBindVertexArray(vao_);

    int len = sizeof(cubePositions) / sizeof(cubePositions[0]);
    for (int i = 0; i < len; ++i) {
        mat4 model_mat;
        vec3 pos;
        glm_vec3_copy(cubePositions[i], pos);
        if (i == 0) {
            glm_vec3_add(pos, model_pos_, pos);
            shader_set_vec3(shader_, "lightPos", pos);
        }
        glm_translate_to(model_mat_, pos, model_mat);
//        if (i) glm_rotate(model_mat, glm_rad((int)(glfwGetTime() * 250) % 360), cubeRotAis[i]);
        if (i != 0) {
            shader_use(shader_);
            shader_set_matrix(shader_, "model", model_mat);
        } else {
            shader_use(shader_white_);
            shader_set_matrix(shader_white_, "model", model_mat);
        }
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
static struct nk_context *nk_ctx_;
static struct nk_colorf bg_;

void init_gui(GLFWwindow *window) {
    nk_ctx_ = nk_glfw3_init(window, NK_GLFW3_DEFAULT);
    {struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&atlas);
        /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
        /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
        /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
        /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
        /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
        /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
        nk_glfw3_font_stash_end();
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        /*nk_style_set_font(ctx, &droid->handle);*/}
}

void update_gui() {/* GUI */
    if (nk_begin(nk_ctx_, "update", nk_rect(50, 50, 230, 280),
                 NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                 NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        static int property = 20;
        char tmp[256];
        snprintf(tmp, sizeof(tmp), "fps:%f", 1.0f / delta_time_);
        nk_layout_row_dynamic(nk_ctx_, 30, 1);
        nk_label(nk_ctx_, tmp, NK_TEXT_LEFT);

        nk_layout_row_static(nk_ctx_, 30, 120, 1);
        if (nk_button_label(nk_ctx_, "reset camera")) {
//            fprintf(stdout, "button pressed\n");
            camera_set_euler_angle(&camera_, -90.0f, 0.0f);
        }

        nk_layout_row_dynamic(nk_ctx_, 30, 2);
        movable_ = nk_check_label(nk_ctx_, "movable", movable_);

        nk_layout_row_dynamic(nk_ctx_, 25, 1);
        nk_property_int(nk_ctx_, "Compression:", 0, &property, 100, 10, 1);

        nk_layout_row_dynamic(nk_ctx_, 20, 1);
        nk_label(nk_ctx_, "background:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(nk_ctx_, 25, 1);
        if (nk_combo_begin_color(nk_ctx_, nk_rgb_cf((bg_)), nk_vec2(nk_widget_width(nk_ctx_), 400))) {
            nk_layout_row_dynamic(nk_ctx_, 120, 1);
            bg_ = nk_color_picker(nk_ctx_, bg_, NK_RGBA);
            nk_layout_row_dynamic(nk_ctx_, 25, 1);
            bg_.r = nk_propertyf(nk_ctx_, "#R:", 0, bg_.r, 1.0f, 0.01f, 0.005f);
            bg_.g = nk_propertyf(nk_ctx_, "#G:", 0, bg_.g, 1.0f, 0.01f, 0.005f);
            bg_.b = nk_propertyf(nk_ctx_, "#B:", 0, bg_.b, 1.0f, 0.01f, 0.005f);
            bg_.a = nk_propertyf(nk_ctx_, "#A:", 0, bg_.a, 1.0f, 0.01f, 0.005f);
            nk_combo_end(nk_ctx_);
        }

        nk_layout_row_dynamic(nk_ctx_, 20, 1);
        nk_label(nk_ctx_, "light color:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(nk_ctx_, 25, 1);
        if (nk_combo_begin_color(nk_ctx_, nk_rgb_cf((light_color_)), nk_vec2(nk_widget_width(nk_ctx_), 400))) {
            nk_layout_row_dynamic(nk_ctx_, 120, 1);
            light_color_ = nk_color_picker(nk_ctx_, light_color_, NK_RGBA);
            nk_layout_row_dynamic(nk_ctx_, 25, 1);
            light_color_.r = nk_propertyf(nk_ctx_, "#R:", 0, light_color_.r, 1.0f, 0.01f, 0.005f);
            light_color_.g = nk_propertyf(nk_ctx_, "#G:", 0, light_color_.g, 1.0f, 0.01f, 0.005f);
            light_color_.b = nk_propertyf(nk_ctx_, "#B:", 0, light_color_.b, 1.0f, 0.01f, 0.005f);
            light_color_.a = nk_propertyf(nk_ctx_, "#A:", 0, light_color_.a, 1.0f, 0.01f, 0.005f);
            nk_combo_end(nk_ctx_);
        }
    }
    nk_end(nk_ctx_);
}

int main() {
    texture2d_cache_init();
    camera_init(&camera_, (vec3){0.0f, 0.0f, 10.0f}, (vec3){0.0f, 1.0f,  0.0f}, -90.0f, 0.0f);
    glm_mat4_identity(projection_mat_);
    glm_mat4_identity(model_mat_);
//    glm_mat4_identity(view_mat_);

    glm_perspective(glm_rad(45.0f), (float)WIN_WIDTH / (float)WIN_HEIGHT,
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

    if (shader_init(&shader_white_,
                    "../Shaders/pos_col_vertex_img.shader",
                    "../Shaders/pos_col_frag_col4.shader")) {
        return -4;
    }

    // init VAO
    init_vao_data();
    init_image_data();

    shader_use(shader_);
    shader_set_int(shader_, "texture1", 0);
    shader_set_int(shader_, "texture2", 1);
    shader_set_float(shader_, "mixValue", 0.5f);
    shader_set_vec3(shader_, "lightColor", (vec4){0.0f, 1.0f, 0.0f});

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    init_gui(window);

    last_frame_time_ = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float time = glfwGetTime();
        delta_time_ = time - last_frame_time_;
        last_frame_time_ = time;

        glfwPollEvents();
        process_input(window);

        glClearColor(bg_.r, bg_.g, bg_.b, bg_.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        nk_glfw3_new_frame();
        update_gui();

        glEnable(GL_DEPTH_TEST);
        render();
        nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

        glfwSwapBuffers(window);
        ++frame_;
    }

    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);

    nk_glfw3_shutdown();
    glfwTerminate();
    return 0;
}
