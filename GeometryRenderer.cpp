#define GLEW_STATIC
#include <GL/glew.h>
#include "GeometryRenderer.hpp"
#include <cmath>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265358979323846f

GeometryRenderer::GeometryRenderer() : m_drawMode(GL_TRIANGLES) {
    m_currentMaterial.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    m_currentMaterial.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    m_currentMaterial.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    m_currentMaterial.shininess = 32.0f;
}

GeometryRenderer::~GeometryRenderer() {
    deleteMesh(m_cubeMesh);
    deleteMesh(m_sphereMesh);
    deleteMesh(m_cylinderMesh);
    deleteMesh(m_coneMesh);
    deleteMesh(m_planeMesh);
    deleteMesh(m_torusMesh);
    deleteMesh(m_pyramidMesh);
    deleteMesh(m_gridMesh);

    glDeleteVertexArrays(1, &m_lineVAO);
    glDeleteBuffers(1, &m_lineVBO);
    glDeleteVertexArrays(1, &m_pointVAO);
    glDeleteBuffers(1, &m_pointVBO);
}

bool GeometryRenderer::initialize() {
    // Inicjalizacja GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Błąd inicjalizacji GLEW: " << glewGetErrorString(err) << std::endl;
        return false;
    }

    // Utworzenie podstawowych kształtów
    createCube();
    createSphere();
    createCylinder();
    createCone();
    createPlane();
    createTorus();
    createPyramid();
    createGrid();

    // Inicjalizacja buforów
    glGenVertexArrays(1, &m_lineVAO);
    glGenBuffers(1, &m_lineVBO);
    glGenVertexArrays(1, &m_pointVAO);
    glGenBuffers(1, &m_pointVBO);

    std::cout << "GeometryRenderer zainicjalizowany" << std::endl;
    return true;
}

void GeometryRenderer::setupMesh(Mesh& mesh, const std::vector<Vertex>& vertices,
                                 const std::vector<unsigned int>& indices) {
    mesh.vertices = vertices;      // Zapisanie wierzchołków
    mesh.indices = indices;        // Zapisanie indeksów

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    // Wierzchołki
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);

    // Indeksy
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    // Atrybuty wierzchołków - ZGODNE Z RYS. 8-10
    // Pozycja (atrybut 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normalna (atrybut 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, normal));

    // Kolor (atrybut 2) - DODANE!
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, color));

    // Koordynaty tekstury (atrybut 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
    mesh.indexCount = static_cast<int>(indices.size());
}

void GeometryRenderer::deleteMesh(Mesh& mesh) {
    glDeleteVertexArrays(1, &mesh.VAO);
    glDeleteBuffers(1, &mesh.VBO);
    glDeleteBuffers(1, &mesh.EBO);
}

void GeometryRenderer::createCube() {
    std::vector<Vertex> vertices;

    // Definicja wierzchołków z kolorami - ZGODNE Z RYS. 8
    // Front (czerwony)
    vertices.push_back({{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.3f, 0.3f}, {0.0f, 0.0f}});
    vertices.push_back({{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.3f, 0.3f}, {1.0f, 0.0f}});
    vertices.push_back({{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.3f, 0.3f}, {1.0f, 1.0f}});
    vertices.push_back({{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.3f, 0.3f}, {0.0f, 1.0f}});

    // Back (żółty)
    vertices.push_back({{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.3f}, {1.0f, 0.0f}});
    vertices.push_back({{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.3f}, {1.0f, 1.0f}});
    vertices.push_back({{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.3f}, {0.0f, 1.0f}});
    vertices.push_back({{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.3f}, {0.0f, 0.0f}});

    // Top (zielony)
    vertices.push_back({{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.3f, 1.0f, 0.3f}, {0.0f, 1.0f}});
    vertices.push_back({{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.3f, 1.0f, 0.3f}, {0.0f, 0.0f}});
    vertices.push_back({{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.3f, 1.0f, 0.3f}, {1.0f, 0.0f}});
    vertices.push_back({{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.3f, 1.0f, 0.3f}, {1.0f, 1.0f}});

    // Bottom (niebieski)
    vertices.push_back({{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.3f, 0.3f, 1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.3f, 0.3f, 1.0f}, {0.0f, 1.0f}});
    vertices.push_back({{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.3f, 0.3f, 1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.3f, 0.3f, 1.0f}, {1.0f, 0.0f}});

    // Right (różowy)
    vertices.push_back({{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.3f, 1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.3f, 1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.3f, 1.0f}, {0.0f, 1.0f}});
    vertices.push_back({{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.3f, 1.0f}, {0.0f, 0.0f}});

    // Left (cyjan)
    vertices.push_back({{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.3f, 1.0f, 1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.3f, 1.0f, 1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.3f, 1.0f, 1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.3f, 1.0f, 1.0f}, {0.0f, 1.0f}});

    // Indeksy - ZGODNE Z RYS. 9
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,    // Front
        4, 5, 6, 6, 7, 4,    // Back
        8, 9, 10, 10, 11, 8, // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20  // Left
    };

    setupMesh(m_cubeMesh, vertices, indices);
}

void GeometryRenderer::createSphere(int sectors, int stacks) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2 * PI / sectors;
    float stackStep = PI / stacks;

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = PI / 2 - i * stackStep;
        float xy = cosf(stackAngle);
        float z = sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            Vertex vertex;
            vertex.position = glm::vec3(x, y, z);
            vertex.normal = glm::vec3(x, y, z);
            // Kolory w zależności od pozycji
            vertex.color = glm::vec3((x + 1.0f) / 2.0f, (y + 1.0f) / 2.0f, (z + 1.0f) / 2.0f);
            vertex.texCoord = glm::vec2((float)j / sectors, (float)i / stacks);

            vertices.push_back(vertex);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
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

    setupMesh(m_sphereMesh, vertices, indices);
}

void GeometryRenderer::createCylinder(int sectors) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2 * PI / sectors;

    // Środek góry
    vertices.push_back({{0.0f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}});
    // Środek dołu
    vertices.push_back({{0.0f, -0.5f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f}});

    for (int i = 0; i <= sectors; ++i) {
        float angle = i * sectorStep;
        float x = cosf(angle);
        float z = sinf(angle);

        // Górna podstawa
        vertices.push_back({{x, 0.5f, z}, {0.0f, 1.0f, 0.0f}, {0.8f, 0.2f, 0.2f}, {x * 0.5f + 0.5f, z * 0.5f + 0.5f}});
        // Dolna podstawa
        vertices.push_back({{x, -0.5f, z}, {0.0f, -1.0f, 0.0f}, {0.2f, 0.2f, 0.8f}, {x * 0.5f + 0.5f, z * 0.5f + 0.5f}});
        // Ściana górna
        vertices.push_back({{x, 0.5f, z}, {x, 0.0f, z}, {0.2f, 0.8f, 0.2f}, {static_cast<float>(i) / sectors, 1.0f}});
        // Ściana dolna
        vertices.push_back({{x, -0.5f, z}, {x, 0.0f, z}, {0.8f, 0.8f, 0.2f}, {static_cast<float>(i) / sectors, 0.0f}});
    }

    // Górna podstawa
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(0);
        indices.push_back(2 + i * 4);
        indices.push_back(2 + (i + 1) * 4);
    }

    // Dolna podstawa
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(1);
        indices.push_back(3 + i * 4);
        indices.push_back(3 + (i + 1) * 4);
    }

    // Ściany
    for (int i = 0; i < sectors; ++i) {
        int k1 = 4 + i * 4;
        int k2 = 5 + i * 4;
        int k3 = 4 + (i + 1) * 4;
        int k4 = 5 + (i + 1) * 4;

        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k3);

        indices.push_back(k2);
        indices.push_back(k4);
        indices.push_back(k3);
    }

    setupMesh(m_cylinderMesh, vertices, indices);
}

void GeometryRenderer::createCone(int sectors) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2 * PI / sectors;

    // Wierzchołek stożka
    vertices.push_back({{0.0f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}});
    // Środek podstawy
    vertices.push_back({{0.0f, -0.5f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f}});

    for (int i = 0; i <= sectors; ++i) {
        float angle = i * sectorStep;
        float x = cosf(angle);
        float z = sinf(angle);

        // Podstawa
        vertices.push_back({{x, -0.5f, z}, {0.0f, -1.0f, 0.0f}, {0.6f, 0.3f, 0.9f}, {x * 0.5f + 0.5f, z * 0.5f + 0.5f}});

        // Ściana
        glm::vec3 normal = glm::normalize(glm::vec3(x, 0.25f, z));
        vertices.push_back({{x, -0.5f, z}, normal, {0.9f, 0.6f, 0.3f}, {static_cast<float>(i) / sectors, 0.0f}});
        vertices.push_back({{0.0f, 0.5f, 0.0f}, normal, {0.3f, 0.9f, 0.6f}, {static_cast<float>(i) / sectors, 1.0f}});
    }

    // Podstawa
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(1);
        indices.push_back(2 + i * 3);
        indices.push_back(2 + (i + 1) * 3);
    }

    // Ściany
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(3 + i * 3);
        indices.push_back(4 + i * 3);
        indices.push_back(3 + (i + 1) * 3);
    }

    setupMesh(m_coneMesh, vertices, indices);
}

void GeometryRenderer::createPlane() {
    std::vector<Vertex> vertices = {
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.3f, 0.7f, 0.3f}, {0.0f, 0.0f}},
        {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.3f, 0.7f, 0.3f}, {1.0f, 0.0f}},
        {{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.3f, 0.7f, 0.3f}, {1.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.3f, 0.7f, 0.3f}, {0.0f, 1.0f}}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0
    };

    setupMesh(m_planeMesh, vertices, indices);
}

void GeometryRenderer::createTorus(float radius, float tubeRadius, int sectors, int rings) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2 * PI / sectors;
    float ringStep = 2 * PI / rings;

    for (int i = 0; i <= rings; ++i) {
        float ringAngle = i * ringStep;
        float cosRing = cosf(ringAngle);
        float sinRing = sinf(ringAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;
            float cosSector = cosf(sectorAngle);
            float sinSector = sinf(sectorAngle);

            Vertex vertex;
            vertex.position.x = (radius + tubeRadius * cosSector) * cosRing;
            vertex.position.y = (radius + tubeRadius * cosSector) * sinRing;
            vertex.position.z = tubeRadius * sinSector;

            vertex.normal.x = cosRing * cosSector;
            vertex.normal.y = sinRing * cosSector;
            vertex.normal.z = sinSector;

            // Kolory w zależności od pozycji
            vertex.color.r = 0.5f + 0.5f * cosSector;
            vertex.color.g = 0.5f + 0.5f * sinSector;
            vertex.color.b = 0.5f + 0.5f * cosRing;

            vertex.texCoord.x = static_cast<float>(j) / sectors;
            vertex.texCoord.y = static_cast<float>(i) / rings;

            vertices.push_back(vertex);
        }
    }

    for (int i = 0; i < rings; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = i * (sectors + 1) + j;
            int second = first + sectors + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    setupMesh(m_torusMesh, vertices, indices);
}

void GeometryRenderer::createPyramid() {
    std::vector<Vertex> vertices = {
        // Podstawa (niebieska)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.2f, 0.2f, 0.8f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.2f, 0.2f, 0.8f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.2f, 0.2f, 0.8f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.2f, 0.2f, 0.8f}, {0.0f, 1.0f}},

        // Wierzchołek (czerwony)
        {{ 0.0f,  0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.8f, 0.2f, 0.2f}, {0.5f, 1.0f}},
    };

    std::vector<unsigned int> indices = {
        // Podstawa
        0, 1, 2, 2, 3, 0,

        // Ściany
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    setupMesh(m_pyramidMesh, vertices, indices);
}

void GeometryRenderer::createGrid(int size) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int halfSize = size / 2;

    // Linie poziome i pionowe
    for (int i = -halfSize; i <= halfSize; ++i) {
        // Linie poziome (szare)
        vertices.push_back({{static_cast<float>(-halfSize), 0.0f, static_cast<float>(i)},
                           {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}});
        vertices.push_back({{static_cast<float>(halfSize), 0.0f, static_cast<float>(i)},
                           {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}});

        // Linie pionowe (szare)
        vertices.push_back({{static_cast<float>(i), 0.0f, static_cast<float>(-halfSize)},
                           {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}});
        vertices.push_back({{static_cast<float>(i), 0.0f, static_cast<float>(halfSize)},
                           {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}});
    }

    for (unsigned int i = 0; i < vertices.size(); ++i) {
        indices.push_back(i);
    }

    setupMesh(m_gridMesh, vertices, indices);
}

// NOWE FUNKCJE DLA ZADAŃ Z INSTRUKCJI

void GeometryRenderer::drawIndexedCube() {
    std::cout << "Rysowanie indeksowanego sześcianu (Zadanie 2)" << std::endl;

    // Wykorzystanie istniejącej siatki
    glBindVertexArray(m_cubeMesh.VAO);
    glDrawElements(GL_TRIANGLES, m_cubeMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GeometryRenderer::drawComplexObject() {
    std::cout << "Rysowanie obiektu złożonego: litera A (Zadanie 3)" << std::endl;

    // Definicja wierzchołków litery A
    std::vector<Vertex> vertices = {
        // Podstawa A
        {{-0.3f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.1f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f}},
        {{0.1f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f}},
        {{0.3f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.2f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.3f, 0.5f}},
        {{0.2f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.7f, 0.5f}}
    };

    std::vector<unsigned int> indices = {
        0, 1, 4,    // Lewy trójkąt
        4, 1, 2,    // Górny trójkąt
        4, 2, 5,    // Środkowy trójkąt
        5, 2, 3     // Prawy trójkąt
    };

    // Tymczasowy mesh
    Mesh letterMesh;
    setupMesh(letterMesh, vertices, indices);

    // Rysowanie
    glBindVertexArray(letterMesh.VAO);
    glDrawElements(GL_TRIANGLES, letterMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Sprzątanie
    deleteMesh(letterMesh);
}

// Funkcje rysowania prymitywów (Zadanie 1)
void GeometryRenderer::drawPoints(const std::vector<glm::vec3>& points, float size) {
    glPointSize(size);
    glBegin(GL_POINTS);
    for (const auto& point : points) {
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
}

void GeometryRenderer::drawLines(const std::vector<glm::vec3>& vertices) {
    if (vertices.size() % 2 != 0) return;

    glBegin(GL_LINES);
    for (size_t i = 0; i < vertices.size(); i += 2) {
        glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
        glVertex3f(vertices[i+1].x, vertices[i+1].y, vertices[i+1].z);
    }
    glEnd();
}

void GeometryRenderer::drawLineStrip(const std::vector<glm::vec3>& vertices) {
    if (vertices.size() < 2) return;

    glBegin(GL_LINE_STRIP);
    for (const auto& v : vertices) {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
}

void GeometryRenderer::drawLineLoop(const std::vector<glm::vec3>& vertices) {
    if (vertices.size() < 2) return;

    glBegin(GL_LINE_LOOP);
    for (const auto& v : vertices) {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
}

void GeometryRenderer::drawTriangles(const std::vector<glm::vec3>& vertices) {
    if (vertices.size() % 3 != 0) return;

    glBegin(GL_TRIANGLES);
    for (const auto& v : vertices) {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
}

void GeometryRenderer::drawTriangleStrip(const std::vector<glm::vec3>& vertices) {
    if (vertices.size() < 3) return;

    glBegin(GL_TRIANGLE_STRIP);
    for (const auto& v : vertices) {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
}

void GeometryRenderer::drawTriangleFan(const std::vector<glm::vec3>& vertices) {
    if (vertices.size() < 3) return;

    glBegin(GL_TRIANGLE_FAN);
    for (const auto& v : vertices) {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
}

void GeometryRenderer::drawQuads(const std::vector<glm::vec3>& vertices) {
    if (vertices.size() % 4 != 0) return;

    glBegin(GL_QUADS);
    for (const auto& v : vertices) {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
}

void GeometryRenderer::setupOldOpenGLArrays(const Mesh& mesh) {
    // Przygotowanie tablic dla starego OpenGL (Rys. 10)
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> colors;

    for (const auto& vertex : mesh.vertices) {
        positions.push_back(vertex.position.x);
        positions.push_back(vertex.position.y);
        positions.push_back(vertex.position.z);

        normals.push_back(vertex.normal.x);
        normals.push_back(vertex.normal.y);
        normals.push_back(vertex.normal.z);

        colors.push_back(vertex.color.r);
        colors.push_back(vertex.color.g);
        colors.push_back(vertex.color.b);
    }

    // Włączenie tablic (jak w Rys. 10)
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Ustawienie wskaźników
    glVertexPointer(3, GL_FLOAT, 0, positions.data());
    glNormalPointer(GL_FLOAT, 0, normals.data());
    glColorPointer(3, GL_FLOAT, 0, colors.data());
}

void GeometryRenderer::drawCubeOldOpenGL() {
    std::cout << "Rysowanie sześcianu starym OpenGL (Rys. 10)" << std::endl;

    // Ustawienie tablic zgodnie z instrukcją
    setupOldOpenGLArrays(m_cubeMesh);

    // Rysowanie za pomocą glDrawElements
    glDrawElements(GL_TRIANGLES, m_cubeMesh.indices.size(),
                   GL_UNSIGNED_INT, m_cubeMesh.indices.data());

    // Wyłączenie tablic
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

// ORYGINALNE FUNKCJE RYSOWANIA (bez zmian)
void GeometryRenderer::drawCube(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);

    if (rotation.x != 0.0f) model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    if (rotation.y != 0.0f) model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    if (rotation.z != 0.0f) model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glBindVertexArray(m_cubeMesh.VAO);
    glDrawElements(m_drawMode, m_cubeMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GeometryRenderer::drawSphere(const glm::vec3& position, float radius) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius));

    glBindVertexArray(m_sphereMesh.VAO);
    glDrawElements(m_drawMode, m_sphereMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GeometryRenderer::drawCylinder(const glm::vec3& position, float height, float radius) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius, height, radius));

    glBindVertexArray(m_cylinderMesh.VAO);
    glDrawElements(m_drawMode, m_cylinderMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GeometryRenderer::drawCone(const glm::vec3& position, float height, float radius) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius, height, radius));

    glBindVertexArray(m_coneMesh.VAO);
    glDrawElements(m_drawMode, m_coneMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GeometryRenderer::drawPlane(const glm::vec3& position, const glm::vec2& size) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(size.x, 1.0f, size.y));

    glBindVertexArray(m_planeMesh.VAO);
    glDrawElements(m_drawMode, m_planeMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GeometryRenderer::drawTorus(const glm::vec3& position, float majorRadius, float minorRadius) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(majorRadius, majorRadius, majorRadius));

    glBindVertexArray(m_torusMesh.VAO);
    glDrawElements(m_drawMode, m_torusMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GeometryRenderer::drawPyramid(const glm::vec3& position, float baseSize, float height) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(baseSize, height, baseSize));

    glBindVertexArray(m_pyramidMesh.VAO);
    glDrawElements(m_drawMode, m_pyramidMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GeometryRenderer::drawGrid(const glm::vec3& position, int size, float spacing) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(spacing));

    // Zapisz tryb rysowania
    GLenum prevMode = m_drawMode;
    m_drawMode = GL_LINES;

    glBindVertexArray(m_gridMesh.VAO);
    glDrawElements(m_drawMode, m_gridMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Przywróć tryb rysowania
    m_drawMode = prevMode;
}

void GeometryRenderer::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
    float vertices[] = {
        start.x, start.y, start.z,
        end.x, end.y, end.z
    };

    glBindVertexArray(m_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void GeometryRenderer::drawPoint(const glm::vec3& position, float size, const glm::vec3& color) {
    float vertices[] = {position.x, position.y, position.z};

    glBindVertexArray(m_pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glPointSize(size);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
}

void GeometryRenderer::drawCoordinateSystem(float length) {
    // Oś X (czerwona)
    setColor(glm::vec3(1.0f, 0.0f, 0.0f));
    drawLine(glm::vec3(0.0f), glm::vec3(length, 0.0f, 0.0f));

    // Oś Y (zielona)
    setColor(glm::vec3(0.0f, 1.0f, 0.0f));
    drawLine(glm::vec3(0.0f), glm::vec3(0.0f, length, 0.0f));

    // Oś Z (niebieska)
    setColor(glm::vec3(0.0f, 0.0f, 1.0f));
    drawLine(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, length));

    // Strzałki
    setColor(glm::vec3(1.0f, 0.0f, 0.0f));
    drawArrow(glm::vec3(0.0f), glm::vec3(length, 0.0f, 0.0f), length * 0.1f);

    setColor(glm::vec3(0.0f, 1.0f, 0.0f));
    drawArrow(glm::vec3(0.0f), glm::vec3(0.0f, length, 0.0f), length * 0.1f);

    setColor(glm::vec3(0.0f, 0.0f, 1.0f));
    drawArrow(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, length), length * 0.1f);
}

void GeometryRenderer::draw3DGrid(const glm::vec3& center, int size, float spacing) {
    int halfSize = size / 2;

    // Pionowe linie w płaszczyźnie XZ
    for (int x = -halfSize; x <= halfSize; ++x) {
        glm::vec3 start(center.x + x * spacing, center.y, center.z - halfSize * spacing);
        glm::vec3 end(center.x + x * spacing, center.y, center.z + halfSize * spacing);
        drawLine(start, end, glm::vec3(0.5f));
    }

    for (int z = -halfSize; z <= halfSize; ++z) {
        glm::vec3 start(center.x - halfSize * spacing, center.y, center.z + z * spacing);
        glm::vec3 end(center.x + halfSize * spacing, center.y, center.z + z * spacing);
        drawLine(start, end, glm::vec3(0.5f));
    }
}

void GeometryRenderer::drawArrow(const glm::vec3& start, const glm::vec3& end, float headSize) {
    glm::vec3 direction = glm::normalize(end - start);
    glm::vec3 perpendicular = glm::vec3(-direction.z, 0.0f, direction.x);

    // Główna linia
    drawLine(start, end);

    // Główka strzałki
    glm::vec3 headBase = end - direction * headSize;
    glm::vec3 head1 = headBase + perpendicular * headSize * 0.5f;
    glm::vec3 head2 = headBase - perpendicular * headSize * 0.5f;

    drawLine(end, head1);
    drawLine(end, head2);
}

void GeometryRenderer::drawBox(const glm::vec3& min, const glm::vec3& max) {
    glm::vec3 vertices[8] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, max.y, max.z),
        glm::vec3(min.x, max.y, max.z)
    };

    // Rysowanie krawędzi
    drawLine(vertices[0], vertices[1]);
    drawLine(vertices[1], vertices[2]);
    drawLine(vertices[2], vertices[3]);
    drawLine(vertices[3], vertices[0]);

    drawLine(vertices[4], vertices[5]);
    drawLine(vertices[5], vertices[6]);
    drawLine(vertices[6], vertices[7]);
    drawLine(vertices[7], vertices[4]);

    drawLine(vertices[0], vertices[4]);
    drawLine(vertices[1], vertices[5]);
    drawLine(vertices[2], vertices[6]);
    drawLine(vertices[3], vertices[7]);
}

void GeometryRenderer::drawSphereWireframe(const glm::vec3& position, float radius, int segments) {
    // Poziome okręgi
    for (int i = 0; i < segments; ++i) {
        float theta1 = 2.0f * PI * i / segments;
        float theta2 = 2.0f * PI * (i + 1) / segments;

        for (int j = 0; j < segments; ++j) {
            float phi1 = PI * j / segments;
            float phi2 = PI * (j + 1) / segments;

            glm::vec3 p1 = position + radius * glm::vec3(
                sin(phi1) * cos(theta1),
                cos(phi1),
                sin(phi1) * sin(theta1)
            );

            glm::vec3 p2 = position + radius * glm::vec3(
                sin(phi2) * cos(theta1),
                cos(phi2),
                sin(phi2) * sin(theta1)
            );

            glm::vec3 p3 = position + radius * glm::vec3(
                sin(phi1) * cos(theta2),
                cos(phi1),
                sin(phi1) * sin(theta2)
            );

            drawLine(p1, p2);
            drawLine(p1, p3);
        }
    }
}

void GeometryRenderer::drawPolygon(const std::vector<glm::vec3>& vertices, bool filled) {
    if (vertices.size() < 3) return;

    if (filled) {
        // Rysowanie wypełnionego wielokąta
        // Tu potrzeba triangulacji, dla uproszczenia rysujemy jako linie
    }

    // Rysowanie konturu
    for (size_t i = 0; i < vertices.size(); ++i) {
        size_t next = (i + 1) % vertices.size();
        drawLine(vertices[i], vertices[next]);
    }
}

void GeometryRenderer::drawCircle(const glm::vec3& center, float radius, int segments) {
    std::vector<glm::vec3> vertices;

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * PI * i / segments;
        vertices.push_back(center + glm::vec3(
            cos(angle) * radius,
            0.0f,
            sin(angle) * radius
        ));
    }

    for (size_t i = 0; i < vertices.size() - 1; ++i) {
        drawLine(vertices[i], vertices[i + 1]);
    }
}

void GeometryRenderer::drawDisk(const glm::vec3& center, float innerRadius, float outerRadius, int segments) {
    for (int i = 0; i < segments; ++i) {
        float angle1 = 2.0f * PI * i / segments;
        float angle2 = 2.0f * PI * (i + 1) / segments;

        glm::vec3 inner1 = center + glm::vec3(cos(angle1) * innerRadius, 0.0f, sin(angle1) * innerRadius);
        glm::vec3 inner2 = center + glm::vec3(cos(angle2) * innerRadius, 0.0f, sin(angle2) * innerRadius);
        glm::vec3 outer1 = center + glm::vec3(cos(angle1) * outerRadius, 0.0f, sin(angle1) * outerRadius);
        glm::vec3 outer2 = center + glm::vec3(cos(angle2) * outerRadius, 0.0f, sin(angle2) * outerRadius);

        drawLine(inner1, inner2);
        drawLine(outer1, outer2);
        drawLine(inner1, outer1);
    }
}

void GeometryRenderer::setMaterial(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess) {
    m_currentMaterial.ambient = ambient;
    m_currentMaterial.diffuse = diffuse;
    m_currentMaterial.specular = specular;
    m_currentMaterial.shininess = shininess;
}

void GeometryRenderer::setColor(const glm::vec3& color) {
    setMaterial(color * 0.2f, color, color * 0.5f, 32.0f);
}

void GeometryRenderer::setDrawMode(GLenum mode) {
    m_drawMode = mode;
}

void GeometryRenderer::setModelMatrix(const glm::mat4& model) {
    // Implementacja w shaderze
}

void GeometryRenderer::setViewMatrix(const glm::mat4& view) {
    // Implementacja w shaderze
}

void GeometryRenderer::setProjectionMatrix(const glm::mat4& projection) {
    // Implementacja w shaderze
}

void GeometryRenderer::drawMesh(const Mesh& mesh) {
    glBindVertexArray(mesh.VAO);
    glDrawElements(m_drawMode, mesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}