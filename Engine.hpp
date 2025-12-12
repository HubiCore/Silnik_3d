// Engine.hpp
#ifndef ENGINE_HPP
#define ENGINE_HPP

// WAŻNE: Najpierw definiujemy GLEW_STATIC
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

// Najpierw dołączamy GLEW (musi być przed OpenGL)
#include <GL/glew.h>

// Następnie definiujemy GLFW_INCLUDE_NONE, aby GLFW nie dołączało OpenGL
#define GLFW_INCLUDE_NONE

// Teraz dołączamy GLFW
#include <GLFW/glfw3.h>

#include <functional>

// Mode rzutowania
enum class ProjectionMode {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

// Wrapper function declarations
void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseMoveCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
void resizeCallbackWrapper(GLFWwindow* window, int width, int height);

class Engine {
private:
    // Konfiguracja
    int m_resX;
    int m_resY;
    int m_targetFPS;
    bool m_isFullscreen;
    bool m_isRunning;

    // GLFW
    GLFWwindow* m_window;
    GLFWmonitor* m_monitor;
    const GLFWvidmode* m_videoMode;

    // Callbacki
    std::function<void(GLFWwindow*, int, int, int, int)> m_keyCallback;
    std::function<void(GLFWwindow*, double, double)> m_mouseMoveCallback;
    std::function<void(GLFWwindow*, int, int, int)> m_mouseButtonCallback;
    std::function<void(GLFWwindow*, int, int)> m_resizeCallback;

    // Ustawienia graficzne
    float m_clearColor[4];
    ProjectionMode m_projectionMode;
    float m_fov;
    float m_nearPlane;
    float m_farPlane;

    // Buforowanie
    bool m_enableDoubleBuffering;
    bool m_enableDepthBuffer;

    // Timer
    double m_lastFrameTime;
    double m_frameTimeTarget;

    // Delta time
    double m_deltaTime;
    double m_currentTime;
    double m_lastTime;

    // Prywatne metody
    bool initializeGLFW();
    bool createWindow();
    void setupCallbacks();
    void setupOpenGL();

    // Metody dostępowe dla wrapperów
    void callKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (m_keyCallback) {
            m_keyCallback(window, key, scancode, action, mods);
        }
    }

    void callMouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
        if (m_mouseMoveCallback) {
            m_mouseMoveCallback(window, xpos, ypos);
        }
    }

    void callMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        if (m_mouseButtonCallback) {
            m_mouseButtonCallback(window, button, action, mods);
        }
    }

    void callResizeCallback(GLFWwindow* window, int width, int height) {
        if (m_resizeCallback) {
            m_resizeCallback(window, width, height);
        }
    }

    // Deklaracje zaprzyjaźnionych funkcji
    friend void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void mouseMoveCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
    friend void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
    friend void resizeCallbackWrapper(GLFWwindow* window, int width, int height);

public:
    // Konstruktor i destruktor
    Engine(int width = 800, int height = 600, int fps = 60, bool fullscreen = false);
    ~Engine();

    // Inicjalizacja i główna pętla
    bool initialize();
    void run(std::function<void()> updateCallback, std::function<void()> renderCallback);
    void close();

    // Konfiguracja
    void setFullscreen(bool fullscreen);
    void setResolution(int width, int height);
    void setFPS(int fps);
    void enableDoubleBuffering(bool enable);
    void enableDepthBuffer(bool enable);

    // Obsługa wejścia
    void setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> callback);
    void setMouseMoveCallback(std::function<void(GLFWwindow*, double, double)> callback);
    void setMouseButtonCallback(std::function<void(GLFWwindow*, int, int, int)> callback);
    void setResizeCallback(std::function<void(GLFWwindow*, int, int)> callback);

    // Obsługa grafiki
    void setClearColor(float r, float g, float b, float a = 1.0f);
    void clearScreen();

    // Rzutowanie
    void setPerspectiveProjection(float fov = 45.0f, float near = 0.1f, float far = 100.0f);
    void setOrthographicProjection(float left, float right, float bottom, float top, float near = -1.0f, float far = 1.0f);
    void applyProjection();

    // Stan
    bool shouldClose() const;
    GLFWwindow* getWindow() const;
    void swapBuffers();
    void pollEvents();

    // Ustawienia myszy
    void setCursorMode(int mode);
    void enableMouseInput(bool enable);

    // Informacje
    int getWidth() const { return m_resX; }
    int getHeight() const { return m_resY; }
    float getAspectRatio() const { return static_cast<float>(m_resX) / m_resY; }
    bool isFullscreen() const { return m_isFullscreen; }

    // Czas
    double getTime() const;
    double getDeltaTime() const { return m_deltaTime; }
};

#endif // ENGINE_HPP