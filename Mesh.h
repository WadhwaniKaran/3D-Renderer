#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

enum TEXTURE {
    DIFFUSE = 0,
    SPECULAR,
    EMISSION,
    TOTAL,
};

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    Mesh(const std::string& mesh_path);
    bool draw(Shader& shader);
    void load_texture(const std::string& texture_path, int type);
    ~Mesh();
private:
    unsigned int VBO, VAO, EBO;
    unsigned int textures[TEXTURE::TOTAL];

    bool load_from_obj(const std::string& mesh_path);
    bool setup();
    void load_from_image(const std::string& texture_path);
};

Mesh::Mesh(const std::string& mesh_path) {
    if (!this->load_from_obj(mesh_path)) {
        std::cout << "ERROR::MESH::LOAD_FAILED" << std::endl;
        return;
    }
    if (!this->setup()) {
        std::cout << "ERROR::MESH::SETUP_FAILED" << std::endl;
        return;
    }
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

bool Mesh::load_from_obj(const std::string& mesh_path) {
    std::ifstream mesh_file;
    mesh_file.exceptions(std::ifstream::badbit);

    try {
        mesh_file.open(mesh_path);
        std::vector<glm::vec3> pos, normal;
        std::vector<glm::vec2> tex;
        while (!mesh_file.eof()) {
            char junk;
            char line[128];
            std::stringstream s;
            mesh_file.getline(line, 128);
            s << line;
            float x, y, z;
            if (line[0] == 'v') {
                if (line[1] == 'n') {
                    s >> junk >> junk >> x >> y >> z;
                    normal.push_back(glm::vec3(x, y, z));
                }
                else if (line[1] == 't') {
                    s >> junk >> junk >> x >> y;
                    tex.push_back(glm::vec2(x, y));
                }
                else {
                    s >> junk >> x >> y >> z;
                    pos.push_back(glm::vec3(x, y, z));
                }

            }
            if (line[0] == 'f') {
                Vertex v;
                s >> junk;
                for (int i = 0; i < 3; i++) {
                    s >> x >> junk >> y >> junk >> z; // Position / TexCoords / Normal
                    v.Position = pos[x - 1];
                    v.TexCoords = tex[y - 1];
                    v.Normal = normal[z - 1];
                    indices.push_back(indices.size());
                    vertices.push_back(v);
                }
            }
        }
        pos.clear();
        normal.clear();
        tex.clear();
        mesh_file.close();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::MESH::FILE_NOT_READ_SUCCESSFULLY" << std::endl;
        return false;
    }

    return true;
}

bool Mesh::setup() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)); // Normal

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords)); // TexCoords

    glBindVertexArray(0);

    return true;
}

bool Mesh::draw(Shader& shader) {
    shader.use();
    for (int i = 0; i < TEXTURE::TOTAL; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    return true;
}

void Mesh::load_from_image(const std::string& texture_path) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texture_path.c_str(), &width, &height,
        &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
            GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "ERROR::TEXTURE::LOAD_FAILED" << std::endl;
    }
    stbi_image_free(data);
}

void Mesh::load_texture(const std::string& texture_path, int type) {
    glGenTextures(1, &textures[type]);
    glActiveTexture(GL_TEXTURE0 + type);
    glBindTexture(GL_TEXTURE_2D, textures[type]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    load_from_image(texture_path);
}