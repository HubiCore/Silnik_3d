// Transform.hpp
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

enum class Space {
    LOCAL,
    WORLD
};

class Transform {
private:
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;

    glm::mat4 m_localMatrix;
    glm::mat4 m_worldMatrix;
    bool m_dirty;

    Transform* m_parent;
    std::vector<Transform*> m_children;

public:
    Transform();
    Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
    ~Transform();

    // Podstawowe transformacje
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::quat& rotation);
    void setRotation(const glm::vec3& eulerAngles); // w stopniach
    void setScale(const glm::vec3& scale);

    void translate(const glm::vec3& translation, Space space = Space::LOCAL);
    void rotate(const glm::quat& rotation, Space space = Space::LOCAL);
    void rotate(const glm::vec3& axis, float angleDegrees, Space space = Space::LOCAL);
    void scale(const glm::vec3& scaleFactor);

    // Pobieranie transformacji
    glm::vec3 getPosition() const { return m_position; }
    glm::quat getRotation() const { return m_rotation; }
    glm::vec3 getEulerAngles() const;
    glm::vec3 getScale() const { return m_scale; }

    // Macierze transformacji
    glm::mat4 getLocalMatrix();
    glm::mat4 getWorldMatrix();

    // Kierunki lokalne
    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
    glm::vec3 getUp() const;

    // Hierarchia
    void setParent(Transform* parent);
    Transform* getParent() const { return m_parent; }
    void addChild(Transform* child);
    void removeChild(Transform* child);
    const std::vector<Transform*>& getChildren() const { return m_children; }

    // Transformacja punktów
    glm::vec3 transformPoint(const glm::vec3& point);
    glm::vec3 inverseTransformPoint(const glm::vec3& point);

    // Reset
    void reset();

    // Interpolacja
    static Transform lerp(const Transform& a, const Transform& b, float t);
    static Transform slerp(const Transform& a, const Transform& b, float t);

private:
    void updateMatrices();
    void markDirty();
};

#endif // TRANSFORM_HPP