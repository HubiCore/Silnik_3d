#ifndef BITMAP_HANDLER_HPP
#define BITMAP_HANDLER_HPP

#include <GL/glew.h>
#include <string>
#include <vector>

/**
 * @class BitmapHandler
 * @brief Klasa do zarządzania teksturami bitmapowymi w OpenGL
 *
 * Obsługuje ładowanie, przechowywanie i zarządzanie teksturami 2D
 * przy użyciu biblioteki STB Image. Zapewnia funkcjonalność
 * mipmapping, filtrowania i opakowywania tekstur.
 */
class BitmapHandler {
private:
    GLuint m_textureID;    /**< ID tekstury OpenGL */
    int m_width;           /**< Szerokość tekstury w pikselach */
    int m_height;          /**< Wysokość tekstury w pikselach */
    int m_channels;        /**< Liczba kanałów kolorów (1, 2, 3, 4) */
    bool m_hasMipmaps;     /**< Flaga wskazująca czy wygenerowano mipmapy */

    /**
     * @brief Ładuje teksturę przy użyciu biblioteki STB Image
     * @param filePath Ścieżka do pliku obrazu
     * @return true jeśli ładowanie się powiodło, false w przeciwnym razie
     */
    bool loadWithSTB(const std::string& filePath);

public:
    /**
     * @brief Konstruktor domyślny BitmapHandler
     */
    BitmapHandler();

    /**
     * @brief Destruktor BitmapHandler
     *
     * Zwalnia zasoby tekstury OpenGL
     */
    ~BitmapHandler();

    // Ładowanie tekstur

    /**
     * @brief Ładuje teksturę z pliku
     * @param filePath Ścieżka do pliku obrazu
     * @return true jeśli ładowanie się powiodło, false w przeciwnym razie
     */
    bool loadTexture(const std::string& filePath);

    /**
     * @brief Ładuje teksturę z pliku z automatycznym generowaniem mipmap
     * @param filePath Ścieżka do pliku obrazu
     * @return true jeśli ładowanie się powiodło, false w przeciwnym razie
     */
    bool loadTextureWithMipmaps(const std::string& filePath);

    /**
     * @brief Generuje mipmapy dla aktualnie załadowanej tekstury
     */
    void generateMipmaps();

    // Ustawienia tekstury

    /**
     * @brief Ustawia parametry filtrowania tekstury
     * @param minFilter Filtr minifikacji (GL_NEAREST, GL_LINEAR, etc.)
     * @param magFilter Filtr magnifikacji (GL_NEAREST, GL_LINEAR)
     */
    void setFiltering(GLenum minFilter, GLenum magFilter);

    /**
     * @brief Ustawia parametry opakowywania tekstury
     * @param wrapS Tryb opakowania dla osi S (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
     * @param wrapT Tryb opakowania dla osi T (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
     */
    void setWrapping(GLenum wrapS, GLenum wrapT);

    // Bind/Unbind

    /**
     * @brief Bindowanie tekstury do jednostki teksturującej
     * @param textureUnit Jednostka teksturująca (domyślnie GL_TEXTURE0)
     */
    void bind(GLenum textureUnit = GL_TEXTURE0) const;

    /**
     * @brief Odwiązanie tekstury
     */
    void unbind() const;

    // Gettery

    /**
     * @brief Zwraca ID tekstury OpenGL
     * @return ID tekstury
     */
    GLuint getTextureID() const { return m_textureID; }

    /**
     * @brief Zwraca szerokość tekstury
     * @return Szerokość w pikselach
     */
    int getWidth() const { return m_width; }

    /**
     * @brief Zwraca wysokość tekstury
     * @return Wysokość w pikselach
     */
    int getHeight() const { return m_height; }

    /**
     * @brief Zwraca liczbę kanałów kolorów
     * @return Liczba kanałów (1, 2, 3 lub 4)
     */
    int getChannels() const { return m_channels; }

    /**
     * @brief Sprawdza czy tekstura jest poprawnie załadowana
     * @return true jeśli tekstura jest załadowana, false w przeciwnym razie
     */
    bool isValid() const { return m_textureID != 0; }

    // Usuwanie kopiowania

    /**
     * @brief Usunięty konstruktor kopiujący
     */
    BitmapHandler(const BitmapHandler&) = delete;

    /**
     * @brief Usunięty operator przypisania kopiującego
     */
    BitmapHandler& operator=(const BitmapHandler&) = delete;

    // Przenoszenie

    /**
     * @brief Konstruktor przenoszący
     * @param other Obiekt do przeniesienia
     */
    BitmapHandler(BitmapHandler&& other) noexcept;

    /**
     * @brief Operator przypisania przenoszącego
     * @param other Obiekt do przeniesienia
     * @return Referencja do bieżącego obiektu
     */
    BitmapHandler& operator=(BitmapHandler&& other) noexcept;
};

#endif // BITMAP_HANDLER_HPP