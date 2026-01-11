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

// ============================================
// TexturedCube Implementation
// ============================================

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

// ============================================
// TexturedSphere Implementation
// ============================================

TexturedSphere::TexturedSphere() : TexturedObject() {}

TexturedSphere::~TexturedSphere() {}

void TexturedSphere::create(float radius, int sectors, int stacks) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2 * glm::pi<float>() / sectors;
    float stackStep = glm::pi<float>() / stacks;

    // Generowanie wierzchołków
    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * stackStep; // Od π/2 do -π/2
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            // Pozycja
            glm::vec3 position(x, y, z);

            // Normalna (znormalizowany wektor pozycji)
            glm::vec3 normal = glm::normalize(position);

            // Koordynaty tekstury
            float s = (float)j / sectors;
            float t = (float)i / stacks;
            glm::vec2 texCoord(s, 1.0f - t); // Odwracamy t dla poprawnego układu tekstur

            vertices.push_back({position, normal, texCoord});
        }
    }

    // Generowanie indeksów
    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            // 2 trójkąty na każdy kwadrat
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

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

    std::cout << "Utworzono teksturowaną kulę (radius: " << radius
              << ", sektory: " << sectors << ", stosy: " << stacks
              << ") - " << m_vertexCount << " wierzchołków, "
              << m_indexCount << " indeksów" << std::endl;
}

void TexturedSphere::setupBuffers() {
    create(); // Tworzy domyślną kulę
}

// ============================================
// TexturedCylinder Implementation
// ============================================

TexturedCylinder::TexturedCylinder() : TexturedObject() {}

TexturedCylinder::~TexturedCylinder() {}

void TexturedCylinder::create(float radius, float height, int sectors) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2.0f * glm::pi<float>() / sectors;
    float halfHeight = height / 2.0f;

    // 1. Centra podstaw
    vertices.push_back({{0.0f, halfHeight, 0.0f},  {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}});   // 0 - górny środek
    vertices.push_back({{0.0f, -halfHeight, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f}}); // 1 - dolny środek

    // 2. Wierzchołki obwodu
    for (int i = 0; i <= sectors; ++i) {
        float angle = i * sectorStep;
        float x = cosf(angle);
        float z = sinf(angle);

        glm::vec3 sideNormal = glm::normalize(glm::vec3(x, 0.0f, z));

        // górna podstawa (punkt na obwodzie)
        vertices.push_back({
            {x * radius, halfHeight, z * radius},
            {0.0f, 1.0f, 0.0f},
            {x * 0.5f + 0.5f, z * 0.5f + 0.5f}
        });

        // dolna podstawa (punkt na obwodzie)
        vertices.push_back({
            {x * radius, -halfHeight, z * radius},
            {0.0f, -1.0f, 0.0f},
            {x * 0.5f + 0.5f, z * 0.5f + 0.5f}
        });

        // ściana górna (dla ściany bocznej)
        vertices.push_back({
            {x * radius, halfHeight, z * radius},
            sideNormal,
            {(float)i / sectors, 1.0f}
        });

        // ściana dolna (dla ściany bocznej)
        vertices.push_back({
            {x * radius, -halfHeight, z * radius},
            sideNormal,
            {(float)i / sectors, 0.0f}
        });
    }

    // 3. Indeksy dla górnej podstawy (CCW patrząc z góry)
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(0);                      // środek górny
        indices.push_back(2 + (i + 1) * 4);       // punkt (i+1) na obwodzie górnej podstawy
        indices.push_back(2 + i * 4);             // punkt i na obwodzie górnej podstawy
    }

    // 4. Indeksy dla dolnej podstawy (CCW patrząc od dołu)
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(1);                      // środek dolny
        indices.push_back(3 + i * 4);             // punkt i na obwodzie dolnej podstawy
        indices.push_back(3 + (i + 1) * 4);       // punkt (i+1) na obwodzie dolnej podstawy
    }

    // 5. Indeksy dla ścian bocznych
    for (int i = 0; i < sectors; ++i) {
        int base = 2 + i * 4;                     // początek aktualnego sektora
        int next = 2 + (i + 1) * 4;               // początek następnego sektora

        // Pierwszy trójkąt ściany bocznej (przeciwnie do ruchu wskazówek zegara)
        indices.push_back(base + 2);              // górny punkt aktualnego sektora (ściana)
        indices.push_back(next + 2);              // górny punkt następnego sektora (ściana)
        indices.push_back(base + 3);              // dolny punkt aktualnego sektora (ściana)

        // Drugi trójkąt ściany bocznej (przeciwnie do ruchu wskazówek zegara)
        indices.push_back(base + 3);              // dolny punkt aktualnego sektora (ściana)
        indices.push_back(next + 2);              // górny punkt następnego sektora (ściana)
        indices.push_back(next + 3);              // dolny punkt następnego sektora (ściana)
    }

    m_vertexCount = static_cast<int>(vertices.size());
    m_indexCount = static_cast<int>(indices.size());

    // Generowanie VAO, VBO, EBO
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }

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
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);

    std::cout << "Utworzono teksturowany cylinder (radius: " << radius
              << ", height: " << height << ", sektory: " << sectors
              << ") - " << m_vertexCount << " wierzchołków, "
              << m_indexCount << " indeksów" << std::endl;
}

void TexturedCylinder::setupBuffers() {
    create(); // Tworzy domyślny cylinder
}
