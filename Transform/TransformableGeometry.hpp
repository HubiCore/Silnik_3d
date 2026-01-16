// TransformableGeometry.hpp
#ifndef TRANSFORMABLE_GEOMETRY_HPP
#define TRANSFORMABLE_GEOMETRY_HPP

#include "TransformableObject.hpp"
#include <glm/glm.hpp>
#include <memory>

// Forward declaration
class ComplexObject;

/**
 * @class CubeObject
 * @brief Klasa reprezentująca sześcian jako transformowalny obiekt
 *
 * Dziedziczy po TransformableObject, umożliwia rysowanie kolorowego sześcianu
 */
class CubeObject : public TransformableObject {
private:
    glm::vec3 m_color;

public:
    /**
     * @brief Konstruktor CubeObject
     * @param color Kolor sześcianu (domyślnie pomarańczowy)
     */
    CubeObject(const glm::vec3& color = glm::vec3(1.0f, 0.5f, 0.2f));

    /**
     * @brief Rysuje sześcian
     * @override
     */
    void draw() const override;

    /**
     * @brief Zwraca kolor sześcianu
     * @return Aktualny kolor obiektu
     */
    glm::vec3 getColor() const override { return m_color; }

    /**
     * @brief Ustawia nowy kolor sześcianu
     * @param color Nowy kolor
     */
    void setColor(const glm::vec3& color) override { m_color = color; }
};

/**
 * @class SphereObject
 * @brief Klasa reprezentująca sferę jako transformowalny obiekt
 *
 * Dziedziczy po TransformableObject, umożliwia rysowanie kolorowej sfery
 */
class SphereObject : public TransformableObject {
private:
    float m_radius;
    glm::vec3 m_color;

public:
    /**
     * @brief Konstruktor SphereObject
     * @param radius Promień sfery (domyślnie 1.0)
     * @param color Kolor sfery (domyślnie zielony)
     */
    SphereObject(float radius = 1.0f, const glm::vec3& color = glm::vec3(0.2f, 0.8f, 0.2f));

    /**
     * @brief Rysuje sferę
     * @override
     */
    void draw() const override;

    /**
     * @brief Zwraca promień sfery
     * @return Aktualny promień
     */
    float getRadius() const { return m_radius; }

    /**
     * @brief Ustawia nowy promień sfery
     * @param radius Nowy promień
     */
    void setRadius(float radius) { m_radius = radius; }

    /**
     * @brief Zwraca kolor sfery
     * @return Aktualny kolor obiektu
     */
    glm::vec3 getColor() const override { return m_color; }

    /**
     * @brief Ustawia nowy kolor sfery
     * @param color Nowy kolor
     */
    void setColor(const glm::vec3& color) override { m_color = color; }
};

/**
 * @class CylinderObject
 * @brief Klasa reprezentująca cylinder jako transformowalny obiekt
 *
 * Dziedziczy po TransformableObject, umożliwia rysowanie kolorowego cylindra
 */
class CylinderObject : public TransformableObject {
private:
    float m_height;
    float m_radius;
    glm::vec3 m_color;

public:
    /**
     * @brief Konstruktor CylinderObject
     * @param height Wysokość cylindra (domyślnie 2.0)
     * @param radius Promień cylindra (domyślnie 0.5)
     * @param color Kolor cylindra (domyślnie niebieski)
     */
    CylinderObject(float height = 2.0f, float radius = 0.5f,
                   const glm::vec3& color = glm::vec3(0.2f, 0.5f, 1.0f));

    /**
     * @brief Rysuje cylinder
     * @override
     */
    void draw() const override;

    /**
     * @brief Zwraca wysokość cylindra
     * @return Aktualna wysokość
     */
    float getHeight() const { return m_height; }

    /**
     * @brief Zwraca promień cylindra
     * @return Aktualny promień
     */
    float getRadius() const { return m_radius; }

    /**
     * @brief Ustawia nową wysokość cylindra
     * @param height Nowa wysokość
     */
    void setHeight(float height) { m_height = height; }

    /**
     * @brief Ustawia nowy promień cylindra
     * @param radius Nowy promień
     */
    void setRadius(float radius) { m_radius = radius; }

    /**
     * @brief Zwraca kolor cylindra
     * @return Aktualny kolor obiektu
     */
    glm::vec3 getColor() const override { return m_color; }

    /**
     * @brief Ustawia nowy kolor cylindra
     * @param color Nowy kolor
     */
    void setColor(const glm::vec3& color) override { m_color = color; }
};

/**
 * @class ComplexObjectWithTransform
 * @brief Klasa reprezentująca złożony obiekt (literę H) jako transformowalny obiekt
 *
 * Dziedziczy po TransformableObject, zarządza złożonym obiektem ComplexObject
 */
class ComplexObjectWithTransform : public TransformableObject {
private:
    std::unique_ptr<ComplexObject> m_complexObject;
    glm::vec3 m_color;

public:
    /**
     * @brief Konstruktor ComplexObjectWithTransform
     * @param width Szerokość litery H (domyślnie 2.0)
     * @param height Wysokość litery H (domyślnie 3.0)
     * @param depth Głębokość litery H (domyślnie 0.5)
     * @param color Kolor litery H (domyślnie czerwony)
     */
    ComplexObjectWithTransform(float width = 2.0f, float height = 3.0f, float depth = 0.5f,
                               const glm::vec3& color = glm::vec3(0.9f, 0.2f, 0.2f));

    /**
     * @brief Destruktor ComplexObjectWithTransform
     *
     * Wymagany dla unique_ptr z niekompletnym typem
     */
    ~ComplexObjectWithTransform();

    /**
     * @brief Tworzy literę H o nowych wymiarach
     * @param width Nowa szerokość
     * @param height Nowa wysokość
     * @param depth Nowa głębokość
     */
    void createLetterH(float width, float height, float depth);

    /**
     * @brief Rysuje złożony obiekt
     * @override
     */
    void draw() const override;

    /**
     * @brief Zwraca kolor obiektu
     * @return Aktualny kolor obiektu
     */
    glm::vec3 getColor() const override { return m_color; }

    /**
     * @brief Ustawia nowy kolor obiektu
     * @param color Nowy kolor
     *
     * @note W tej implementacji kolor jest ustawiany tylko podczas tworzenia,
     * zmiana koloru wymagałaby przerobienia tworzenia obiektu
     */
    void setColor(const glm::vec3& color) override;
};

#endif // TRANSFORMABLE_GEOMETRY_HPP