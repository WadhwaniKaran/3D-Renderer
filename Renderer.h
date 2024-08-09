#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

//#include <Mesh.h>
#include <Shader.h>
#include <Camera.h>

#include <Model.h>

#include <string>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void glfw_error_callback(int error, const char* description);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double offset_x, double offset_y);

struct Config {
    int width;
    int height;
    const char* title;
    glm::vec4 color;
};

class Renderer {
    GLFWwindow* window;
    Config config;
    Camera cam;
    float last_frame = 0, current_frame = 0, delta_time = 0;
public:
    Renderer(int screen_width, int screen_height, const char* title);
    int setup();
    void render_loop();
    void process_input();
    ~Renderer();

};

Renderer::Renderer(int screen_width, int screen_height, const char* title) {
    this->window = NULL;
    this->config.width = screen_width;
    this->config.height = screen_height;
    this->config.title = title;
    this->config.color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
}

Renderer::~Renderer() {
    if (this->window) {
        glfwDestroyWindow(this->window);
    }
    glfwTerminate();
}

int Renderer::setup() {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        std::cout << "ERROR::GLFW::INITIALIZATION_FAILED" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->window = glfwCreateWindow(this->config.width, this->config.height, this->config.title, nullptr, nullptr);
    if (this->window == nullptr) {
        std::cout << "ERROR::GLFW::FAILED_TO_CREATE_WINDOW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(this->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR::GLAD::FAILED_TO_INITIALISE" << std::endl;
        glfwTerminate();
        return -1;
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    

    glViewport(0, 0, config.width, config.height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return 0;
}

void Renderer::render_loop() {
    glm::vec3 point_lights[] = {
        glm::vec3(0.7f,  0.2f,  2.0f), glm::vec3(0.8f, 0.0f, 0.0f),
        glm::vec3(2.3f, -3.3f, -4.0f), glm::vec3(0.0f, 0.8f, 0.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f), glm::vec3(0.8f, 0.4f, 0.8f),
        glm::vec3(0.0f,  0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.8f)
    };

    Model backpack("models\\backpack\\backpack.obj");
    Model cube("models\\cube\\cube.obj");
    Shader shader("shaders\\backpack.vert", "shaders\\backpack.frag");
    Shader light("shaders\\lightSource.vert", "shaders\\lightSource.frag");

    glm::mat4 projection = glm::mat4(1.0f);
    glm::vec3 point_light_pos = glm::vec3(0.7f, -1.2f, 4.0f);
    glm::vec3 point_light_color = glm::vec3(0.2f, 0.0f, 0.4f);
   
    glm::vec3 dirlight_color = glm::vec3(1.0f);
    shader.use();
    shader.setFloat("material.shininess", 32.0f);
    shader.setVec3f("dirLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
    shader.setVec3f("dirLight.ambient", dirlight_color * 0.2f);
    shader.setVec3f("dirLight.diffuse", dirlight_color);
    shader.setVec3f("dirLight.specular", glm::vec3(1.0f));

    shader.setVec3f("pointLights[0].position", point_lights[0]);
    shader.setVec3f("pointLights[0].ambient", point_lights[1] * 0.2f);
    shader.setVec3f("pointLights[0].diffuse", point_lights[1]);
    shader.setVec3f("pointLights[0].specular", point_lights[1]);
    shader.setFloat("pointLights[0].constant", 1.0f);
    shader.setFloat("pointLights[0].linear", 0.045f);
    shader.setFloat("pointLights[0].quadratic", 0.0075f);

    shader.setVec3f("pointLights[1].position", point_lights[2]);
    shader.setVec3f("pointLights[1].ambient", point_lights[3] * 0.2f);
    shader.setVec3f("pointLights[1].diffuse", point_lights[3]);
    shader.setVec3f("pointLights[1].specular", point_lights[3]);
    shader.setFloat("pointLights[1].constant", 1.0f);
    shader.setFloat("pointLights[1].linear", 0.045f);
    shader.setFloat("pointLights[1].quadratic", 0.0075f);

    shader.setVec3f("pointLights[2].position", point_lights[4]);
    shader.setVec3f("pointLights[2].ambient", point_lights[5] * 0.2f);
    shader.setVec3f("pointLights[2].diffuse", point_lights[5]);
    shader.setVec3f("pointLights[2].specular", point_lights[5]);
    shader.setFloat("pointLights[2].constant", 1.0f);
    shader.setFloat("pointLights[2].linear", 0.045f);
    shader.setFloat("pointLights[2].quadratic", 0.0075f);

    shader.setVec3f("pointLights[3].position", point_lights[6]);
    shader.setVec3f("pointLights[3].ambient", point_lights[7] * 0.2f);
    shader.setVec3f("pointLights[3].diffuse", point_lights[7]);
    shader.setVec3f("pointLights[3].specular", point_lights[7]);
    shader.setFloat("pointLights[3].constant", 1.0f);
    shader.setFloat("pointLights[3].linear", 0.045f);
    shader.setFloat("pointLights[3].quadratic", 0.0075f);

    shader.setVec3f("flashLight.ambient", glm::vec3(0.1f, 0.2f, 0.25f) * 0.0f);
    shader.setVec3f("flashLight.diffuse", glm::vec3(0.1f, 0.2f, 0.25f));
    shader.setVec3f("flashLight.specular", glm::vec3(0.1f, 0.2f, 0.25f));
    shader.setFloat("flashLight.cutoff", glm::cos(glm::radians(7.5f)));
    shader.setFloat("flashLight.outerCutoff", glm::cos(glm::radians(10.5f)));
    shader.setFloat("flashLight.constant", 1.0f);
    shader.setFloat("flashLight.linear", 0.07f);
    shader.setFloat("flashLight.quadratic", 0.017f);

    while (!glfwWindowShouldClose(window)) {
        last_frame = current_frame;
        current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        double fps = 1 / delta_time;
        std::stringstream ss;
        ss << "FPS: " << fps;
        glfwSetWindowTitle(window, ss.str().c_str());

        process_input();

        glClearColor(config.color.r, config.color.g, config.color.b, config.color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = glm::perspective(glm::radians(cam.zoom), (float)config.width / (float)config.height, 1.0f, 100.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        glm::mat3 normal_matrix = glm::transpose(glm::inverse(model));

        shader.use();
        shader.setMat4f("model", model);
        shader.setMat4f("view", cam.look_at);
        shader.setMat4f("projection", projection);
        shader.setMat3f("normalMatrix", normal_matrix);
        shader.setVec3f("viewPos", cam.position);

        shader.setVec3f("flashLight.position", cam.position);
        shader.setVec3f("flashLight.direction", cam.direction);

        backpack.draw(shader);

        light.use();
        light.setMat4f("view", cam.look_at);
        light.setMat4f("projection", projection);

        for (int i = 0; i < 4; i++) {
            glm::mat4 model_light = glm::mat4(1.0f);
            model_light = glm::translate(model_light, point_lights[i * 2]);
            model_light = glm::scale(model_light, glm::vec3(0.2f));

            light.use();
            light.setMat4f("model", model_light);
            light.setVec3f("lightColor", point_lights[i * 2 + 1]);
            cube.draw(light);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void Renderer::process_input() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    cam.process_cam_movement(window, delta_time);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void glfw_error_callback(int error, const char* description) {
    std::cout << "GLFW Error (" << error << "): " << description << std::endl;
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    if (camera::FIRST_MOUSE) {
        camera::last_x = x_pos;
        camera::last_y = y_pos;
        camera::FIRST_MOUSE = false;
    }

    camera::offset_x = x_pos - camera::last_x;
    camera::offset_y = camera::last_y - y_pos;
    camera::last_x = x_pos;
    camera::last_y = y_pos;
}

void scroll_callback(GLFWwindow* window, double offset_x, double offset_y) {
    camera::zoom -= (float)offset_y;
}