#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;

    Shader(const std::string& vertex_path, const std::string& fragment_path);
    ~Shader();
    void use();

    void setMat4f(const std::string& name, glm::mat4 mat);
    void setMat3f(const std::string& name, glm::mat3 mat);
    void setVec3f(const std::string& name, glm::vec3 vec);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
};

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
    std::string vertex_code, fragment_code;
    std::ifstream v_file, f_file;

    v_file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    f_file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    try {
        v_file.open(vertex_path);
        f_file.open(fragment_path);
        std::stringstream v_stream, f_stream;

        v_stream << v_file.rdbuf();
        f_stream << f_file.rdbuf();

        v_file.close();
        f_file.close();

        vertex_code = v_stream.str();
        fragment_code = f_stream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FAILED_TO_READ_FILE " << e.what() << std::endl;
        glfwTerminate();
        return;
    }


    const char* v_code = vertex_code.c_str();
    const char* f_code = fragment_code.c_str();

    int success;
    char info_log[512];

    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_code, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_code, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::LINKING_FAILED\n" << info_log << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setMat4f(const std::string& name, glm::mat4 mat) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat3f(const std::string& name, glm::mat3 mat) {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3f(const std::string& name, glm::vec3 vec) {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}