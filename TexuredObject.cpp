#include "TexturedObject.hpp"
#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

TexturedObject::TexturedObject()
    : m_VAO(0), m_VBO(0), m_EBO(0), m_textureID(0),
      m_vertexCount(0), m_indexCount(0),
      m_position(0.0f), m_rotation(0.0f), m_scale(1.0f) {}

TexturedObject::~TexturedObject() {
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
    if (m_EBO != 0) glDeleteBuffers(1, &m_EBO);
}

bool TexturedObject::loadTexture(const std::string& filePath) {
    m_texture = std::make_shared<BitmapHandler>();
    if (m_texture->loadTexture(filePath)) {
        m_textureID = m_texture->getTextureID();
        return true;
    }
    return false;
}

void TexturedObject::setTexture(std::shared_ptr<BitmapHandler> texture) {
    m_texture = texture;
    if (texture) {
        m_textureID = texture->getTextureID();
    } else {
        m_textureID = 0;
    }
}

void TexturedObject::setPosition(const glm::vec3& position) {
    m_position = position;
}

void TexturedObject::setRotation(const glm::vec3& rotation) {
    m_rotation = rotation;
}

void TexturedObject::setScale(const glm::vec3& scale) {
    m_scale = scale;
}

void TexturedObject::translate(const glm::vec3& translation) {
    m_position += translation;
}

void TexturedObject::rotate(const glm::vec3& rotation) {
    m_rotation += rotation;
}

void TexturedObject::scale(const glm::vec3& scale) {
    m_scale *= scale;
}

glm::mat4 TexturedObject::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_position);
    model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, m_scale);
    return model;
}

void TexturedObject::draw() const {
    if (m_VAO == 0) return;
    
    glBindVertexArray(m_VAO);
    if (m_indexCount > 0) {
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }
    glBindVertexArray(0);
}

void TexturedObject::drawWithTexture() const {
    if (m_texture) {
        m_texture->bind(GL_TEXTURE0);
    }
    draw();
}

// Implementacja TexturedCube

TexturedCube::TexturedCube() : TexturedObject() {}

TexturedCube::~TexturedCube() {}

void TexturedCube::create(float size) {
    float halfSize = size / 2.0f;
    
    std::vector<Vertex> vertices = {
        // Front
        {{-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ halfSize,  halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-halfSize,  halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        
        // Back
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-halfSize,  halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ halfSize,  halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        {{ halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        
        // Top
        {{-halfSize,  halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{-halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ halfSize,  halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        
        // Bottom
        {{-halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{ halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
        {{ halfSize, -halfSize,  halfSize}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-halfSize, -halfSize,  halfSize}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        
        // Right
        {{ halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ halfSize,  halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{ halfSize,  halfSize,  halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{ halfSize, -halfSize,  halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        
        // Left
        {{-halfSize, -halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-halfSize, -halfSize,  halfSize}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-halfSize,  halfSize,  halfSize}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-halfSize,  halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,    // Front
        4, 5, 6, 6, 7, 4,    // Back
        8, 9, 10, 10, 11, 8, // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20  // Left
    };
    
    m_vertexCount = static_cast<int>(vertices.size());
    m_indexCount = static_cast<int>(indices.size());
    
    // Generowanie VAO, VBO, EBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    // Wierzchołki
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), 
                 vertices.data(), GL_STATIC_DRAW);
    
    // Indeksy
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
    
    // Atrybuty wierzchołków
    // Pozycja
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)offsetof(Vertex, position));
    
    // Normalna
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, normal));
    
    // Koordynaty tekstury
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, texCoord));
    
    glBindVertexArray(0);
    
    std::cout << "Utworzono teksturowany sześcian (" 
              << m_vertexCount << " wierzchołków, " 
              << m_indexCount << " indeksów)" << std::endl;
}

void TexturedCube::setupBuffers() {
    // Już zaimplementowane w create()
}