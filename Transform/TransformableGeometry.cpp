// TransformableGeometry.cpp
#include "TransformableGeometry.hpp"
#include "../ComplexObject.hpp"
#include "../GeometryRenderer.hpp"
#include <iostream>

/**
 * @brief Konstruktor CubeObject
 * @param color Kolor sześcianu
 */
CubeObject::CubeObject(const glm::vec3& color) : m_color(color) {
}

/**
 * @brief Rysuje sześcian
 *
 * Jeśli renderer jest dostępny, ustawia kolor i rysuje jednostkowy sześcian
 * z uwzględnieniem skali w macierzy modelu
 */
void CubeObject::draw() const {
    if (!m_renderer) return;

    // Ustaw kolor
    m_renderer->setColor(m_color);

    // Rysuj jednostkowy sześcian (scale jest uwzględniony w macierzy modelu)
    m_renderer->drawCube(glm::vec3(0.0f), glm::vec3(1.0f), getEulerAngles());
}

/**
 * @brief Konstruktor SphereObject
 * @param radius Promień sfery
 * @param color Kolor sfery
 */
SphereObject::SphereObject(float radius, const glm::vec3& color)
    : m_radius(radius), m_color(color) {
}

/**
 * @brief Rysuje sferę
 *
 * Jeśli renderer jest dostępny, ustawia kolor i rysuje sferę
 * z aktualną pozycją i promieniem
 */
void SphereObject::draw() const {
    if (!m_renderer) return;

    // Ustaw kolor
    m_renderer->setColor(m_color);

    // Rysuj sferę
    m_renderer->drawSphere(getPosition(), m_radius);
}

/**
 * @brief Konstruktor CylinderObject
 * @param height Wysokość cylindra
 * @param radius Promień cylindra
 * @param color Kolor cylindra
 */
CylinderObject::CylinderObject(float height, float radius, const glm::vec3& color)
    : m_height(height), m_radius(radius), m_color(color) {
}

/**
 * @brief Rysuje cylinder
 *
 * Jeśli renderer jest dostępny, ustawia kolor i rysuje cylinder
 * z aktualną pozycją, wysokością i promieniem
 */
void CylinderObject::draw() const {
    if (!m_renderer) return;

    // Ustaw kolor
    m_renderer->setColor(m_color);

    // Rysuj cylinder
    m_renderer->drawCylinder(getPosition(), m_height, m_radius);
}

/**
 * @brief Konstruktor ComplexObjectWithTransform
 * @param width Szerokość litery H
 * @param height Wysokość litery H
 * @param depth Głębokość litery H
 * @param color Kolor litery H
 */
ComplexObjectWithTransform::ComplexObjectWithTransform(float width, float height, float depth,
                                                       const glm::vec3& color)
    : m_color(color) {
    m_complexObject = std::make_unique<ComplexObject>();
    m_complexObject->createLetterH(width, height, depth, color);
}

/**
 * @brief Destruktor ComplexObjectWithTransform
 */
ComplexObjectWithTransform::~ComplexObjectWithTransform() {
    // Destruktor jest potrzebny dla unique_ptr z niekompletnym typem
}

/**
 * @brief Tworzy literę H o nowych wymiarach
 * @param width Nowa szerokość
 * @param height Nowa wysokość
 * @param depth Nowa głębokość
 */
void ComplexObjectWithTransform::createLetterH(float width, float height, float depth) {
    if (m_complexObject) {
        m_complexObject->createLetterH(width, height, depth, m_color);
    }
}

/**
 * @brief Rysuje złożony obiekt
 *
 * Jeśli complexObject i renderer są dostępne, ustawia kolor i rysuje obiekt
 */
void ComplexObjectWithTransform::draw() const {
    if (!m_complexObject || !m_renderer) return;

    // Ustaw kolor
    m_renderer->setColor(m_color);

    // Rysuj obiekt złożony
    m_complexObject->draw();
}

/**
 * @brief Ustawia nowy kolor obiektu
 * @param color Nowy kolor
 *
 * @note W tej implementacji kolor jest ustawiany tylko podczas tworzenia,
 * zmiana koloru wymagałaby przerobienia tworzenia obiektu
 */
void ComplexObjectWithTransform::setColor(const glm::vec3& color) {
    m_color = color;
    // Aby zmienić kolor, trzeba by przerobić tworzenie obiektu
    // W tej implementacji kolor jest ustawiany tylko podczas tworzenia
}