//
// Created by tangs on 2020/9/2.
//

#ifndef GLFW_CAMERA_H
#define GLFW_CAMERA_H

#include <cglm/cglm.h>

// default camera values
static const float YAW          = -90.0f;
static const float PITCH        =   0.0f;
static const float SPEED        =   2.5f;
static const float SENSITIVITY  =   0.1f;
static const float ZOOM         =  45.0f;

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

struct Camera {
    // camera attributes
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;
    // euler angles
    float yaw;
    float pitch;
    // camera options
    float movement_speed;
    float mouse_sensitivity;
    float zoom;
};

void camera_init(struct Camera *camera, vec3 position, vec3 up, float yaw, float pitch);
void camera_init_def(struct Camera *camera);
void camera_get_view_matrix(struct Camera *camera, mat4 dest);
void camera_process_keyboard(struct Camera *camera, enum Camera_Movement direction, float delta_time);
void camera_process_mouse_movement(struct Camera *camera, float x_offset, float y_offset, bool constrain_pitch);
void camera_process_mouse_scroll(struct Camera *camera, float y_offset);

#endif //GLFW_CAMERA_H
