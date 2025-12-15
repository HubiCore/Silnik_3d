#ifndef GEOMETRY_RENDERER_HPP
#define GEOMETRY_RENDERER_HPP

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Mesh {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    int indexCount;
};

class GeometryRenderer {
private:
    GLenum m_drawMode;
    Material m_currentMaterial;

    Mesh m_cubeMesh;
    Mesh m_sphereMesh;
    Mesh m_cylinderMesh;
    Mesh m_coneMesh;
    Mesh m_planeMesh;
    Mesh m_torusMesh;
    Mesh m_pyramidMesh;
    Mesh m_gridMesh;

    unsigned int m_lineVAO;
    unsigned int m_lineVBO;
    unsigned int m_pointVAO;
    unsigned int m_pointVBO;

    void setupMesh(Mesh& mesh, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void deleteMesh(Mesh& mesh);
    void createCube();
    void createSphere(int sectors = 32, int stacks = 32);
    void createCylinder(int sectors = 32);
    void createCone(int sectors = 32);
    void createPlane();
    void createTorus(float radius = 0.5f, float tubeRadius = 0.2f, int sectors = 32, int rings = 32);
    void createPyramid();
    void createGrid(int size = 10);

public:
    GeometryRenderer();
    ~GeometryRenderer();

    bool initialize();

    // Podstawowe funkcje rysowania
    void drawCube(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation);
    void drawSphere(const glm::vec3& position, float radius);
    void drawCylinder(const glm::vec3& position, float height, float radius);
    void drawCone(const glm::vec3& position, float height, float radius);
    void drawPlane(const glm::vec3& position, const glm::vec2& size);
    void drawTorus(const glm::vec3& position, float majorRadius, float minorRadius);
    void drawPyramid(const glm::vec3& position, float baseSize, float height);
    void drawGrid(const glm::vec3& position, int size, float spacing);

    // Funkcje pomocnicze
    void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.0f));
    void drawPoint(const glm::vec3& position, float size = 5.0f, const glm::vec3& color = glm::vec3(1.0f));
    void drawCoordinateSystem(float length = 1.0f);
    void draw3DGrid(const glm::vec3& center, int size, float spacing);
    void drawArrow(const glm::vec3& start, const glm::vec3& end, float headSize = 0.1f);
    void drawBox(const glm::vec3& min, const glm::vec3& max);
    void drawSphereWireframe(const glm::vec3& position, float radius, int segments = 32);
    void drawPolygon(const std::vector<glm::vec3>& vertices, bool filled = false);
    void drawCircle(const glm::vec3& center, float radius, int segments = 32);
    void drawDisk(const glm::vec3& center, float innerRadius, float outerRadius, int segments = 32);

    // Ustawienia
    void setMaterial(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess);
    void setColor(const glm::vec3& color);
    void setDrawMode(GLenum mode);
    void setModelMatrix(const glm::mat4& model);
    void setViewMatrix(const glm::mat4& view);
    void setProjectionMatrix(const glm::mat4& projection);
    void drawMesh(const Mesh& mesh);
};

#endif // GEOMETRY_RENDERER_HPP