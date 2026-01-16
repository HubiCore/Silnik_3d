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

/**
 * @brief Konstruktor domyślny BitmapHandler
 *
 * Inicjalizuje wszystkie pola zerami
 */
BitmapHandler::BitmapHandler()
    : m_textureID(0), m_width(0), m_height(0),
      m_channels(0), m_hasMipmaps(false) {}

/**
 * @brief Destruktor BitmapHandler
 *
 * Zwalnia zasoby tekstury OpenGL jeśli tekstura została załadowana
 */
BitmapHandler::~BitmapHandler() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
    }
}

/**
 * @brief Ładuje teksturę przy użyciu biblioteki STB Image
 * @param filePath Ścieżka do pliku obrazu
 * @return true jeśli ładowanie się powiodło, false w przeciwnym razie
 *
 * @details Funkcja używa biblioteki STB Image do wczytania obrazu,
 * automatycznie rozpoznaje format (JPEG, PNG, BMP, etc.) i konwertuje
 * na odpowiedni format OpenGL. Obraz jest automatycznie odwracany
 * w osi Y dla zgodności z OpenGL.
 */
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

/**
 * @brief Ładuje teksturę z pliku
 * @param filePath Ścieżka do pliku obrazu
 * @return true jeśli ładowanie się powiodło, false w przeciwnym razie
 *
 * @details Jeśli tekstura była już załadowana, zostanie najpierw usunięta
 */
bool BitmapHandler::loadTexture(const std::string& filePath) {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }

    return loadWithSTB(filePath);
}

/**
 * @brief Ładuje teksturę z pliku z automatycznym generowaniem mipmap
 * @param filePath Ścieżka do pliku obrazu
 * @return true jeśli ładowanie się powiodło, false w przeciwnym razie
 */
bool BitmapHandler::loadTextureWithMipmaps(const std::string& filePath) {
    if (!loadTexture(filePath)) {
        return false;
    }

    generateMipmaps();
    return true;
}

/**
 * @brief Generuje mipmapy dla aktualnie załadowanej tekstury
 *
 * @details Mipmapy są serią prekompresowanych wersji tekstury w różnych
 * rozmiarach, które poprawiają jakość renderowania przy skalowaniu
 */
void BitmapHandler::generateMipmaps() {
    if (m_textureID == 0) return;

    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glGenerateMipmap(GL_TEXTURE_2D);
    m_hasMipmaps = true;

    std::cout << "Wygenerowano mipmapy dla tekstury" << std::endl;
}

/**
 * @brief Ustawia parametry filtrowania tekstury
 * @param minFilter Filtr minifikacji (GL_NEAREST, GL_LINEAR, etc.)
 * @param magFilter Filtr magnifikacji (GL_NEAREST, GL_LINEAR)
 */
void BitmapHandler::setFiltering(GLenum minFilter, GLenum magFilter) {
    if (m_textureID == 0) return;

    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

/**
 * @brief Ustawia parametry opakowywania tekstury
 * @param wrapS Tryb opakowania dla osi S (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
 * @param wrapT Tryb opakowania dla osi T (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
 */
void BitmapHandler::setWrapping(GLenum wrapS, GLenum wrapT) {
    if (m_textureID == 0) return;

    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

/**
 * @brief Bindowanie tekstury do jednostki teksturującej
 * @param textureUnit Jednostka teksturująca (domyślnie GL_TEXTURE0)
 */
void BitmapHandler::bind(GLenum textureUnit) const {
    if (m_textureID == 0) return;

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

/**
 * @brief Odwiązanie tekstury
 */
void BitmapHandler::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief Konstruktor przenoszący
 * @param other Obiekt do przeniesienia
 *
 * @details Przenosi zasoby z innego obiektu, zerując oryginalny obiekt
 */
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

/**
 * @brief Operator przypisania przenoszącego
 * @param other Obiekt do przeniesienia
 * @return Referencja do bieżącego obiektu
 *
 * @details Przenosi zasoby z innego obiektu, najpierw zwalniając
 * własne zasoby jeśli były załadowane
 */
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