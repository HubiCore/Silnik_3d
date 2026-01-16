// ComplexObject.cpp
#include "ComplexObject.hpp"
#include <GL/glew.h>
#include <iostream>
#include <cmath>

/**
 * @brief Konstruktor domyślny ComplexObject
 *
 * Inicjalizuje pozycję, skalę, rotację i liczniki, zeruje identyfikatory OpenGL
 */
ComplexObject::ComplexObject()
    : position(0.0f), scale(1.0f), rotation(0.0f), vertexCount(0), triangleCount(0) {
    mesh.VAO = 0;
    mesh.VBO = 0;
    mesh.EBO = 0;
    mesh.indexCount = 0;
}

/**
 * @brief Destruktor ComplexObject
 *
 * Zwalnia zasoby graficzne (VAO, VBO, EBO) poprzez wywołanie deleteMesh()
 */
ComplexObject::~ComplexObject() {
    deleteMesh();
}

/**
 * @brief Tworzy literę H z trzech cylindrów
 * @param width Szerokość litery H
 * @param height Wysokość litery H
 * @param depth Głębokość litery H
 * @param color Kolor litery H
 *
 * @details Tworzy literę H składającą się z trzech cylindrów:
 * 1. Lewy pionowy cylinder
 * 2. Poziomy cylinder środkowy (obrócony o 90 stopni)
 * 3. Prawy pionowy cylinder
 */
void ComplexObject::createLetterH(float width, float height, float depth, const glm::vec3& color) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertexCount = 0;
    triangleCount = 0;

    // Parametry litery H
    float strokeWidth = width * 0.2f;
    float halfWidth = width / 2.0f;
    float cylinderRadius = strokeWidth / 2.0f;
    int sectors = 12;

    // Puszka 1 (lewo)
    glm::vec3 leftPos(-halfWidth + cylinderRadius, 0.0f, 0.0f);
    addCylinder(vertices, indices, leftPos, height, cylinderRadius, color, 0.0f, sectors);

    // Puszka 2 (środek)
    glm::vec3 centerPos(0.0f, 0.0f, 0.0f);
    addCylinder(vertices, indices, centerPos, width * 0.7, cylinderRadius, color, 90.0f, sectors);
    //to 0.7 to skala, żeby H ładniej wyglądało >///<

    // Puszka 3 (prawo)
    glm::vec3 rightPos(halfWidth - cylinderRadius, 0.0f, 0.0f);
    addCylinder(vertices, indices, rightPos, height, cylinderRadius, color, 0.0f, sectors);

    // Ustawienie siatki
    setupMesh(vertices, indices);
    vertexCount = vertices.size();
    triangleCount = indices.size() / 3;

}

/**
 * @brief Dodaje cylinder do obiektu
 * @param vertices Referencja do wektora wierzchołków (będzie modyfikowany)
 * @param indices Referencja do wektora indeksów (będzie modyfikowany)
 * @param position Pozycja cylindra
 * @param height Wysokość cylindra
 * @param radius Promień cylindra
 * @param color Kolor cylindra
 * @param rotationAngle Kąt obrotu cylindra wokół osi Z (w stopniach)
 * @param sectors Liczba sektorów (dokładność przybliżenia cylindra)
 *
 * @details Generuje wierzchołki i indeksy dla cylindra składającego się z:
 * - Dwóch podstaw (górnej i dolnej)
 * - Bocznej ściany
 * Każdy cylinder jest transformowany zgodnie z podaną pozycją, rotacją i skalą
 */
void ComplexObject::addCylinder(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                               const glm::vec3& position, float height, float radius,
                               const glm::vec3& color, float rotationAngle, int sectors) {
    const float PI = 3.14159265358979323846f;
    int baseIndex = vertices.size();

    float sectorStep = 2.0f * PI / sectors;

    // Transformacja cylindra
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, glm::vec3(radius, height, radius));

    // Transformacja normalnych (bez skalowania)
    glm::mat3 normalTransform = glm::mat3(glm::rotate(glm::mat4(1.0f),
                                                      glm::radians(rotationAngle),
                                                      glm::vec3(0.0f, 0.0f, 1.0f)));

    // Generowanie wierzchołków puszki
    std::vector<glm::vec3> localPositions;
    std::vector<glm::vec3> localNormals;
    std::vector<glm::vec2> texCoords;

    // Centra podstaw
    localPositions.push_back(glm::vec3(0.0f, 0.5f, 0.0f));   // górny środek
    localNormals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    texCoords.push_back(glm::vec2(0.5f, 0.5f));

    localPositions.push_back(glm::vec3(0.0f, -0.5f, 0.0f));  // dolny środek
    localNormals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    texCoords.push_back(glm::vec2(0.5f, 0.5f));

    // Wierzchołki obwodu
    for (int i = 0; i <= sectors; ++i) {
        float angle = i * sectorStep;
        float x = cosf(angle);
        float z = sinf(angle);

        glm::vec3 sideNormal = glm::normalize(glm::vec3(x, 0.0f, z));

        // górna podstawa
        localPositions.push_back(glm::vec3(x, 0.5f, z));
        localNormals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        texCoords.push_back(glm::vec2(x * 0.5f + 0.5f, z * 0.5f + 0.5f));

        // dolna podstawa
        localPositions.push_back(glm::vec3(x, -0.5f, z));
        localNormals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
        texCoords.push_back(glm::vec2(x * 0.5f + 0.5f, z * 0.5f + 0.5f));

        // ściana górna
        localPositions.push_back(glm::vec3(x, 0.5f, z));
        localNormals.push_back(sideNormal);
        texCoords.push_back(glm::vec2(static_cast<float>(i) / sectors, 1.0f));

        // ściana dolna
        localPositions.push_back(glm::vec3(x, -0.5f, z));
        localNormals.push_back(sideNormal);
        texCoords.push_back(glm::vec2(static_cast<float>(i) / sectors, 0.0f));
    }

    for (size_t i = 0; i < localPositions.size(); ++i) {
        Vertex v;
        glm::vec4 pos(localPositions[i], 1.0f);
        pos = transform * pos;
        v.position = glm::vec3(pos);

        v.normal = glm::normalize(normalTransform * localNormals[i]);
        v.texCoord = texCoords[i];

        vertices.push_back(v);
    }

    for (int i = 0; i < sectors; ++i) {
        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 2 + (i + 1) * 4);
        indices.push_back(baseIndex + 2 + i * 4);
    }

    for (int i = 0; i < sectors; ++i) {
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 3 + i * 4);
        indices.push_back(baseIndex + 3 + (i + 1) * 4);
    }

    for (int i = 0; i < sectors; ++i) {
        int base = baseIndex + 2 + i * 4;
        int next = baseIndex + 2 + (i + 1) * 4;

        indices.push_back(base + 2);
        indices.push_back(next + 2);
        indices.push_back(base + 3);

        indices.push_back(base + 3);
        indices.push_back(next + 2);
        indices.push_back(next + 3);
    }
}

/**
 * @brief Konfiguruje siatkę 3D z podanych wierzchołków i indeksów
 * @param vertices Wektor wierzchołków
 * @param indices Wektor indeksów
 *
 * @details Tworzy VAO, VBO i EBO w OpenGL, przesyła dane do GPU
 * i konfiguruje atrybuty wierzchołków (pozycja, normalna, UV)
 */
void ComplexObject::setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    deleteMesh();

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Atrybuty wierzchołków
    // Pozycja (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normalna (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Współrzędne tekstury (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
    mesh.indexCount = static_cast<int>(indices.size());
}

/**
 * @brief Usuwa zasoby siatki 3D (VAO, VBO, EBO)
 */
void ComplexObject::deleteMesh() {
    if (mesh.VAO) {
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBO);
        glDeleteBuffers(1, &mesh.EBO);
        mesh.VAO = mesh.VBO = mesh.EBO = 0;
        mesh.indexCount = 0;
    }
}

/**
 * @brief Rysuje złożony obiekt
 *
 * @details Używa VAO i EBO do narysowania obiektu za pomocą glDrawElements
 * Jeśli siatka nie jest zainicjalizowana, wypisuje błąd
 */
void ComplexObject::draw() const {
    if (mesh.VAO == 0 || mesh.indexCount == 0) {
        std::cerr << "Błąd: Jeśli to czytasz to zainicjalizuj litere H w main.cpp" << std::endl;
        return;
    }

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/**
 * @brief Ustawia pozycję obiektu
 * @param pos Nowa pozycja obiektu
 */
void ComplexObject::setPosition(const glm::vec3& pos) { position = pos; }

/**
 * @brief Ustawia skalę obiektu
 * @param scl Nowa skala obiektu
 */
void ComplexObject::setScale(const glm::vec3& scl) { scale = scl; }

/**
 * @brief Ustawia rotację obiektu
 * @param rot Nowa rotacja obiektu (kąty Eulera w stopniach)
 */
void ComplexObject::setRotation(const glm::vec3& rot) { rotation = rot; }

/**
 * @brief Zwraca macierz modelu obiektu
 * @return Macierz transformacji modelu
 *
 * @details Tworzy macierz modelu na podstawie pozycji, rotacji i skali
 * Kolejność transformacji: translacja -> obrót (Z, Y, X) -> skalowanie
 */
glm::mat4 ComplexObject::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    if (rotation.z != 0.0f)
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    if (rotation.y != 0.0f)
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    if (rotation.x != 0.0f)
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

    model = glm::scale(model, scale);
    return model;
}