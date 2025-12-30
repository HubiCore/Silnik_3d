// TransformableObject.hpp
#ifndef TRANSFORMABLE_OBJECT_HPP
#define TRANSFORMABLE_OBJECT_HPP

#include "Transform.hpp"
#include "../GeometryRenderer.hpp"
#include <memory>

class TransformableObject {
protected:
    std::unique_ptr<Transform> m_transform;
    GeometryRenderer* m_renderer;

public:
    TransformableObject();
    virtual ~TransformableObject();

    // Transformacje
    virtual void setPosition(const glm::vec3& position);
    virtual void setRotation(const glm::quat& rotation);
    virtual void setRotation(const glm::vec3& eulerAngles);
    virtual void setScale(const glm::vec3& scale);

    virtual void translate(const glm::vec3& translation, Space space = Space::LOCAL);
    virtual void rotate(const glm::quat& rotation, Space space = Space::LOCAL);
    virtual void rotate(const glm::vec3& axis, float angleDegrees, Space space = Space::LOCAL);
    virtual void scale(const glm::vec3& scaleFactor);

    glm::vec3 getPosition() const;
    glm::quat getRotation() const;
    glm::vec3 getEulerAngles() const;
    glm::vec3 getScale() const;

    glm::mat4 getModelMatrix() const;

    // Hierarchia
    void setParent(TransformableObject* parent);
    TransformableObject* getParent() const;
    void addChild(TransformableObject* child);

    // Renderowanie
    virtual void draw() const = 0;
    void setRenderer(GeometryRenderer* renderer);

    // Właściwości
    virtual glm::vec3 getColor() const = 0;
    virtual void setColor(const glm::vec3& color) = 0;

protected:
    GeometryRenderer* getRenderer() const;
};

#endif // TRANSFORMABLE_OBJECT_HPP