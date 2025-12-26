#ifndef COMPLEX_OBJECT_HPP
#define COMPLEX_OBJECT_HPP
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <glm/glm.hpp>
#include "GeometryRenderer.hpp"


class ComplexObject {
public:
    ComplexObject();
    ~ComplexObject();

    void createLetterH(float width, float height, float depth, const glm::vec3& color = glm::vec3(0.9f, 0.2f, 0.2f));

    void draw() const;

    // Transformacje
    void setPosition(const glm::vec3& position);
    void setScale(const glm::vec3& scale);
    void setRotation(const glm::vec3& rotation);
    glm::mat4 getModelMatrix() const;

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getScale() const { return scale; }
    glm::vec3 getRotation() const { return rotation; }

    int getVertexCount() const { return vertexCount; }
    int getTriangleCount() const { return triangleCount; }

private:
    Mesh mesh;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    int vertexCount;
    int triangleCount;

    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void deleteMesh();
    void addCuboid(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const glm::vec3& position, const glm::vec3& size, const glm::vec3& color, const glm::vec2& uvScale = glm::vec2(1.0f));
    void addCylinder(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                               const glm::vec3& position, float height, float radius,
                               const glm::vec3& color, float rotationAngle, int sectors);
};

#endif