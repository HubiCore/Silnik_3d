// TransformableGeometry.cpp
#include "TransformableGeometry.hpp"
#include "../ComplexObject.hpp"
#include "../GeometryRenderer.hpp"
#include <iostream>

CubeObject::CubeObject(const glm::vec3& color) : m_color(color) {
}

void CubeObject::draw() const {
    if (!m_renderer) return;

    // Ustaw kolor
    m_renderer->setColor(m_color);

    // Rysuj jednostkowy sześcian (scale jest uwzględniony w macierzy modelu)
    m_renderer->drawCube(glm::vec3(0.0f), glm::vec3(1.0f), getEulerAngles());
}

SphereObject::SphereObject(float radius, const glm::vec3& color)
    : m_radius(radius), m_color(color) {
}

void SphereObject::draw() const {
    if (!m_renderer) return;

    // Ustaw kolor
    m_renderer->setColor(m_color);

    // Rysuj sferę
    m_renderer->drawSphere(getPosition(), m_radius);
}

CylinderObject::CylinderObject(float height, float radius, const glm::vec3& color)
    : m_height(height), m_radius(radius), m_color(color) {
}

void CylinderObject::draw() const {
    if (!m_renderer) return;

    // Ustaw kolor
    m_renderer->setColor(m_color);

    // Rysuj cylinder
    m_renderer->drawCylinder(getPosition(), m_height, m_radius);
}

ComplexObjectWithTransform::ComplexObjectWithTransform(float width, float height, float depth,
                                                       const glm::vec3& color)
    : m_color(color) {
    m_complexObject = std::make_unique<ComplexObject>();
    m_complexObject->createLetterH(width, height, depth, color);
}

ComplexObjectWithTransform::~ComplexObjectWithTransform() {
    // Destruktor jest potrzebny dla unique_ptr z niekompletnym typem
}

void ComplexObjectWithTransform::createLetterH(float width, float height, float depth) {
    if (m_complexObject) {
        m_complexObject->createLetterH(width, height, depth, m_color);
    }
}

void ComplexObjectWithTransform::draw() const {
    if (!m_complexObject || !m_renderer) return;

    // Ustaw kolor
    m_renderer->setColor(m_color);

    // Rysuj obiekt złożony
    m_complexObject->draw();
}

void ComplexObjectWithTransform::setColor(const glm::vec3& color) {
    m_color = color;
    // Aby zmienić kolor, trzeba by przerobić tworzenie obiektu
    // W tej implementacji kolor jest ustawiany tylko podczas tworzenia
}