#ifndef COMPLEX_OBJECT_HPP
#define COMPLEX_OBJECT_HPP
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <glm/glm.hpp>
#include "GeometryRenderer.hpp"

/**
 * @class ComplexObject
 * @brief Klasa reprezentująca złożony obiekt 3D składający się z wielu prymitywów
 *
 * Obsługuje tworzenie i renderowanie złożonych kształtów 3D, takich jak litery.
 * Aktualnie implementuje tworzenie litery H z cylindrów.
 */
class ComplexObject {
public:
    /**
     * @brief Konstruktor domyślny ComplexObject
     */
    ComplexObject();

    /**
     * @brief Destruktor ComplexObject
     *
     * Zwalnia zasoby graficzne (VAO, VBO, EBO)
     */
    ~ComplexObject();

    /**
     * @brief Tworzy literę H z cylindrów
     * @param width Szerokość litery H
     * @param height Wysokość litery H
     * @param depth Głębokość litery H
     * @param color Kolor litery H (domyślnie czerwony)
     */
    void createLetterH(float width, float height, float depth, const glm::vec3& color = glm::vec3(0.9f, 0.2f, 0.2f));

    /**
     * @brief Rysuje złożony obiekt
     */
    void draw() const;

    // Transformacje

    /**
     * @brief Ustawia pozycję obiektu
     * @param position Nowa pozycja obiektu
     */
    void setPosition(const glm::vec3& position);

    /**
     * @brief Ustawia skalę obiektu
     * @param scale Nowa skala obiektu
     */
    void setScale(const glm::vec3& scale);

    /**
     * @brief Ustawia rotację obiektu
     * @param rotation Nowa rotacja obiektu (kąty Eulera w stopniach)
     */
    void setRotation(const glm::vec3& rotation);

    /**
     * @brief Zwraca macierz modelu obiektu
     * @return Macierz transformacji modelu
     */
    glm::mat4 getModelMatrix() const;

    /**
     * @brief Zwraca pozycję obiektu
     * @return Pozycja obiektu
     */
    glm::vec3 getPosition() const { return position; }

    /**
     * @brief Zwraca skalę obiektu
     * @return Skala obiektu
     */
    glm::vec3 getScale() const { return scale; }

    /**
     * @brief Zwraca rotację obiektu
     * @return Rotacja obiektu (kąty Eulera w stopniach)
     */
    glm::vec3 getRotation() const { return rotation; }

    /**
     * @brief Zwraca liczbę wierzchołków w obiekcie
     * @return Liczba wierzchołków
     */
    int getVertexCount() const { return vertexCount; }

    /**
     * @brief Zwraca liczbę trójkątów w obiekcie
     * @return Liczba trójkątów
     */
    int getTriangleCount() const { return triangleCount; }

private:
    Mesh mesh;                    /**< Struktura siatki 3D (VAO, VBO, EBO) */
    glm::vec3 position;          /**< Pozycja obiektu w przestrzeni świata */
    glm::vec3 scale;             /**< Skala obiektu */
    glm::vec3 rotation;          /**< Rotacja obiektu (kąty Eulera w stopniach) */
    int vertexCount;             /**< Liczba wierzchołków w obiekcie */
    int triangleCount;           /**< Liczba trójkątów w obiekcie */

    /**
     * @brief Konfiguruje siatkę 3D z podanych wierzchołków i indeksów
     * @param vertices Wektor wierzchołków
     * @param indices Wektor indeksów
     */
    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    /**
     * @brief Usuwa zasoby siatki 3D (VAO, VBO, EBO)
     */
    void deleteMesh();

    /**
     * @brief Dodaje prostopadłościan do obiektu
     * @param vertices Referencja do wektora wierzchołków (będzie modyfikowany)
     * @param indices Referencja do wektora indeksów (będzie modyfikowany)
     * @param position Pozycja prostopadłościanu
     * @param size Rozmiar prostopadłościanu
     * @param color Kolor prostopadłościanu
     * @param uvScale Skala współrzędnych tekstury (domyślnie 1.0)
     */
    void addCuboid(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                  const glm::vec3& position, const glm::vec3& size, const glm::vec3& color,
                  const glm::vec2& uvScale = glm::vec2(1.0f));

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
     */
    void addCylinder(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                    const glm::vec3& position, float height, float radius,
                    const glm::vec3& color, float rotationAngle, int sectors);
};

#endif