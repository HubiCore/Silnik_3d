#define GLEW_STATIC
#include <GL/glew.h>
#include "GeometryRenderer.hpp"
#include <cmath>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265358979323846f

/**
 * @brief Konstruktor GeometryRenderer
 *
 * Inicjalizuje tryb rysowania i domyślne właściwości materiału
 */
GeometryRenderer::GeometryRenderer() : m_drawMode(GL_TRIANGLES) {
    m_currentMaterial.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    m_currentMaterial.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    m_currentMaterial.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    m_currentMaterial.shininess = 32.0f;
}

/**
 * @brief Destruktor GeometryRenderer
 *
 * Zwalnia wszystkie zasoby OpenGL (VAO, VBO, EBO)
 */
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

/**
 * @brief Inicjalizuje renderer geometryczny
 * @return true jeśli inicjalizacja się powiodła, false w przeciwnym razie
 *
 * @details Inicjalizuje GLEW, tworzy wszystkie podstawowe kształty geometryczne
 * i inicjalizuje bufory dla linii i punktów.
 */
bool GeometryRenderer::initialize() {
    // Inicjalizacja GLEW (jeśli potrzebne)
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

    // Inicjalizacja buforów dla linii i punktów
    glGenVertexArrays(1, &m_lineVAO);
    glGenBuffers(1, &m_lineVBO);

    glGenVertexArrays(1, &m_pointVAO);
    glGenBuffers(1, &m_pointVBO);

    std::cout << "GeometryRenderer zainicjalizowany" << std::endl;
    return true;
}

/**
 * @brief Konfiguruje siatkę 3D z podanych wierzchołków i indeksów
 * @param mesh Referencja do struktury Mesh
 * @param vertices Wektor wierzchołków
 * @param indices Wektor indeksów
 *
 * @details Tworzy VAO, VBO i EBO w OpenGL, przesyła dane do GPU
 * i konfiguruje atrybuty wierzchołków (pozycja, normalna, UV).
 */
void GeometryRenderer::setupMesh(Mesh& mesh, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    // Wierzchołki
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // Indeksy
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Atrybuty wierzchołków
    // Pozycja
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normalna
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Koordynaty tekstury
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
    mesh.indexCount = static_cast<int>(indices.size());
}

/**
 * @brief Usuwa zasoby siatki 3D
 * @param mesh Referencja do struktury Mesh
 */
void GeometryRenderer::deleteMesh(Mesh& mesh) {
    glDeleteVertexArrays(1, &mesh.VAO);
    glDeleteBuffers(1, &mesh.VBO);
    glDeleteBuffers(1, &mesh.EBO);
}

/**
 * @brief Tworzy siatkę sześcianu jednostkowego
 *
 * @details Tworzy sześcian o rozmiarze 1x1x1 ze środkiem w (0,0,0).
 * Każda ściana ma normalną skierowaną na zewnątrz i współrzędne UV.
 */
void GeometryRenderer::createCube() {
    std::vector<Vertex> vertices = {
        // Front
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

        // Back
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

        // Top
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},

        // Bottom
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},

        // Right
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

        // Left
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}
    };

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

/**
 * @brief Tworzy siatkę sfery jednostkowej
 * @param sectors Liczba sektorów (dokładność wokół osi Z)
 * @param stacks Liczba warstw (dokładność wzdłuż osi Y)
 *
 * @details Tworzy sferę o promieniu 1 metodą parametryczną (phi i theta).
 * Wykorzystuje parametryczne równania sfery.
 */
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

/**
 * @brief Tworzy siatkę cylindra jednostkowego
 * @param sectors Liczba sektorów (dokładność okręgu)
 *
 * @details Tworzy cylinder o wysokości 1 i promieniu 1.
 * Składa się z dwóch podstaw (górnej i dolnej) i ściany bocznej.
 * Wykorzystuje poprawny winding order (CCW) dla wszystkich trójkątów.
 */
void GeometryRenderer::createCylinder(int sectors) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2.0f * PI / sectors;

    // Centra podstaw
    vertices.push_back({{0.0f, 0.5f, 0.0f},  {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}});   // 0
    vertices.push_back({{0.0f, -0.5f, 0.0f}, {0.0f,-1.0f, 0.0f}, {0.5f, 0.5f}});   // 1

    // Wierzchołki obwodu
    for (int i = 0; i <= sectors; ++i) {
        float angle = i * sectorStep;
        float x = cosf(angle);
        float z = sinf(angle);

        glm::vec3 sideNormal = glm::normalize(glm::vec3(x, 0.0f, z));

        // top
        vertices.push_back({{x,  0.5f, z}, {0,1,0}, {x*0.5f+0.5f, z*0.5f+0.5f}});
        // bottom
        vertices.push_back({{x, -0.5f, z}, {0,-1,0},{x*0.5f+0.5f, z*0.5f+0.5f}});
        // side top
        vertices.push_back({{x,  0.5f, z}, sideNormal, {(float)i/sectors, 1.0f}});
        // side bottom
        vertices.push_back({{x, -0.5f, z}, sideNormal, {(float)i/sectors, 0.0f}});
    }

    // GÓRNA PODSTAWA (CCW)
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(0);
        indices.push_back(2 + (i+1)*4);
        indices.push_back(2 + i*4);
    }

    // DOLNA PODSTAWA (CCW)
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(1);
        indices.push_back(3 + i*4);
        indices.push_back(3 + (i+1)*4);
    }

    // ŚCIANY BOCZNE – 🔥 POPRAWIONY WINDING (ale dospermiona emotka)
    for (int i = 0; i < sectors; ++i) {
        int base = 2 + i * 4;
        int next = 2 + (i + 1) * 4;

        // triangle 1
        indices.push_back(base + 2);
        indices.push_back(next + 2);
        indices.push_back(base + 3);

        // triangle 2
        indices.push_back(base + 3);
        indices.push_back(next + 2);
        indices.push_back(next + 3);
    }

    setupMesh(m_cylinderMesh, vertices, indices);
}

/**
 * @brief Tworzy siatkę stożka jednostkowego
 * @param sectors Liczba sektorów (dokładność okręgu)
 *
 * @details Tworzy stożek o wysokości 1 i promieniu podstawy 1.
 * Składa się z podstawy i ściany bocznej zbiegającej się w wierzchołku.
 */
void GeometryRenderer::createCone(int sectors) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2 * PI / sectors;

    // Wierzchołek stożka
    vertices.push_back({{0.0f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}});
    // Środek podstawy
    vertices.push_back({{0.0f, -0.5f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f}});

    for (int i = 0; i <= sectors; ++i) {
        float angle = i * sectorStep;
        float x = cosf(angle);
        float z = sinf(angle);

        // Podstawa
        vertices.push_back({{x, -0.5f, z}, {0.0f, -1.0f, 0.0f}, {x * 0.5f + 0.5f, z * 0.5f + 0.5f}});

        // Ściana
        glm::vec3 normal = glm::normalize(glm::vec3(x, 0.25f, z));
        vertices.push_back({{x, -0.5f, z}, normal, {static_cast<float>(i) / sectors, 0.0f}});
        vertices.push_back({{0.0f, 0.5f, 0.0f}, normal, {static_cast<float>(i) / sectors, 1.0f}});
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

/**
 * @brief Tworzy siatkę płaszczyzny jednostkowej
 *
 * @details Tworzy kwadratową płaszczyznę o rozmiarze 1x1 w płaszczyźnie XZ.
 * Normalna skierowana jest w górę (wzdłuż osi Y).
 */
void GeometryRenderer::createPlane() {
    std::vector<Vertex> vertices = {
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0
    };

    setupMesh(m_planeMesh, vertices, indices);
}

/**
 * @brief Tworzy siatkę torusa
 * @param radius Główny promień torusa
 * @param tubeRadius Promień rury torusa
 * @param sectors Liczba sektorów
 * @param rings Liczba pierścieni
 *
 * @details Tworzy torus metodą parametryczną (dwa kąty).
 * Torus jest podobny do obwarzanka lub dętki.
 */
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

/**
 * @brief Tworzy siatkę piramidy (ostrosłupa kwadratowego)
 *
 * @details Tworzy piramidę o podstawie kwadratowej i wysokości 1.
 * Każda ściana boczna jest osobno triangulowana z poprawnymi normalnymi.
 */
void GeometryRenderer::createPyramid() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 base[4] = {
        {-0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f,  0.5f},
        {-0.5f, -0.5f,  0.5f}
    };

    glm::vec3 apex = {0.0f, 0.5f, 0.0f};

    /* ===== PODSTAWA ===== */
    int baseStart = 0;
    for (int i = 0; i < 4; ++i) {
        vertices.push_back({
            base[i],
            {0.0f, -1.0f, 0.0f},
            {0.0f, 0.0f}
        });
    }

    /* ===== ŚCIANY BOCZNE ===== */
    for (int i = 0; i < 4; ++i) {
        int next = (i + 1) % 4;

        // UWAGA: KOLEJNOŚĆ MA ZNACZENIE
        glm::vec3 edge1 = apex - base[i];
        glm::vec3 edge2 = base[next] - base[i];
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        int start = vertices.size();

        // osobne wierzchołki
        vertices.push_back({ base[i],    normal, {0.0f, 0.0f} });
        vertices.push_back({ apex,       normal, {0.5f, 1.0f} });
        vertices.push_back({ base[next], normal, {1.0f, 0.0f} });


        indices.push_back(start + 0); // base[i]
        indices.push_back(start + 1); // apex
        indices.push_back(start + 2); // base[next]
    }

    setupMesh(m_pyramidMesh, vertices, indices);
}

/**
 * @brief Tworzy siatkę pomocniczej siatki 2D
 * @param size Rozmiar siatki (liczba linii)
 *
 * @details Tworzy siatkę składającą się z linii poziomych i pionowych.
 * Używana jako pomoc wizualna w scenach 3D.
 */
void GeometryRenderer::createGrid(int size) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int halfSize = size / 2;

    // Linie poziome i pionowe
    for (int i = -halfSize; i <= halfSize; ++i) {
        // Linie poziome
        vertices.push_back({{static_cast<float>(-halfSize), 0.0f, static_cast<float>(i)},
                           {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
        vertices.push_back({{static_cast<float>(halfSize), 0.0f, static_cast<float>(i)},
                           {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}});

        // Linie pionowe
        vertices.push_back({{static_cast<float>(i), 0.0f, static_cast<float>(-halfSize)},
                           {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
        vertices.push_back({{static_cast<float>(i), 0.0f, static_cast<float>(halfSize)},
                           {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}});
    }

    for (unsigned int i = 0; i < vertices.size(); ++i) {
        indices.push_back(i);
    }

    setupMesh(m_gridMesh, vertices, indices);
}

/**
 * @brief Rysuje sześcian z transformacją
 * @param position Pozycja sześcianu
 * @param scale Skala sześcianu
 * @param rotation Rotacja sześcianu (kąty Eulera w stopniach)
 */
void GeometryRenderer::drawCube(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);

    if (rotation.x != 0.0f) model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    if (rotation.y != 0.0f) model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    if (rotation.z != 0.0f) model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Ustaw macierz modelu w shaderze
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(m_cubeMesh.VAO);
    glDrawElements(m_drawMode, m_cubeMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/**
 * @brief Rysuje sferę
 * @param position Pozycja środka sfery
 * @param radius Promień sfery
 */
void GeometryRenderer::drawSphere(const glm::vec3& position, float radius) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius));

    // Ustaw macierz modelu w shaderze
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(m_sphereMesh.VAO);
    glDrawElements(m_drawMode, m_sphereMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/**
 * @brief Rysuje cylinder
 * @param position Pozycja środka cylindra
 * @param height Wysokość cylindra
 * @param radius Promień cylindra
 */
void GeometryRenderer::drawCylinder(const glm::vec3& position, float height, float radius) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius, height, radius));

    // Ustaw macierz modelu w shaderze
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(m_cylinderMesh.VAO);
    glDrawElements(m_drawMode, m_cylinderMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/**
 * @brief Rysuje stożek
 * @param position Pozycja środka stożka
 * @param height Wysokość stożka
 * @param radius Promień podstawy stożka
 */
void GeometryRenderer::drawCone(const glm::vec3& position, float height, float radius) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius, height, radius));

    // Ustaw macierz modelu w shaderze
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(m_coneMesh.VAO);
    glDrawElements(m_drawMode, m_coneMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/**
 * @brief Rysuje płaszczyznę
 * @param position Pozycja środka płaszczyzny
 * @param size Rozmiar płaszczyzny (szerokość, głębokość)
 */
void GeometryRenderer::drawPlane(const glm::vec3& position, const glm::vec2& size) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(size.x, 1.0f, size.y));

    // Ustaw macierz modelu w shaderze
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(m_planeMesh.VAO);
    glDrawElements(m_drawMode, m_planeMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/**
 * @brief Rysuje torus
 * @param position Pozycja środka torusa
 * @param majorRadius Główny promień torusa
 * @param minorRadius Promień rury torusa
 */
void GeometryRenderer::drawTorus(const glm::vec3& position, float majorRadius, float minorRadius) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(majorRadius, majorRadius, majorRadius));

    // Ustaw macierz modelu w shaderze
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(m_torusMesh.VAO);
    glDrawElements(m_drawMode, m_torusMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/**
 * @brief Rysuje piramidę
 * @param position Pozycja środka podstawy piramidy
 * @param baseSize Rozmiar podstawy piramidy
 * @param height Wysokość piramidy
 */
void GeometryRenderer::drawPyramid(const glm::vec3& position, float baseSize, float height) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(baseSize, height, baseSize));

    // Ustaw macierz modelu w shaderze
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(m_pyramidMesh.VAO);
    glDrawElements(m_drawMode, m_pyramidMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/**
 * @brief Rysuje siatkę pomocniczą
 * @param position Pozycja środka siatki
 * @param size Rozmiar siatki (liczba komórek)
 * @param spacing Odstęp między liniami siatki
 *
 * @details Tymczasowo zmienia tryb rysowania na GL_LINES, a następnie przywraca poprzedni.
 */
void GeometryRenderer::drawGrid(const glm::vec3& position, int size, float spacing) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(spacing));

    // Zapisz tryb rysowania
    GLenum prevMode = m_drawMode;
    m_drawMode = GL_LINES;

    // Ustaw macierz modelu w shaderze
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(m_gridMesh.VAO);
    glDrawElements(m_drawMode, m_gridMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Przywróć tryb rysowania
    m_drawMode = prevMode;
}

/**
 * @brief Rysuje linię pomiędzy dwoma punktami
 * @param start Punkt początkowy linii
 * @param end Punkt końcowy linii
 * @param color Kolor linii (domyślnie biały)
 */
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

/**
 * @brief Rysuje punkt
 * @param position Pozycja punktu
 * @param size Rozmiar punktu (domyślnie 5.0)
 * @param color Kolor punktu (domyślnie biały)
 */
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

/**
 * @brief Rysuje układ współrzędnych 3D
 * @param length Długość osi (domyślnie 1.0)
 *
 * @details Rysuje osie X (czerwona), Y (zielona), Z (niebieska) ze strzałkami.
 */
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

/**
 * @brief Rysuje 3-wymiarową siatkę pomocniczą
 * @param center Środek siatki
 * @param size Rozmiar siatki (liczba komórek)
 * @param spacing Odstęp między liniami siatki
 */
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

/**
 * @brief Rysuje strzałkę
 * @param start Punkt początkowy strzałki
 * @param end Punkt końcowy strzałki
 * @param headSize Rozmiar główki strzałki (domyślnie 0.1)
 *
 * @details Rysuje linię główną i trójkątną główkę strzałki.
 */
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

/**
 * @brief Rysuje prostopadłościan (kontur)
 * @param min Minimalny punkt (lewy-dolny-tylny)
 * @param max Maksymalny punkt (prawy-górny-przedni)
 */
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

/**
 * @brief Rysuje szkielet sfery (wireframe)
 * @param position Pozycja środka sfery
 * @param radius Promień sfery
 * @param segments Liczba segmentów (dokładność)
 */
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

/**
 * @brief Rysuje wielokąt
 * @param vertices Lista wierzchołków wielokąta
 * @param filled Czy wypełnić wielokąt (domyślnie false)
 */
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

/**
 * @brief Rysuje okrąg w płaszczyźnie XZ
 * @param center Środek okręgu
 * @param radius Promień okręgu
 * @param segments Liczba segmentów (dokładność)
 */
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

/**
 * @brief Rysuje pierścień (dysk z otworem) w płaszczyźnie XZ
 * @param center Środek pierścienia
 * @param innerRadius Wewnętrzny promień
 * @param outerRadius Zewnętrzny promień
 * @param segments Liczba segmentów (dokładność)
 */
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

/**
 * @brief Rysuje "harnas" (funkcja eksperymentalna)
 * @param position Pozycja harnasa
 * @param height Wysokość harnasa
 * @param radius Promień harnasa
 *
 * @note Funkcja w budowie - obecnie pusta implementacja
 */
void GeometryRenderer::drawHarnas(const glm::vec3& position, float height, float radius) {
    //NAHHHHHHH BRU HB URUASNR
}

/**
 * @brief Ustawia właściwości materiału
 * @param ambient Składowa otoczenia
 * @param diffuse Składowa rozproszenia
 * @param specular Składowa odbicia
 * @param shininess Współczynnik połysku
 *
 * @note Wymaga implementacji uniformów w shaderze
 */
void GeometryRenderer::setMaterial(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess) {
    m_currentMaterial.ambient = ambient;
    m_currentMaterial.diffuse = diffuse;
    m_currentMaterial.specular = specular;
    m_currentMaterial.shininess = shininess;

    // Ustaw materiały w shaderze
    // glUniform3fv(ambientLoc, 1, glm::value_ptr(ambient));
    // glUniform3fv(diffuseLoc, 1, glm::value_ptr(diffuse));
    // glUniform3fv(specularLoc, 1, glm::value_ptr(specular));
    // glUniform1f(shininessLoc, shininess);
}

/**
 * @brief Ustawia kolor dla wszystkich składowych materiału
 * @param color Kolor bazowy
 *
 * @details Ambient = 20% koloru, Diffuse = 100% koloru, Specular = 50% koloru
 */
void GeometryRenderer::setColor(const glm::vec3& color) {
    // Dla prostoty ustawiamy ten sam kolor dla wszystkich składników
    setMaterial(color * 0.2f, color, color * 0.5f, 32.0f);
}

/**
 * @brief Ustawia tryb rysowania OpenGL
 * @param mode Tryb rysowania (GL_TRIANGLES, GL_LINES, GL_POINTS itp.)
 */
void GeometryRenderer::setDrawMode(GLenum mode) {
    m_drawMode = mode;
}

/**
 * @brief Ustawia macierz modelu (do implementacji w shaderze)
 * @param model Macierz modelu
 */
void GeometryRenderer::setModelMatrix(const glm::mat4& model) {
    // Ustaw w shaderze
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

/**
 * @brief Ustawia macierz widoku (do implementacji w shaderze)
 * @param view Macierz widoku
 */
void GeometryRenderer::setViewMatrix(const glm::mat4& view) {
    // Ustaw w shaderze
    // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

/**
 * @brief Ustawia macierz rzutowania (do implementacji w shaderze)
 * @param projection Macierz rzutowania
 */
void GeometryRenderer::setProjectionMatrix(const glm::mat4& projection) {
    // Ustaw w shaderze
    // glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

/**
 * @brief Rysuje dowolną siatkę Mesh
 * @param mesh Referencja do siatki Mesh
 */
void GeometryRenderer::drawMesh(const Mesh& mesh) {
    glBindVertexArray(mesh.VAO);
    glDrawElements(m_drawMode, mesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}