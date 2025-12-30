// SceneManager.hpp
#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "../Transform/TransformableObject.hpp"
#include "../Transform/TransformableGeometry.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

class SceneManager {
private:
    std::vector<std::unique_ptr<TransformableObject>> m_objects;
    std::unordered_map<std::string, TransformableObject*> m_namedObjects;
    GeometryRenderer* m_renderer;

public:
    SceneManager(GeometryRenderer* renderer = nullptr);
    ~SceneManager();

    void setRenderer(GeometryRenderer* renderer);

    // Dodawanie obiektów
    TransformableObject* createCube(const std::string& name = "",
                                    const glm::vec3& position = glm::vec3(0.0f),
                                    const glm::vec3& rotation = glm::vec3(0.0f),
                                    const glm::vec3& scale = glm::vec3(1.0f),
                                    const glm::vec3& color = glm::vec3(1.0f, 0.5f, 0.2f));

    TransformableObject* createSphere(const std::string& name = "",
                                      const glm::vec3& position = glm::vec3(0.0f),
                                      float radius = 1.0f,
                                      const glm::vec3& color = glm::vec3(0.2f, 0.8f, 0.2f));

    TransformableObject* createCylinder(const std::string& name = "",
                                        const glm::vec3& position = glm::vec3(0.0f),
                                        float height = 2.0f,
                                        float radius = 0.5f,
                                        const glm::vec3& color = glm::vec3(0.2f, 0.5f, 1.0f));

    ComplexObjectWithTransform* createLetterH(const std::string& name = "",
                                             const glm::vec3& position = glm::vec3(0.0f),
                                             float width = 2.0f,
                                             float height = 3.0f,
                                             float depth = 0.5f,
                                             const glm::vec3& color = glm::vec3(0.9f, 0.2f, 0.2f));

    // Pobieranie obiektów
    TransformableObject* getObject(const std::string& name);
    TransformableObject* getObject(size_t index);

    // Usuwanie obiektów
    void removeObject(const std::string& name);
    void removeObject(size_t index);
    void clear();

    // Renderowanie
    void drawAll();

    // Transformacje grupowe
    void translateAll(const glm::vec3& translation, Space space = Space::LOCAL);
    void rotateAll(const glm::quat& rotation, Space space = Space::LOCAL);
    void scaleAll(const glm::vec3& scaleFactor);

    // Statystyki
    size_t getObjectCount() const { return m_objects.size(); }

private:
    std::string generateUniqueName(const std::string& base);
};

#endif // SCENE_MANAGER_HPP