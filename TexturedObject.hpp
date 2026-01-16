#ifndef TEXTURED_OBJECT_HPP
#define TEXTURED_OBJECT_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "BitmapHandler.hpp"

/**
 * @class TexturedObject
 * @brief Bazowa klasa dla obiektów 3D z teksturami
 *
 * Klasa abstrakcyjna dostarczająca podstawową funkcjonalność dla teksturowanych
 * obiektów 3D, w tym zarządzanie buforami, teksturami i transformacjami.
 */
class TexturedObject {
protected:
    GLuint m_VAO;           ///< Vertex Array Object ID
    GLuint m_VBO;           ///< Vertex Buffer Object ID
    GLuint m_EBO;           ///< Element Buffer Object ID
    GLuint m_textureID;     ///< ID tekstury OpenGL

    int m_vertexCount;      ///< Liczba wierzchołków w obiekcie
    int m_indexCount;       ///< Liczba indeksów w obiekcie

    glm::vec3 m_position;   ///< Pozycja obiektu w przestrzeni świata
    glm::vec3 m_rotation;   ///< Rotacja obiektu (kąty Eulera w stopniach)
    glm::vec3 m_scale;      ///< Skala obiektu

    std::shared_ptr<BitmapHandler> m_texture; ///< Wskaźnik do handlera tekstury

    /**
     * @brief Metoda wirtualna do konfiguracji buforów OpenGL
     *
     * Musi być zaimplementowana w klasach pochodnych. Odpowiada za stworzenie
     * i skonfigurowanie VAO, VBO i EBO dla konkretnego typu obiektu.
     */
    virtual void setupBuffers() = 0;

public:
    /**
     * @brief Konstruktor domyślny
     */
    TexturedObject();

    /**
     * @brief Destruktor wirtualny
     *
     * Zwalnia zasoby OpenGL (VAO, VBO, EBO).
     */
    virtual ~TexturedObject();

    // Ładowanie tekstury
    /**
     * @brief Ładuje teksturę z pliku
     * @param filePath Ścieżka do pliku tekstury
     * @return true jeśli ładowanie się powiodło, false w przeciwnym razie
     */
    bool loadTexture(const std::string& filePath);

    /**
     * @brief Ustawia wstępnie załadowaną teksturę
     * @param texture Wskaźnik do handlera tekstury
     */
    void setTexture(std::shared_ptr<BitmapHandler> texture);

    // Transformacje
    /**
     * @brief Ustawia pozycję obiektu
     * @param position Nowa pozycja w przestrzeni 3D
     */
    void setPosition(const glm::vec3& position);

    /**
     * @brief Ustawia rotację obiektu
     * @param rotation Kąty Eulera w stopniach (X, Y, Z)
     */
    void setRotation(const glm::vec3& rotation);

    /**
     * @brief Ustawia skalę obiektu
     * @param scale Współczynniki skali dla każdej osi
     */
    void setScale(const glm::vec3& scale);

    /**
     * @brief Przesuwa obiekt o podany wektor
     * @param translation Wektor translacji
     */
    void translate(const glm::vec3& translation);

    /**
     * @brief Obraca obiekt o podane kąty
     * @param rotation Kąty obrotu w stopniach (X, Y, Z)
     */
    void rotate(const glm::vec3& rotation);

    /**
     * @brief Skaluje obiekt o podane współczynniki
     * @param scale Współczynniki skali dla każdej osi
     */
    void scale(const glm::vec3& scale);

    // Gettery
    /**
     * @brief Pobiera pozycję obiektu
     * @return Pozycja obiektu
     */
    glm::vec3 getPosition() const { return m_position; }

    /**
     * @brief Pobiera rotację obiektu
     * @return Rotacja obiektu (kąty Eulera)
     */
    glm::vec3 getRotation() const { return m_rotation; }

    /**
     * @brief Pobiera skalę obiektu
     * @return Skala obiektu
     */
    glm::vec3 getScale() const { return m_scale; }

    /**
     * @brief Pobiera ID tekstury OpenGL
     * @return ID tekstury
     */
    GLuint getTextureID() const { return m_textureID; }

    // Renderowanie
    /**
     * @brief Renderuje obiekt bez wiązania tekstury
     *
     * Rysuje obiekt używając aktualnie związanej tekstury w shaderze.
     */
    virtual void draw() const;

    /**
     * @brief Renderuje obiekt z automatycznym wiązaniem tekstury
     *
     * Wiąże teksturę do jednostki teksturującej 0, a następnie rysuje obiekt.
     */
    virtual void drawWithTexture() const;

    /**
     * @brief Tworzy macierz modelu dla obiektu
     * @return Macierz 4x4 reprezentująca transformację modelu
     *
     * Macierz uwzględnia pozycję, rotację i skalę obiektu w kolejności:
     * 1. Skalowanie
     * 2. Rotacja
     * 3. Translacja
     */
    glm::mat4 getModelMatrix() const;
};

/**
 * @class TexturedCube
 * @brief Klasa reprezentująca teksturowany sześcian
 *
 * Dziedziczy po TexturedObject, implementuje geometrię sześcianu.
 */
class TexturedCube : public TexturedObject {
private:
    /**
     * @struct Vertex
     * @brief Struktura wierzchołka sześcianu
     */
    struct Vertex {
        glm::vec3 position; ///< Pozycja wierzchołka
        glm::vec3 normal;   ///< Normalna wierzchołka
        glm::vec2 texCoord; ///< Koordynaty tekstury
    };

    /**
     * @brief Implementacja konfiguracji buforów dla sześcianu
     */
    void setupBuffers() override;

public:
    /**
     * @brief Konstruktor domyślny
     */
    TexturedCube();

    /**
     * @brief Destruktor
     */
    ~TexturedCube();

    // Specyficzne dla sześcianu
    /**
     * @brief Tworzy geometrię sześcianu
     * @param size Rozmiar sześcianu (długość krawędzi), domyślnie 1.0
     */
    void create(float size = 1.0f);
};

/**
 * @class TexturedSphere
 * @brief Klasa reprezentująca teksturowaną kulę (sferę)
 *
 * Dziedziczy po TexturedObject, implementuje geometrię sfery.
 */
class TexturedSphere : public TexturedObject {
private:
    /**
     * @struct Vertex
     * @brief Struktura wierzchołka sfery
     */
    struct Vertex {
        glm::vec3 position; ///< Pozycja wierzchołka
        glm::vec3 normal;   ///< Normalna wierzchołka
        glm::vec2 texCoord; ///< Koordynaty tekstury
    };

    /**
     * @brief Implementacja konfiguracji buforów dla sfery
     */
    void setupBuffers() override;

public:
    /**
     * @brief Konstruktor domyślny
     */
    TexturedSphere();

    /**
     * @brief Destruktor
     */
    ~TexturedSphere();

    /**
     * @brief Tworzy geometrię sfery
     * @param radius Promień sfery, domyślnie 1.0
     * @param sectors Liczba sektorów (podział poziomy), domyślnie 32
     * @param stacks Liczba warstw (podział pionowy), domyślnie 32
     */
    void create(float radius = 1.0f, int sectors = 32, int stacks = 32);
};

/**
 * @class TexturedCylinder
 * @brief Klasa reprezentująca teksturowany cylinder
 *
 * Dziedziczy po TexturedObject, implementuje geometrię cylindra.
 */
class TexturedCylinder : public TexturedObject {
private:
    /**
     * @struct Vertex
     * @brief Struktura wierzchołka cylindra
     */
    struct Vertex {
        glm::vec3 position; ///< Pozycja wierzchołka
        glm::vec3 normal;   ///< Normalna wierzchołka
        glm::vec2 texCoord; ///< Koordynaty tekstury
    };

    /**
     * @brief Implementacja konfiguracji buforów dla cylindra
     */
    void setupBuffers() override;

public:
    /**
     * @brief Konstruktor domyślny
     */
    TexturedCylinder();

    /**
     * @brief Destruktor
     */
    ~TexturedCylinder();

    /**
     * @brief Tworzy geometrię cylindra
     * @param radius Promień podstawy, domyślnie 1.0
     * @param height Wysokość cylindra, domyślnie 2.0
     * @param sectors Liczba sektorów (podział okręgu), domyślnie 32
     */
    void create(float radius = 1.0f, float height = 2.0f, int sectors = 32);
};

#endif // TEXTURED_OBJECT_HPP