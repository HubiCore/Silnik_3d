#ifndef TEXTURED_OBJECT_HPP
#define TEXTURED_OBJECT_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "BitmapHandler.hpp"

class TexturedObject {
protected:
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    GLuint m_textureID;

    int m_vertexCount;
    int m_indexCount;

    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;

    std::shared_ptr<BitmapHandler> m_texture;

    virtual void setupBuffers() = 0;

public:
    TexturedObject();
    virtual ~TexturedObject();

    // Ładowanie tekstury
    bool loadTexture(const std::string& filePath);
    void setTexture(std::shared_ptr<BitmapHandler> texture);

    // Transformacje
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);
    void translate(const glm::vec3& translation);
    void rotate(const glm::vec3& rotation);
    void scale(const glm::vec3& scale);

    // Gettery
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getRotation() const { return m_rotation; }
    glm::vec3 getScale() const { return m_scale; }
    GLuint getTextureID() const { return m_textureID; }

    // Renderowanie
    virtual void draw() const;
    virtual void drawWithTexture() const;

    // Tworzenie macierzy modelu
    glm::mat4 getModelMatrix() const;
};

class TexturedCube : public TexturedObject {
private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    void setupBuffers() override;

public:
    TexturedCube();
    ~TexturedCube();

    // Specyficzne dla sześcianu
    void create(float size = 1.0f);
};

class TexturedSphere : public TexturedObject {
private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    void setupBuffers() override;

public:
    TexturedSphere();
    ~TexturedSphere();

    void create(float radius = 1.0f, int sectors = 32, int stacks = 32);
};

class TexturedCylinder : public TexturedObject {
private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    void setupBuffers() override;

public:
    TexturedCylinder();
    ~TexturedCylinder();

    void create(float radius = 1.0f, float height = 2.0f, int sectors = 32);
};

#endif // TEXTURED_OBJECT_HPP