// TransformableGeometry.hpp
#ifndef TRANSFORMABLE_GEOMETRY_HPP
#define TRANSFORMABLE_GEOMETRY_HPP

#include "TransformableObject.hpp"
#include <glm/glm.hpp>
#include <memory>

// Forward declaration
class ComplexObject;

class CubeObject : public TransformableObject {
private:
    glm::vec3 m_color;

public:
    CubeObject(const glm::vec3& color = glm::vec3(1.0f, 0.5f, 0.2f));

    void draw() const override;
    glm::vec3 getColor() const override { return m_color; }
    void setColor(const glm::vec3& color) override { m_color = color; }
};

class SphereObject : public TransformableObject {
private:
    float m_radius;
    glm::vec3 m_color;

public:
    SphereObject(float radius = 1.0f, const glm::vec3& color = glm::vec3(0.2f, 0.8f, 0.2f));

    void draw() const override;
    float getRadius() const { return m_radius; }
    void setRadius(float radius) { m_radius = radius; }

    glm::vec3 getColor() const override { return m_color; }
    void setColor(const glm::vec3& color) override { m_color = color; }
};

class CylinderObject : public TransformableObject {
private:
    float m_height;
    float m_radius;
    glm::vec3 m_color;

public:
    CylinderObject(float height = 2.0f, float radius = 0.5f,
                   const glm::vec3& color = glm::vec3(0.2f, 0.5f, 1.0f));

    void draw() const override;
    float getHeight() const { return m_height; }
    float getRadius() const { return m_radius; }
    void setHeight(float height) { m_height = height; }
    void setRadius(float radius) { m_radius = radius; }

    glm::vec3 getColor() const override { return m_color; }
    void setColor(const glm::vec3& color) override { m_color = color; }
};

class ComplexObjectWithTransform : public TransformableObject {
private:
    std::unique_ptr<ComplexObject> m_complexObject;
    glm::vec3 m_color;

public:
    ComplexObjectWithTransform(float width = 2.0f, float height = 3.0f, float depth = 0.5f,
                               const glm::vec3& color = glm::vec3(0.9f, 0.2f, 0.2f));
    ~ComplexObjectWithTransform();

    void createLetterH(float width, float height, float depth);
    void draw() const override;

    glm::vec3 getColor() const override { return m_color; }
    void setColor(const glm::vec3& color) override;
};

#endif // TRANSFORMABLE_GEOMETRY_HPP