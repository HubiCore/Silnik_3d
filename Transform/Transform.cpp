// Transform.cpp
#include "Transform.hpp"
#include <iostream>
#include <algorithm>

Transform::Transform()
    : m_position(0.0f), m_rotation(glm::identity<glm::quat>()),
      m_scale(1.0f), m_parent(nullptr), m_dirty(true) {
    m_localMatrix = glm::mat4(1.0f);
    m_worldMatrix = glm::mat4(1.0f);
}

Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
    : m_position(position), m_rotation(rotation),
      m_scale(scale), m_parent(nullptr), m_dirty(true) {
    updateMatrices();
}

Transform::~Transform() {
    for (auto& child : m_children) {
        child->m_parent = nullptr;
    }
    m_children.clear();
}

void Transform::setPosition(const glm::vec3& position) {
    m_position = position;
    markDirty();
}

void Transform::setRotation(const glm::quat& rotation) {
    m_rotation = glm::normalize(rotation);
    markDirty();
}

void Transform::setRotation(const glm::vec3& eulerAngles) {
    glm::vec3 radians = glm::radians(eulerAngles);
    m_rotation = glm::quat(radians);
    markDirty();
}

void Transform::setScale(const glm::vec3& scale) {
    m_scale = scale;
    markDirty();
}

void Transform::translate(const glm::vec3& translation, Space space) {
    if (space == Space::LOCAL) {
        m_position += m_rotation * translation;
    } else {
        m_position += translation;
    }
    markDirty();
}

void Transform::rotate(const glm::quat& rotation, Space space) {
    if (space == Space::LOCAL) {
        m_rotation = rotation * m_rotation;
    } else {
        m_rotation = m_rotation * rotation;
    }
    m_rotation = glm::normalize(m_rotation);
    markDirty();
}

void Transform::rotate(const glm::vec3& axis, float angleDegrees, Space space) {
    float radians = glm::radians(angleDegrees);
    glm::quat rotation = glm::angleAxis(radians, glm::normalize(axis));
    rotate(rotation, space);
}

void Transform::scale(const glm::vec3& scaleFactor) {
    m_scale *= scaleFactor;
    markDirty();
}

glm::vec3 Transform::getEulerAngles() const {
    return glm::degrees(glm::eulerAngles(m_rotation));
}

glm::mat4 Transform::getLocalMatrix() {
    if (m_dirty) {
        updateMatrices();
    }
    return m_localMatrix;
}

glm::mat4 Transform::getWorldMatrix() {
    if (m_dirty) {
        updateMatrices();
    }
    return m_worldMatrix;
}

glm::vec3 Transform::getForward() const {
    return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Transform::getRight() const {
    return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 Transform::getUp() const {
    return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

void Transform::setParent(Transform* parent) {
    if (m_parent == parent) return;

    if (m_parent) {
        auto it = std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this);
        if (it != m_parent->m_children.end()) {
            m_parent->m_children.erase(it);
        }
    }

    m_parent = parent;
    if (parent) {
        parent->m_children.push_back(this);
    }

    markDirty();
}

void Transform::addChild(Transform* child) {
    if (!child || child == this) return;
    child->setParent(this);
}

void Transform::removeChild(Transform* child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        m_children.erase(it);
        child->m_parent = nullptr;
        child->markDirty();
    }
}

glm::vec3 Transform::transformPoint(const glm::vec3& point) {
    glm::mat4 worldMatrix = getWorldMatrix();
    glm::vec4 transformed = worldMatrix * glm::vec4(point, 1.0f);
    return glm::vec3(transformed);
}

glm::vec3 Transform::inverseTransformPoint(const glm::vec3& point) {
    glm::mat4 worldMatrix = getWorldMatrix();
    glm::mat4 inverseMatrix = glm::inverse(worldMatrix);
    glm::vec4 transformed = inverseMatrix * glm::vec4(point, 1.0f);
    return glm::vec3(transformed);
}

void Transform::reset() {
    m_position = glm::vec3(0.0f);
    m_rotation = glm::identity<glm::quat>();
    m_scale = glm::vec3(1.0f);
    markDirty();
}

Transform Transform::lerp(const Transform& a, const Transform& b, float t) {
    Transform result;
    result.m_position = glm::mix(a.m_position, b.m_position, t);
    result.m_rotation = glm::slerp(a.m_rotation, b.m_rotation, t);
    result.m_scale = glm::mix(a.m_scale, b.m_scale, t);
    result.m_dirty = true;
    return result;
}

Transform Transform::slerp(const Transform& a, const Transform& b, float t) {
    return lerp(a, b, t); // Używamy lerp dla całej transformacji
}

void Transform::updateMatrices() {
    // Oblicz macierz lokalną
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_position);
    glm::mat4 rotation = glm::mat4_cast(m_rotation);
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), m_scale);

    m_localMatrix = translation * rotation * scaling;

    // Oblicz macierz światową
    if (m_parent) {
        m_worldMatrix = m_parent->getWorldMatrix() * m_localMatrix;
    } else {
        m_worldMatrix = m_localMatrix;
    }

    m_dirty = false;

    // Oznacz dzieci jako wymagające aktualizacji
    for (auto child : m_children) {
        child->markDirty();
    }
}

void Transform::markDirty() {
    m_dirty = true;
}