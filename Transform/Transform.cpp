// Transform.cpp
#include "Transform.hpp"
#include <iostream>
#include <algorithm>

/**
 * @brief Default constructor.
 */
Transform::Transform()
    : m_position(0.0f), m_rotation(glm::identity<glm::quat>()),
      m_scale(1.0f), m_parent(nullptr), m_dirty(true) {
    m_localMatrix = glm::mat4(1.0f);
    m_worldMatrix = glm::mat4(1.0f);
}

/**
 * @brief Parameterized constructor.
 */
Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
    : m_position(position), m_rotation(rotation),
      m_scale(scale), m_parent(nullptr), m_dirty(true) {
    updateMatrices();
}

/**
 * @brief Destructor.
 */
Transform::~Transform() {
    // Clear parent references in children
    for (auto& child : m_children) {
        child->m_parent = nullptr;
    }
    m_children.clear();
}

/**
 * @brief Sets position.
 */
void Transform::setPosition(const glm::vec3& position) {
    m_position = position;
    markDirty();
}

/**
 * @brief Sets rotation using quaternion.
 */
void Transform::setRotation(const glm::quat& rotation) {
    m_rotation = glm::normalize(rotation);
    markDirty();
}

/**
 * @brief Sets rotation using Euler angles.
 */
void Transform::setRotation(const glm::vec3& eulerAngles) {
    glm::vec3 radians = glm::radians(eulerAngles);
    m_rotation = glm::quat(radians);
    markDirty();
}

/**
 * @brief Sets scale.
 */
void Transform::setScale(const glm::vec3& scale) {
    m_scale = scale;
    markDirty();
}

/**
 * @brief Translates the transform.
 */
void Transform::translate(const glm::vec3& translation, Space space) {
    if (space == Space::LOCAL) {
        // Apply translation in local space
        m_position += m_rotation * translation;
    } else {
        // Apply translation in world space
        m_position += translation;
    }
    markDirty();
}

/**
 * @brief Rotates using quaternion.
 */
void Transform::rotate(const glm::quat& rotation, Space space) {
    if (space == Space::LOCAL) {
        // Apply rotation in local space (pre-multiply)
        m_rotation = rotation * m_rotation;
    } else {
        // Apply rotation in world space (post-multiply)
        m_rotation = m_rotation * rotation;
    }
    m_rotation = glm::normalize(m_rotation);
    markDirty();
}

/**
 * @brief Rotates around an axis.
 */
void Transform::rotate(const glm::vec3& axis, float angleDegrees, Space space) {
    float radians = glm::radians(angleDegrees);
    glm::quat rotation = glm::angleAxis(radians, glm::normalize(axis));
    rotate(rotation, space);
}

/**
 * @brief Scales the transform.
 */
void Transform::scale(const glm::vec3& scaleFactor) {
    m_scale *= scaleFactor;
    markDirty();
}

/**
 * @brief Gets rotation as Euler angles.
 */
glm::vec3 Transform::getEulerAngles() const {
    return glm::degrees(glm::eulerAngles(m_rotation));
}

/**
 * @brief Gets local transformation matrix.
 */
glm::mat4 Transform::getLocalMatrix() {
    if (m_dirty) {
        updateMatrices();
    }
    return m_localMatrix;
}

/**
 * @brief Gets world transformation matrix.
 */
glm::mat4 Transform::getWorldMatrix() {
    if (m_dirty) {
        updateMatrices();
    }
    return m_worldMatrix;
}

/**
 * @brief Gets forward direction vector.
 */
glm::vec3 Transform::getForward() const {
    return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f);
}

/**
 * @brief Gets right direction vector.
 */
glm::vec3 Transform::getRight() const {
    return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

/**
 * @brief Gets up direction vector.
 */
glm::vec3 Transform::getUp() const {
    return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

/**
 * @brief Sets parent transform.
 */
void Transform::setParent(Transform* parent) {
    if (m_parent == parent) return;

    // Remove from current parent
    if (m_parent) {
        auto it = std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this);
        if (it != m_parent->m_children.end()) {
            m_parent->m_children.erase(it);
        }
    }

    // Set new parent
    m_parent = parent;
    if (parent) {
        parent->m_children.push_back(this);
    }

    markDirty();
}

/**
 * @brief Adds child transform.
 */
void Transform::addChild(Transform* child) {
    if (!child || child == this) return;
    child->setParent(this);
}

/**
 * @brief Removes child transform.
 */
void Transform::removeChild(Transform* child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        m_children.erase(it);
        child->m_parent = nullptr;
        child->markDirty();
    }
}

/**
 * @brief Transforms point from local to world space.
 */
glm::vec3 Transform::transformPoint(const glm::vec3& point) {
    glm::mat4 worldMatrix = getWorldMatrix();
    glm::vec4 transformed = worldMatrix * glm::vec4(point, 1.0f);
    return glm::vec3(transformed);
}

/**
 * @brief Transforms point from world to local space.
 */
glm::vec3 Transform::inverseTransformPoint(const glm::vec3& point) {
    glm::mat4 worldMatrix = getWorldMatrix();
    glm::mat4 inverseMatrix = glm::inverse(worldMatrix);
    glm::vec4 transformed = inverseMatrix * glm::vec4(point, 1.0f);
    return glm::vec3(transformed);
}

/**
 * @brief Resets transform to identity.
 */
void Transform::reset() {
    m_position = glm::vec3(0.0f);
    m_rotation = glm::identity<glm::quat>();
    m_scale = glm::vec3(1.0f);
    markDirty();
}

/**
 * @brief Linearly interpolates between two transforms.
 */
Transform Transform::lerp(const Transform& a, const Transform& b, float t) {
    Transform result;
    result.m_position = glm::mix(a.m_position, b.m_position, t);
    result.m_rotation = glm::slerp(a.m_rotation, b.m_rotation, t);
    result.m_scale = glm::mix(a.m_scale, b.m_scale, t);
    result.m_dirty = true;
    return result;
}

/**
 * @brief Spherically interpolates between two transforms.
 */
Transform Transform::slerp(const Transform& a, const Transform& b, float t) {
    return lerp(a, b, t); // Uses lerp for all components
}

/**
 * @brief Updates cached transformation matrices.
 */
void Transform::updateMatrices() {
    // Calculate local matrix: T * R * S
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_position);
    glm::mat4 rotation = glm::mat4_cast(m_rotation);
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), m_scale);

    m_localMatrix = translation * rotation * scaling;

    // Calculate world matrix
    if (m_parent) {
        m_worldMatrix = m_parent->getWorldMatrix() * m_localMatrix;
    } else {
        m_worldMatrix = m_localMatrix;
    }

    m_dirty = false;

    // Mark children as needing update
    for (auto child : m_children) {
        child->markDirty();
    }
}

/**
 * @brief Marks transform as needing matrix update.
 */
void Transform::markDirty() {
    m_dirty = true;
}