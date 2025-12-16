#include "Camera.hpp"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, CameraType type)
    : m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
      m_movementSpeed(2.5f),
      m_mouseSensitivity(0.05f),
      m_zoom(60.0f),
      m_type(type),
      m_orbitTarget(glm::vec3(0.0f)),
      m_orbitRadius(4.0f),
      m_orbitYaw(0.0f),
      m_orbitPitch(0.0f) {

    m_position = position;
    m_worldUp = up;
    m_yaw = yaw;
    m_pitch = pitch;

    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    switch (m_type) {
        case ORBIT: {
            // Oblicz pozycję kamery w trybie orbitalnym
            float x = m_orbitTarget.x + m_orbitRadius *
                     cos(glm::radians(m_orbitYaw)) * cos(glm::radians(m_orbitPitch));
            float y = m_orbitTarget.y + m_orbitRadius * sin(glm::radians(m_orbitPitch));
            float z = m_orbitTarget.z + m_orbitRadius *
                     sin(glm::radians(m_orbitYaw)) * cos(glm::radians(m_orbitPitch));

            glm::vec3 orbitPosition(x, y, z);
            return glm::lookAt(orbitPosition, m_orbitTarget, m_worldUp);
        }
        case FREE:
        case FPS:
        default:
            return glm::lookAt(m_position, m_position + m_front, m_up);
    }
}

void Camera::processKeyboard(MovementDirection direction, float deltaTime) {
    float velocity = m_movementSpeed * deltaTime;

    switch (direction) {
        case FORWARD:
            if (m_type == FPS) {
                // W trybie FPS poruszamy się tylko w płaszczyźnie XZ
                m_position += glm::vec3(m_front.x, 0.0f, m_front.z) * velocity;
            } else {
                m_position += m_front * velocity;
            }
            break;
        case BACKWARD:
            if (m_type == FPS) {
                m_position -= glm::vec3(m_front.x, 0.0f, m_front.z) * velocity;
            } else {
                m_position -= m_front * velocity;
            }
            break;
        case LEFT:
            m_position -= m_right * velocity;
            break;
        case RIGHT:
            m_position += m_right * velocity;
            break;
        case UP:
            m_position += m_worldUp * velocity;
            break;
        case DOWN:
            m_position -= m_worldUp * velocity;
            break;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    if (m_type == ORBIT) {
        // Orbitalny ruch kamery
        m_orbitYaw += xoffset * m_mouseSensitivity;
        m_orbitPitch += yoffset * m_mouseSensitivity;

        // Ograniczenia kąta pochylenia
        if (constrainPitch) {
            if (m_orbitPitch > 89.0f) m_orbitPitch = 89.0f;
            if (m_orbitPitch < -89.0f) m_orbitPitch = -89.0f;
        }
    } else {
        // Standardowy ruch kamery
        xoffset *= m_mouseSensitivity;
        yoffset *= m_mouseSensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // Ograniczenia kąta pochylenia
        if (constrainPitch) {
            if (m_pitch > 89.0f) m_pitch = 89.0f;
            if (m_pitch < -89.0f) m_pitch = -89.0f;
        }

        updateCameraVectors();
    }
}

void Camera::processMouseScroll(float yoffset) {
    m_zoom -= yoffset;
    if (m_zoom < 1.0f) m_zoom = 1.0f;
    if (m_zoom > 90.0f) m_zoom = 90.0f;

    if (m_type == ORBIT) {
        m_orbitRadius -= yoffset * 0.5f;
        if (m_orbitRadius < 1.0f) m_orbitRadius = 1.0f;
        if (m_orbitRadius > 50.0f) m_orbitRadius = 50.0f;
    }
}

void Camera::updateCameraVectors() {
    // Oblicz nowy wektor frontu
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);

    // Oblicz wektory prawo i góra
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::orbit(float deltaYaw, float deltaPitch) {
    m_orbitYaw += deltaYaw;
    m_orbitPitch += deltaPitch;

    // Ograniczenia kąta
    if (m_orbitPitch > 89.0f) m_orbitPitch = 89.0f;
    if (m_orbitPitch < -89.0f) m_orbitPitch = -89.0f;
}