#ifndef BITMAP_HANDLER_HPP
#define BITMAP_HANDLER_HPP

#include <GL/glew.h>
#include <string>
#include <vector>

class BitmapHandler {
private:
    GLuint m_textureID;
    int m_width;
    int m_height;
    int m_channels;
    bool m_hasMipmaps;

    bool loadWithSTB(const std::string& filePath);

public:
    BitmapHandler();
    ~BitmapHandler();

    // Ładowanie tekstury
    bool loadTexture(const std::string& filePath);
    bool loadTextureWithMipmaps(const std::string& filePath);

    // Generowanie mipmap
    void generateMipmaps();

    // Ustawienia tekstury
    void setFiltering(GLenum minFilter, GLenum magFilter);
    void setWrapping(GLenum wrapS, GLenum wrapT);

    // Bind/Unbind
    void bind(GLenum textureUnit = GL_TEXTURE0) const;
    void unbind() const;

    // Gettery
    GLuint getTextureID() const { return m_textureID; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getChannels() const { return m_channels; }
    bool isValid() const { return m_textureID != 0; }

    // Usuwanie kopiowania
    BitmapHandler(const BitmapHandler&) = delete;
    BitmapHandler& operator=(const BitmapHandler&) = delete;

    // Przenoszenie
    BitmapHandler(BitmapHandler&& other) noexcept;
    BitmapHandler& operator=(BitmapHandler&& other) noexcept;
};

#endif // BITMAP_HANDLER_HPP