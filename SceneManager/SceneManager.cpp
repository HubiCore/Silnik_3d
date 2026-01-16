// SceneManager.cpp
#include "SceneManager.hpp"
#include <iostream>

/**
 * @brief Constructs SceneManager with specified renderer.
 * @param renderer Pointer to GeometryRenderer instance
 */
SceneManager::SceneManager(GeometryRenderer* renderer) : m_renderer(renderer) {
}

/**
 * @brief Destructor that clears all objects.
 */
SceneManager::~SceneManager() {
    clear();
}

/**
 * @brief Sets renderer for all objects.
 * @param renderer Pointer to GeometryRenderer instance
 */
void SceneManager::setRenderer(GeometryRenderer* renderer) {
    m_renderer = renderer;
    for (auto& obj : m_objects) {
        obj->setRenderer(renderer);
    }
}

/**
 * @brief Creates a cube object with specified parameters.
 */
TransformableObject* SceneManager::createCube(const std::string& name,
                                              const glm::vec3& position,
                                              const glm::vec3& rotation,
                                              const glm::vec3& scale,
                                              const glm::vec3& color) {
    auto cube = std::make_unique<CubeObject>(color);
    cube->setPosition(position);
    cube->setRotation(rotation);
    cube->setScale(scale);
    cube->setRenderer(m_renderer);

    std::string objName = name.empty() ? generateUniqueName("Cube") : name;

    TransformableObject* result = cube.get();
    m_objects.push_back(std::move(cube));
    m_namedObjects[objName] = result;

    return result;
}

/**
 * @brief Creates a sphere object with specified parameters.
 */
TransformableObject* SceneManager::createSphere(const std::string& name,
                                                const glm::vec3& position,
                                                float radius,
                                                const glm::vec3& color) {
    auto sphere = std::make_unique<SphereObject>(radius, color);
    sphere->setPosition(position);
    sphere->setRenderer(m_renderer);

    std::string objName = name.empty() ? generateUniqueName("Sphere") : name;

    TransformableObject* result = sphere.get();
    m_objects.push_back(std::move(sphere));
    m_namedObjects[objName] = result;

    return result;
}

/**
 * @brief Creates a cylinder object with specified parameters.
 */
TransformableObject* SceneManager::createCylinder(const std::string& name,
                                                  const glm::vec3& position,
                                                  float height,
                                                  float radius,
                                                  const glm::vec3& color) {
    auto cylinder = std::make_unique<CylinderObject>(height, radius, color);
    cylinder->setPosition(position);
    cylinder->setRenderer(m_renderer);

    std::string objName = name.empty() ? generateUniqueName("Cylinder") : name;

    TransformableObject* result = cylinder.get();
    m_objects.push_back(std::move(cylinder));
    m_namedObjects[objName] = result;

    return result;
}

/**
 * @brief Creates a letter 'H' complex object with specified parameters.
 */
ComplexObjectWithTransform* SceneManager::createLetterH(const std::string& name,
                                                        const glm::vec3& position,
                                                        float width,
                                                        float height,
                                                        float depth,
                                                        const glm::vec3& color) {
    auto letterH = std::make_unique<ComplexObjectWithTransform>(width, height, depth, color);
    letterH->setPosition(position);
    letterH->setRenderer(m_renderer);

    std::string objName = name.empty() ? generateUniqueName("LetterH") : name;

    ComplexObjectWithTransform* result = letterH.get();
    m_objects.push_back(std::move(letterH));
    m_namedObjects[objName] = result;

    return result;
}

/**
 * @brief Retrieves object by name.
 */
TransformableObject* SceneManager::getObject(const std::string& name) {
    auto it = m_namedObjects.find(name);
    if (it != m_namedObjects.end()) {
        return it->second;
    }
    return nullptr;
}

/**
 * @brief Retrieves object by index.
 */
TransformableObject* SceneManager::getObject(size_t index) {
    if (index < m_objects.size()) {
        return m_objects[index].get();
    }
    return nullptr;
}

/**
 * @brief Removes object by name.
 */
void SceneManager::removeObject(const std::string& name) {
    auto it = m_namedObjects.find(name);
    if (it != m_namedObjects.end()) {
        TransformableObject* obj = it->second;
        m_namedObjects.erase(it);

        // Find and remove from vector
        for (auto vecIt = m_objects.begin(); vecIt != m_objects.end(); ++vecIt) {
            if (vecIt->get() == obj) {
                m_objects.erase(vecIt);
                break;
            }
        }
    }
}

/**
 * @brief Removes object by index.
 */
void SceneManager::removeObject(size_t index) {
    if (index < m_objects.size()) {
        TransformableObject* obj = m_objects[index].get();

        // Remove from named objects map
        for (auto it = m_namedObjects.begin(); it != m_namedObjects.end(); ++it) {
            if (it->second == obj) {
                m_namedObjects.erase(it);
                break;
            }
        }

        m_objects.erase(m_objects.begin() + index);
    }
}

/**
 * @brief Clears all objects from scene.
 */
void SceneManager::clear() {
    m_objects.clear();
    m_namedObjects.clear();
}

/**
 * @brief Draws all objects in scene.
 */
void SceneManager::drawAll() {
    for (auto& obj : m_objects) {
        obj->draw();
    }
}

/**
 * @brief Translates all objects.
 */
void SceneManager::translateAll(const glm::vec3& translation, Space space) {
    for (auto& obj : m_objects) {
        obj->translate(translation, space);
    }
}

/**
 * @brief Rotates all objects.
 */
void SceneManager::rotateAll(const glm::quat& rotation, Space space) {
    for (auto& obj : m_objects) {
        obj->rotate(rotation, space);
    }
}

/**
 * @brief Scales all objects.
 */
void SceneManager::scaleAll(const glm::vec3& scaleFactor) {
    for (auto& obj : m_objects) {
        obj->scale(scaleFactor);
    }
}

/**
 * @brief Generates unique name for object.
 */
std::string SceneManager::generateUniqueName(const std::string& base) {
    static int counter = 0;
    return base + "_" + std::to_string(counter++);
}