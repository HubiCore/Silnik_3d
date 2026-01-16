// Transform.hpp
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

/**
 * @enum Space
 * @brief Coordinate space for transformations.
 *
 * Defines whether transformations are applied in local or world space.
 */
enum class Space {
    LOCAL,  ///< Local coordinate space (relative to object's own orientation)
    WORLD   ///< World coordinate space (global coordinates)
};

/**
 * @class Transform
 * @brief Represents a 3D transformation with position, rotation, and scale.
 *
 * The Transform class handles hierarchical transformations, allowing objects
 * to be parented to each other. It supports both Euler angles and quaternions
 * for rotations, and provides methods for transforming points between spaces.
 */
class Transform {
private:
    glm::vec3 m_position;     ///< Position in 3D space
    glm::quat m_rotation;     ///< Rotation as a quaternion
    glm::vec3 m_scale;        ///< Scale factors for each axis

    glm::mat4 m_localMatrix;  ///< Cached local transformation matrix
    glm::mat4 m_worldMatrix;  ///< Cached world transformation matrix
    bool m_dirty;             ///< Flag indicating matrices need update

    Transform* m_parent;                   ///< Parent transform in hierarchy
    std::vector<Transform*> m_children;    ///< Child transforms

public:
    /**
     * @brief Default constructor.
     *
     * Creates an identity transform at origin with no rotation and unit scale.
     */
    Transform();

    /**
     * @brief Parameterized constructor.
     * @param position Initial position
     * @param rotation Initial rotation as quaternion
     * @param scale Initial scale
     */
    Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

    /**
     * @brief Destructor that cleans up parent-child relationships.
     */
    ~Transform();

    // Basic transformations

    /**
     * @brief Sets the position.
     * @param position New position
     */
    void setPosition(const glm::vec3& position);

    /**
     * @brief Sets the rotation using a quaternion.
     * @param rotation New rotation (will be normalized)
     */
    void setRotation(const glm::quat& rotation);

    /**
     * @brief Sets the rotation using Euler angles.
     * @param eulerAngles Rotation in degrees around X, Y, Z axes
     */
    void setRotation(const glm::vec3& eulerAngles);

    /**
     * @brief Sets the scale.
     * @param scale New scale factors
     */
    void setScale(const glm::vec3& scale);

    /**
     * @brief Translates the transform.
     * @param translation Translation vector
     * @param space Coordinate space for translation (default: LOCAL)
     */
    void translate(const glm::vec3& translation, Space space = Space::LOCAL);

    /**
     * @brief Rotates the transform using a quaternion.
     * @param rotation Rotation quaternion
     * @param space Coordinate space for rotation (default: LOCAL)
     */
    void rotate(const glm::quat& rotation, Space space = Space::LOCAL);

    /**
     * @brief Rotates the transform around an axis.
     * @param axis Rotation axis (will be normalized)
     * @param angleDegrees Rotation angle in degrees
     * @param space Coordinate space for rotation (default: LOCAL)
     */
    void rotate(const glm::vec3& axis, float angleDegrees, Space space = Space::LOCAL);

    /**
     * @brief Scales the transform.
     * @param scaleFactor Scaling factors for each axis
     */
    void scale(const glm::vec3& scaleFactor);

    // Getters

    /**
     * @brief Gets the position.
     * @return Current position
     */
    glm::vec3 getPosition() const { return m_position; }

    /**
     * @brief Gets the rotation as a quaternion.
     * @return Current rotation
     */
    glm::quat getRotation() const { return m_rotation; }

    /**
     * @brief Gets the rotation as Euler angles.
     * @return Rotation in degrees around X, Y, Z axes
     */
    glm::vec3 getEulerAngles() const;

    /**
     * @brief Gets the scale.
     * @return Current scale factors
     */
    glm::vec3 getScale() const { return m_scale; }

    // Transformation matrices

    /**
     * @brief Gets the local transformation matrix.
     * @return 4x4 local transformation matrix (cached, recalculated if dirty)
     */
    glm::mat4 getLocalMatrix();

    /**
     * @brief Gets the world transformation matrix.
     * @return 4x4 world transformation matrix (cached, recalculated if dirty)
     */
    glm::mat4 getWorldMatrix();

    // Local directions

    /**
     * @brief Gets the forward direction vector.
     * @return Normalized forward vector (negative Z in local space)
     */
    glm::vec3 getForward() const;

    /**
     * @brief Gets the right direction vector.
     * @return Normalized right vector (positive X in local space)
     */
    glm::vec3 getRight() const;

    /**
     * @brief Gets the up direction vector.
     * @return Normalized up vector (positive Y in local space)
     */
    glm::vec3 getUp() const;

    // Hierarchy

    /**
     * @brief Sets the parent transform.
     * @param parent New parent transform (nullptr for root)
     */
    void setParent(Transform* parent);

    /**
     * @brief Gets the parent transform.
     * @return Current parent transform (nullptr if root)
     */
    Transform* getParent() const { return m_parent; }

    /**
     * @brief Adds a child transform.
     * @param child Child transform to add
     */
    void addChild(Transform* child);

    /**
     * @brief Removes a child transform.
     * @param child Child transform to remove
     */
    void removeChild(Transform* child);

    /**
     * @brief Gets all child transforms.
     * @return Vector of child transform pointers
     */
    const std::vector<Transform*>& getChildren() const { return m_children; }

    // Point transformation

    /**
     * @brief Transforms a point from local to world space.
     * @param point Point in local space
     * @return Point in world space
     */
    glm::vec3 transformPoint(const glm::vec3& point);

    /**
     * @brief Transforms a point from world to local space.
     * @param point Point in world space
     * @return Point in local space
     */
    glm::vec3 inverseTransformPoint(const glm::vec3& point);

    // Reset

    /**
     * @brief Resets transform to identity.
     *
     * Sets position to (0,0,0), rotation to identity, and scale to (1,1,1).
     */
    void reset();

    // Interpolation

    /**
     * @brief Linearly interpolates between two transforms.
     * @param a First transform
     * @param b Second transform
     * @param t Interpolation factor [0, 1]
     * @return Interpolated transform
     */
    static Transform lerp(const Transform& a, const Transform& b, float t);

    /**
     * @brief Spherically interpolates between two transforms.
     * @param a First transform
     * @param b Second transform
     * @param t Interpolation factor [0, 1]
     * @return Interpolated transform
     *
     * @note Currently uses linear interpolation for all components.
     */
    static Transform slerp(const Transform& a, const Transform& b, float t);

private:
    /**
     * @brief Updates cached transformation matrices.
     *
     * Recalculates local and world matrices if dirty, and propagates
     * dirty flag to children.
     */
    void updateMatrices();

    /**
     * @brief Marks transform as needing matrix update.
     *
     * Sets dirty flag and propagates to children if necessary.
     */
    void markDirty();
};

#endif // TRANSFORM_HPP