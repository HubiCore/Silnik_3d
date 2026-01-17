#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

/**
 * @class Camera
 * @brief Klasa reprezentująca kamerę 3D z różnymi trybami widoku.
 *
 * Klasa obsługuje trzy tryby kamery: swobodną, pierwszoosobową (FPS) oraz orbitalną (trzeciej osoby).
 * Umożliwia ruch kamery za pomocą klawiatury i myszy, oraz generowanie macierzy widoku.
 */
class Camera {
public:
    /**
     * @enum CameraType
     * @brief Typ kamery określający tryb jej działania.
     */
    enum CameraType {
        FREE,       /**< Swobodna kamera - porusza się we wszystkich kierunkach */
        FPS,        /**< Kamera pierwszoosobowa - ograniczona do płaszczyzny XZ */
        ORBIT       /**< Kamera orbitalna (trzeciego osoby) - obraca się wokół celu */
    };

    /**
     * @enum MovementDirection
     * @brief Kierunki ruchu kamery.
     */
    enum MovementDirection {
        FORWARD,    /**< Ruch do przodu */
        BACKWARD,   /**< Ruch do tyłu */
        LEFT,       /**< Ruch w lewo */
        RIGHT,      /**< Ruch w prawo */
        UP,         /**< Ruch w górę */
        DOWN        /**< Ruch w dół */
    };

    /**
     * @brief Konstruktor kamery.
     * @param position Pozycja początkowa kamery (domyślnie (0.0f, 0.0f, 3.0f))
     * @param up Wektor wskazujący górę świata (domyślnie (0.0f, 1.0f, 0.0f))
     * @param yaw Początkowy kąt odchylenia (yaw) w stopniach (domyślnie -90.0f)
     * @param pitch Początkowy kąt pochylenia (pitch) w stopniach (domyślnie 0.0f)
     * @param type Typ kamery (domyślnie FPS)
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f,
           CameraType type = FPS);

    /**
     * @brief Zwraca macierz widoku kamery.
     * @return Macierz widoku typu glm::mat4
     */
    glm::mat4 getViewMatrix() const;

    /**
     * @brief Obsługuje ruch kamery za pomocą klawiatury.
     * @param direction Kierunek ruchu
     * @param deltaTime Czas od ostatniej klatki (do płynnego ruchu)
     */
    void processKeyboard(MovementDirection direction, float deltaTime);

    /**
     * @brief Obsługuje ruch kamery za pomocą myszy.
     * @param xoffset Przesunięcie myszy w osi X
     * @param yoffset Przesunięcie myszy w osi Y
     * @param constrainPitch Czy ograniczać kąt pochylenia (domyślnie true)
     */
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    /**
     * @brief Obsługuje przewijanie kółkiem myszy (zoom).
     * @param yoffset Wartość przewinięcia
     */
    void processMouseScroll(float yoffset);

    /**
     * @brief Aktualizuje wektory kamery na podstawie kątów Eulera.
     */
    void updateCameraVectors();

    // Gettery
    glm::vec3 getPosition() const { return m_position; }           /**< @brief Zwraca pozycję kamery */
    glm::vec3 getFront() const { return m_front; }                 /**< @brief Zwraca wektor frontu kamery */
    glm::vec3 getUp() const { return m_up; }                       /**< @brief Zwraca wektor góry kamery */
    glm::vec3 getRight() const { return m_right; }                 /**< @brief Zwraca wektor prawej strony kamery */
    glm::vec3 getWorldUp() const { return m_worldUp; }             /**< @brief Zwraca wektor góry świata */

    float getYaw() const { return m_yaw; }                         /**< @brief Zwraca kąt odchylenia (yaw) */
    float getPitch() const { return m_pitch; }                     /**< @brief Zwraca kąt pochylenia (pitch) */
    float getMovementSpeed() const { return m_movementSpeed; }     /**< @brief Zwraca prędkość ruchu kamery */
    float getMouseSensitivity() const { return m_mouseSensitivity; } /**< @brief Zwraca czułość myszy */
    float getZoom() const { return m_zoom; }                       /**< @brief Zwraca wartość zoomu */
    CameraType getType() const { return m_type; }                  /**< @brief Zwraca typ kamery */

    // Settery
    void setPosition(const glm::vec3& position) { m_position = position; } /**< @brief Ustawia pozycję kamery */
    void setFront(const glm::vec3& front) { m_front = front; }             /**< @brief Ustawia wektor frontu kamery */
    void setUp(const glm::vec3& up) { m_up = up; }                         /**< @brief Ustawia wektor góry kamery */
    void setYaw(float yaw) { m_yaw = yaw; updateCameraVectors(); }         /**< @brief Ustawia kąt odchylenia i aktualizuje wektory */
    void setPitch(float pitch) { m_pitch = pitch; updateCameraVectors(); } /**< @brief Ustawia kąt pochylenia i aktualizuje wektory */
    void setMovementSpeed(float speed) { m_movementSpeed = speed; }        /**< @brief Ustawia prędkość ruchu kamery */
    void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; } /**< @brief Ustawia czułość myszy */
    void setZoom(float zoom) { m_zoom = zoom; }                           /**< @brief Ustawia wartość zoomu */
    void setType(CameraType type) { m_type = type; }                      /**< @brief Ustawia typ kamery */

    /**
     * @brief Ustawia cel dla trybu orbitalnego.
     * @param target Pozycja celu, wokół którego obraca się kamera
     */
    void setOrbitTarget(const glm::vec3& target) { m_orbitTarget = target; }

    /**
     * @brief Zwraca cel trybu orbitalnego.
     * @return Pozycja celu
     */
    glm::vec3 getOrbitTarget() const { return m_orbitTarget; }

    /**
     * @brief Ustawia promień orbity w trybie orbitalnym.
     * @param radius Promień orbity
     */
    void setOrbitRadius(float radius) { m_orbitRadius = radius; }

    /**
     * @brief Zwraca promień orbity.
     * @return Promień orbity
     */
    float getOrbitRadius() const { return m_orbitRadius; }

    /**
     * @brief Wykonuje ruch orbitalny kamery.
     * @param deltaYaw Zmiana kąta odchylenia
     * @param deltaPitch Zmiana kąta pochylenia
     */
    void orbit(float deltaYaw, float deltaPitch);

private:
    // Atrybuty kamery
    glm::vec3 m_position;      /**< Pozycja kamery */
    glm::vec3 m_front;         /**< Wektor wskazujący przód kamery */
    glm::vec3 m_up;            /**< Wektor wskazujący górę kamery */
    glm::vec3 m_right;         /**< Wektor wskazujący prawą stronę kamery */
    glm::vec3 m_worldUp;       /**< Wektor wskazujący górę świata */

    // Kąty Eulera
    float m_yaw;               /**< Kąt odchylenia (yaw) w stopniach */
    float m_pitch;             /**< Kąt pochylenia (pitch) w stopniach */

    // Opcje kamery
    float m_movementSpeed;     /**< Prędkość ruchu kamery */
    float m_mouseSensitivity;  /**< Czułość myszy */
    float m_zoom;              /**< Wartość zoomu (kąt widzenia) */

    // Typ kamery
    CameraType m_type;         /**< Typ kamery */

    // Parametry dla trybu orbitalnego
    glm::vec3 m_orbitTarget;   /**< Cel, wokół którego obraca się kamera w trybie orbitalnym */
    float m_orbitRadius;       /**< Promień orbity */
    float m_orbitYaw;          /**< Kąt odchylenia w trybie orbitalnym */
    float m_orbitPitch;        /**< Kąt pochylenia w trybie orbitalnym */
};

#endif // CAMERA_HPP