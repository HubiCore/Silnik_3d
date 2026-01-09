#include "BitmapHandler.hpp"
#include <iostream>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#ifdef WIN32
    #include <../stb-master/stb_image.h>
#endif
#ifdef __linux__
    #include <stb/stb_image.h>
#endif
BitmapHandler::BitmapHandler()
    : m_textureID(0), m_width(0), m_height(0), 
      m_channels(0), m_hasMipmaps(false) {}

BitmapHandler::~BitmapHandler() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
    }
}

bool BitmapHandler::loadWithSTB(const std::string& filePath) {
    // Flip obrazu dla OpenGLy
    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* data = stbi_load(filePath.c_str(), 
                                    &m_width, &m_height, 
                                    &m_channels, 0);
    
    if (!data) {
        std::cerr << "Błąd ładowania tekstury: " << filePath << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
        return false;
    }
    
    // Sprawdź czy rozmiar jest potęgą 2 (zalecane dla OpenGL)
    bool isPowerOfTwo = (m_width & (m_width - 1)) == 0 && 
                       (m_height & (m_height - 1)) == 0;
    
    if (!isPowerOfTwo) {
        std::cout << "UWAGA: Tekstura " << filePath 
                  << " nie ma rozmiaru będącego potęgą 2: " 
                  << m_width << "x" << m_height << std::endl;
    }
    
    // Format kanałów
    GLenum format;
    GLenum internalFormat;
    
    switch (m_channels) {
        case 1:
            format = GL_RED;
            internalFormat = GL_RED;
            break;
        case 2:
            format = GL_RG;
            internalFormat = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            internalFormat = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            internalFormat = GL_RGBA;
            break;
        default:
            stbi_image_free(data);
            return false;
    }
    
    // Generuj teksturę OpenGL
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    
    // Przekaż dane do OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 
                m_width, m_height, 0, 
                format, GL_UNSIGNED_BYTE, data);
    
    // Ustaw domyślne filtry
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Zwolnij dane STB
    stbi_image_free(data);
    
    std::cout << "Załadowano teksturę: " << filePath 
              << " (" << m_width << "x" << m_height 
              << ", kanały: " << m_channels << ")" << std::endl;
    
    return true;
}

bool BitmapHandler::loadTexture(const std::string& filePath) {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    
    return loadWithSTB(filePath);
}

bool BitmapHandler::loadTextureWithMipmaps(const std::string& filePath) {
    if (!loadTexture(filePath)) {
        return false;
    }
    
    generateMipmaps();
    return true;
}

void BitmapHandler::generateMipmaps() {
    if (m_textureID == 0) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glGenerateMipmap(GL_TEXTURE_2D);
    m_hasMipmaps = true;
    
    std::cout << "Wygenerowano mipmapy dla tekstury" << std::endl;
}

void BitmapHandler::setFiltering(GLenum minFilter, GLenum magFilter) {
    if (m_textureID == 0) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void BitmapHandler::setWrapping(GLenum wrapS, GLenum wrapT) {
    if (m_textureID == 0) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

void BitmapHandler::bind(GLenum textureUnit) const {
    if (m_textureID == 0) return;
    
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void BitmapHandler::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

BitmapHandler::BitmapHandler(BitmapHandler&& other) noexcept
    : m_textureID(other.m_textureID),
      m_width(other.m_width),
      m_height(other.m_height),
      m_channels(other.m_channels),
      m_hasMipmaps(other.m_hasMipmaps) {
    other.m_textureID = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_channels = 0;
    other.m_hasMipmaps = false;
}

BitmapHandler& BitmapHandler::operator=(BitmapHandler&& other) noexcept {
    if (this != &other) {
        if (m_textureID != 0) {
            glDeleteTextures(1, &m_textureID);
        }
        
        m_textureID = other.m_textureID;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;
        m_hasMipmaps = other.m_hasMipmaps;
        
        other.m_textureID = 0;
        other.m_width = 0;
        other.m_height = 0;
        other.m_channels = 0;
        other.m_hasMipmaps = false;
    }
    return *this;
}