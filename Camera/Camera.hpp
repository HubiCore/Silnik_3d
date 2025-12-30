#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    // Typy kamery
    enum CameraType {
        FREE,       // Swobodna kamera
        FPS,        // Kamera pierwszoosobowa
        ORBIT       // Kamera orbitalna (trzeciego osoby)
    };

    // Kierunki ruchu
    enum MovementDirection {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f,
           CameraType type = FPS);

    // Uzyskanie macierzy widoku
    glm::mat4 getViewMatrix() const;

    // Ruch kamery
    void processKeyboard(MovementDirection direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);

    // Aktualizacja wektorów kamery
    void updateCameraVectors();

    // Gettery i settery
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getFront() const { return m_front; }
    glm::vec3 getUp() const { return m_up; }
    glm::vec3 getRight() const { return m_right; }
    glm::vec3 getWorldUp() const { return m_worldUp; }

    float getYaw() const { return m_yaw; }
    float getPitch() const { return m_pitch; }
    float getMovementSpeed() const { return m_movementSpeed; }
    float getMouseSensitivity() const { return m_mouseSensitivity; }
    float getZoom() const { return m_zoom; }
    CameraType getType() const { return m_type; }

    void setPosition(const glm::vec3& position) { m_position = position; }
    void setFront(const glm::vec3& front) { m_front = front; }
    void setUp(const glm::vec3& up) { m_up = up; }
    void setYaw(float yaw) { m_yaw = yaw; updateCameraVectors(); }
    void setPitch(float pitch) { m_pitch = pitch; updateCameraVectors(); }
    void setMovementSpeed(float speed) { m_movementSpeed = speed; }
    void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
    void setZoom(float zoom) { m_zoom = zoom; }
    void setType(CameraType type) { m_type = type; }

    // Tryb orbitalny (trzeciej osoby)
    void setOrbitTarget(const glm::vec3& target) { m_orbitTarget = target; }
    glm::vec3 getOrbitTarget() const { return m_orbitTarget; }
    void setOrbitRadius(float radius) { m_orbitRadius = radius; }
    float getOrbitRadius() const { return m_orbitRadius; }
    void orbit(float deltaYaw, float deltaPitch);

private:
    // Atrybuty kamery
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // Kąty Eulera
    float m_yaw;
    float m_pitch;

    // Opcje kamery
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;

    // Typ kamery
    CameraType m_type;

    // Parametry dla trybu orbitalnego
    glm::vec3 m_orbitTarget;
    float m_orbitRadius;
    float m_orbitYaw;
    float m_orbitPitch;
};

#endif // CAMERA_HPP