#pragma once 

#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace camera {
    bool FIRST_MOUSE = true;
    double last_x = 0;
    double last_y = 0;
    float offset_x = 0.0;
    float offset_y = 0.0;
    float zoom = 45.0f;
}

class Camera {
public:
    glm::vec3 position, target, up, direction;
    float yaw, pitch, senstivity, zoom;
    glm::mat4 look_at;

    Camera();
    void keyboard_movement(GLFWwindow* window, float delta_time);
    void mouse_movement();
    void mouse_scroll();
    void process_cam_movement(GLFWwindow* window, float delta_time);
    void update_vectors();
};

Camera::Camera() {
    position = glm::vec3(0.0f, 0.0f, 3.0f);
    target = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);

    direction = position + target;

    look_at = glm::lookAt(position, direction, up);

    yaw = -90.0f;
    pitch = 0.0f;
    senstivity = 0.1f;
    zoom = 45.0f;
}

void Camera::keyboard_movement(GLFWwindow* window, float delta_time) {
    float cam_speed = 2.5f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += cam_speed * target;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= cam_speed * target;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= cam_speed * glm::normalize(glm::cross(target, up));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += cam_speed * glm::normalize(glm::cross(target, up));
    }
    update_vectors();
}

void Camera::mouse_movement() {
    camera::offset_x *= senstivity;
    camera::offset_y *= senstivity;

    yaw += camera::offset_x;
    pitch += camera::offset_y;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 temp_dir;
    temp_dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    temp_dir.y = sin(glm::radians(pitch));
    temp_dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    target = glm::normalize(temp_dir);

    update_vectors();
}

void Camera::mouse_scroll() {
    zoom = camera::zoom;
    if (zoom < 1.0f) {
        zoom = 1.0f;
    }
    if (zoom > 45.0f) {
        zoom = 45.0f;
    }
}

void Camera::process_cam_movement(GLFWwindow* window, float delta_time) {
    keyboard_movement(window, delta_time);
    mouse_movement();
    mouse_scroll();
}

void Camera::update_vectors() {
    direction = position + target;
    look_at = glm::lookAt(position, direction, up);
}