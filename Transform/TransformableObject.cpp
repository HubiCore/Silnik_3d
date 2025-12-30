// TransformableObject.cpp
#include "TransformableObject.hpp"

TransformableObject::TransformableObject()
    : m_transform(std::make_unique<Transform>()), m_renderer(nullptr) {
}

TransformableObject::~TransformableObject() {
}

void TransformableObject::setPosition(const glm::vec3& position) {
    m_transform->setPosition(position);
}

void TransformableObject::setRotation(const glm::quat& rotation) {
    m_transform->setRotation(rotation);
}

void TransformableObject::setRotation(const glm::vec3& eulerAngles) {
    m_transform->setRotation(eulerAngles);
}

void TransformableObject::setScale(const glm::vec3& scale) {
    m_transform->setScale(scale);
}

void TransformableObject::translate(const glm::vec3& translation, Space space) {
    m_transform->translate(translation, space);
}

void TransformableObject::rotate(const glm::quat& rotation, Space space) {
    m_transform->rotate(rotation, space);
}

void TransformableObject::rotate(const glm::vec3& axis, float angleDegrees, Space space) {
    m_transform->rotate(axis, angleDegrees, space);
}

void TransformableObject::scale(const glm::vec3& scaleFactor) {
    m_transform->scale(scaleFactor);
}

glm::vec3 TransformableObject::getPosition() const {
    return m_transform->getPosition();
}

glm::quat TransformableObject::getRotation() const {
    return m_transform->getRotation();
}

glm::vec3 TransformableObject::getEulerAngles() const {
    return m_transform->getEulerAngles();
}

glm::vec3 TransformableObject::getScale() const {
    return m_transform->getScale();
}

glm::mat4 TransformableObject::getModelMatrix() const {
    return m_transform->getWorldMatrix();
}

void TransformableObject::setParent(TransformableObject* parent) {
    if (parent) {
        m_transform->setParent(parent->m_transform.get());
    } else {
        m_transform->setParent(nullptr);
    }
}

TransformableObject* TransformableObject::getParent() const {
    Transform* parentTransform = m_transform->getParent();
    // Uwaga: to wymaga dodatkowej mapy Transform -> TransformableObject
    // Dla uproszczenia zwracamy nullptr
    return nullptr;
}

void TransformableObject::addChild(TransformableObject* child) {
    if (child) {
        child->setParent(this);
    }
}

void TransformableObject::setRenderer(GeometryRenderer* renderer) {
    m_renderer = renderer;
}

GeometryRenderer* TransformableObject::getRenderer() const {
    return m_renderer;
}