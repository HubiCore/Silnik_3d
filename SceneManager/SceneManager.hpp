// SceneManager.hpp
#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "../Transform/TransformableObject.hpp"
#include "../Transform/TransformableGeometry.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

/**
 * @class SceneManager
 * @brief Manages scene objects, their creation, retrieval, removal, and group transformations.
 *
 * The SceneManager maintains a collection of 3D objects and provides methods
 * to create, access, and manipulate them. Objects can be referenced by name
 * or by index.
 */
class SceneManager {
private:
    std::vector<std::unique_ptr<TransformableObject>> m_objects; ///< Container for all scene objects
    std::unordered_map<std::string, TransformableObject*> m_namedObjects; ///< Map of named objects for fast lookup
    GeometryRenderer* m_renderer; ///< Pointer to the renderer used for drawing objects

public:
    /**
     * @brief Constructs a SceneManager with an optional renderer.
     * @param renderer Pointer to a GeometryRenderer instance (default: nullptr)
     */
    SceneManager(GeometryRenderer* renderer = nullptr);

    /**
     * @brief Destructor that clears all managed objects.
     */
    ~SceneManager();

    /**
     * @brief Sets the renderer for all existing and future objects.
     * @param renderer Pointer to a GeometryRenderer instance
     */
    void setRenderer(GeometryRenderer* renderer);

    // Object creation methods

    /**
     * @brief Creates a cube object.
     * @param name Optional name for the object (default: auto-generated)
     * @param position Initial position (default: (0,0,0))
     * @param rotation Initial rotation in Euler angles (default: (0,0,0))
     * @param scale Initial scale (default: (1,1,1))
     * @param color RGB color (default: orange)
     * @return Pointer to the created TransformableObject
     */
    TransformableObject* createCube(const std::string& name = "",
                                    const glm::vec3& position = glm::vec3(0.0f),
                                    const glm::vec3& rotation = glm::vec3(0.0f),
                                    const glm::vec3& scale = glm::vec3(1.0f),
                                    const glm::vec3& color = glm::vec3(1.0f, 0.5f, 0.2f));

    /**
     * @brief Creates a sphere object.
     * @param name Optional name for the object (default: auto-generated)
     * @param position Initial position (default: (0,0,0))
     * @param radius Sphere radius (default: 1.0)
     * @param color RGB color (default: green)
     * @return Pointer to the created TransformableObject
     */
    TransformableObject* createSphere(const std::string& name = "",
                                      const glm::vec3& position = glm::vec3(0.0f),
                                      float radius = 1.0f,
                                      const glm::vec3& color = glm::vec3(0.2f, 0.8f, 0.2f));

    /**
     * @brief Creates a cylinder object.
     * @param name Optional name for the object (default: auto-generated)
     * @param position Initial position (default: (0,0,0))
     * @param height Cylinder height (default: 2.0)
     * @param radius Cylinder radius (default: 0.5)
     * @param color RGB color (default: blue)
     * @return Pointer to the created TransformableObject
     */
    TransformableObject* createCylinder(const std::string& name = "",
                                        const glm::vec3& position = glm::vec3(0.0f),
                                        float height = 2.0f,
                                        float radius = 0.5f,
                                        const glm::vec3& color = glm::vec3(0.2f, 0.5f, 1.0f));

    /**
     * @brief Creates a letter 'H' complex object.
     * @param name Optional name for the object (default: auto-generated)
     * @param position Initial position (default: (0,0,0))
     * @param width Letter width (default: 2.0)
     * @param height Letter height (default: 3.0)
     * @param depth Letter depth (default: 0.5)
     * @param color RGB color (default: red)
     * @return Pointer to the created ComplexObjectWithTransform
     */
    ComplexObjectWithTransform* createLetterH(const std::string& name = "",
                                             const glm::vec3& position = glm::vec3(0.0f),
                                             float width = 2.0f,
                                             float height = 3.0f,
                                             float depth = 0.5f,
                                             const glm::vec3& color = glm::vec3(0.9f, 0.2f, 0.2f));

    // Object retrieval methods

    /**
     * @brief Retrieves an object by name.
     * @param name Name of the object to find
     * @return Pointer to the object, or nullptr if not found
     */
    TransformableObject* getObject(const std::string& name);

    /**
     * @brief Retrieves an object by index.
     * @param index Zero-based index of the object
     * @return Pointer to the object, or nullptr if index is invalid
     */
    TransformableObject* getObject(size_t index);

    // Object removal methods

    /**
     * @brief Removes an object by name.
     * @param name Name of the object to remove
     */
    void removeObject(const std::string& name);

    /**
     * @brief Removes an object by index.
     * @param index Zero-based index of the object to remove
     */
    void removeObject(size_t index);

    /**
     * @brief Removes all objects from the scene.
     */
    void clear();

    // Rendering

    /**
     * @brief Draws all objects in the scene.
     */
    void drawAll();

    // Group transformations

    /**
     * @brief Translates all objects by the specified vector.
     * @param translation Translation vector
     * @param space Coordinate space for the transformation (default: LOCAL)
     */
    void translateAll(const glm::vec3& translation, Space space = Space::LOCAL);

    /**
     * @brief Rotates all objects by the specified quaternion.
     * @param rotation Rotation quaternion
     * @param space Coordinate space for the transformation (default: LOCAL)
     */
    void rotateAll(const glm::quat& rotation, Space space = Space::LOCAL);

    /**
     * @brief Scales all objects by the specified factor.
     * @param scaleFactor Scaling factors for each axis
     */
    void scaleAll(const glm::vec3& scaleFactor);

    // Statistics

    /**
     * @brief Gets the number of objects in the scene.
     * @return Number of managed objects
     */
    size_t getObjectCount() const { return m_objects.size(); }

private:
    /**
     * @brief Generates a unique name for an object.
     * @param base Base name to append counter to
     * @return Unique name string
     */
    std::string generateUniqueName(const std::string& base);
};

#endif // SCENE_MANAGER_HPP