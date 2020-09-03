//
// Created by tangs on 2020/9/2.
//

#include "camera.h"

static void update_camera_vectors(struct Camera *camera) {
    vec3 front = {
            cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
            sin(glm_rad(camera->pitch)),
            sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
    };
    glm_normalize_to(front, camera->front);

    glm_cross(camera->front, camera->world_up, camera->right);
    glm_normalize(camera->right);

    glm_cross(camera->right, camera->front, camera->up);
    glm_normalize(camera->up);
}

void camera_init(struct Camera *camera, vec3 position, vec3 up, float yaw, float pitch) {
    glm_vec3_copy(position, camera->position);
    glm_vec3_copy(up, camera->world_up);
    camera->yaw = yaw;
    camera->pitch = pitch;
    // init by default value.
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera->front);
    camera->movement_speed = SPEED;
    camera->mouse_sensitivity = SENSITIVITY;
    camera->zoom = ZOOM;
    update_camera_vectors(camera);
}

void camera_init_def(struct Camera *camera) {
    camera_init(camera, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, YAW, PITCH);
}

void camera_get_view_matrix(struct Camera *camera, mat4 dest) {
    vec3 center;
    glm_vec3_add(camera->position, camera->front, center);
    glm_lookat(camera->position, center, camera->up, dest);
}

void camera_process_keyboard(struct Camera *camera, enum CameraMovement direction, float delta_time) {
    vec3 dt;
    glm_vec3_fill(dt, camera->movement_speed * delta_time);
    switch (direction) {
        case FORWARD: {
            glm_vec3_mul(camera->front, dt, dt);
            glm_vec3_add(camera->position, dt, camera->position);
        }
        break;
        case BACKWARD: {
            glm_vec3_mul(camera->front, dt, dt);
            glm_vec3_sub(camera->position, dt, camera->position);
        }
        break;
        case LEFT: {
            glm_vec3_mul(camera->right, dt, dt);
            glm_vec3_sub(camera->position, dt, camera->position);
        }
        break;
        case RIGHT: {
            glm_vec3_mul(camera->right, dt, dt);
            glm_vec3_add(camera->position, dt, camera->position);
        }
        break;
    }
}

void camera_process_mouse_movement(struct Camera *camera, float x_offset, float y_offset, bool constrain_pitch) {
    x_offset *= camera->mouse_sensitivity;
    y_offset *= camera->mouse_sensitivity;

    camera->yaw += x_offset;
    camera->pitch += y_offset;

    if (constrain_pitch) {
        camera->pitch = glm_clamp(camera->pitch, -89.0f, 89.0f);
    }

    update_camera_vectors(camera);
}

void camera_process_mouse_scroll(struct Camera *camera, float y_offset) {
    camera->zoom -= y_offset;
    camera->zoom = glm_clamp(camera->zoom, 1.0f, 45.0f);
}
