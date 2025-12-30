// SceneManager.cpp
#include "SceneManager.hpp"
#include <iostream>

SceneManager::SceneManager(GeometryRenderer* renderer) : m_renderer(renderer) {
}

SceneManager::~SceneManager() {
    clear();
}

void SceneManager::setRenderer(GeometryRenderer* renderer) {
    m_renderer = renderer;
    for (auto& obj : m_objects) {
        obj->setRenderer(renderer);
    }
}

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

TransformableObject* SceneManager::getObject(const std::string& name) {
    auto it = m_namedObjects.find(name);
    if (it != m_namedObjects.end()) {
        return it->second;
    }
    return nullptr;
}

TransformableObject* SceneManager::getObject(size_t index) {
    if (index < m_objects.size()) {
        return m_objects[index].get();
    }
    return nullptr;
}

void SceneManager::removeObject(const std::string& name) {
    auto it = m_namedObjects.find(name);
    if (it != m_namedObjects.end()) {
        TransformableObject* obj = it->second;
        m_namedObjects.erase(it);

        // Znajdź i usuń z wektora
        for (auto vecIt = m_objects.begin(); vecIt != m_objects.end(); ++vecIt) {
            if (vecIt->get() == obj) {
                m_objects.erase(vecIt);
                break;
            }
        }
    }
}

void SceneManager::removeObject(size_t index) {
    if (index < m_objects.size()) {
        TransformableObject* obj = m_objects[index].get();

        // Usuń z mapy nazwanych obiektów
        for (auto it = m_namedObjects.begin(); it != m_namedObjects.end(); ++it) {
            if (it->second == obj) {
                m_namedObjects.erase(it);
                break;
            }
        }

        m_objects.erase(m_objects.begin() + index);
    }
}

void SceneManager::clear() {
    m_objects.clear();
    m_namedObjects.clear();
}

void SceneManager::drawAll() {
    for (auto& obj : m_objects) {
        obj->draw();
    }
}

void SceneManager::translateAll(const glm::vec3& translation, Space space) {
    for (auto& obj : m_objects) {
        obj->translate(translation, space);
    }
}

void SceneManager::rotateAll(const glm::quat& rotation, Space space) {
    for (auto& obj : m_objects) {
        obj->rotate(rotation, space);
    }
}

void SceneManager::scaleAll(const glm::vec3& scaleFactor) {
    for (auto& obj : m_objects) {
        obj->scale(scaleFactor);
    }
}

std::string SceneManager::generateUniqueName(const std::string& base) {
    static int counter = 0;
    return base + "_" + std::to_string(counter++);
}