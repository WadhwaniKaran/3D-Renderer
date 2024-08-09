#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>

#include <vector>
#include <string>
#include <iostream>

enum TextureType {
	DIFFUSE = 0,
	SPECULAR,
	EMISSION,
	TOTAL
};

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	TextureType type;
	std::string path;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		std::vector<Texture> textures);

	void draw(Shader& shader);
private:
	unsigned int VBO, VAO, EBO;

	void setup();
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
	std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setup();
}

void Mesh::setup() {
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
}

// material.diffuse[1..3] material.specular[1..3] material.emission[1..3]
void clearActiveTextures() {
	for (int i = 0; i < 9; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::draw(Shader& shader) {
	int diffuse_sam = 0, specular_sam = 0, emission_sam = 0;
	clearActiveTextures();
	shader.use();
	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		if(textures[i].type == TextureType::DIFFUSE) {
			shader.setInt(("material.diffuse[" + std::to_string(diffuse_sam++) + "]").c_str(),
				textures[i].id);
		} else if (textures[i].type == TextureType::SPECULAR) {
			shader.setInt(("material.specular[" + std::to_string(specular_sam++) + "]").c_str(), 
				textures[i].id);
		} else if (textures[i].type == TextureType::EMISSION) {
			shader.setInt(("material.emission[" + std::to_string(emission_sam++) + "]").c_str(), 
				textures[i].id);
		} else {
			std::cout << "ERROR::MESH::TEXTURE::INVALID_TYPE" << std::endl;
			return;
		}
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}