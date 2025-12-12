// Engine.cpp

// Najpierw definiujemy GLEW_STATIC
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

// Dolaczamy naglowki we wlasciwej kolejnosci:
// 1. GLEW
#include <GL/glew.h>
// 2. GLFW
#include <GLFW/glfw3.h>

#include "Engine.hpp"
#include <iostream>
#include <chrono>
#include <thread>

// Implementacje funkcji wrapper
void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    void* userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        Engine* engine = static_cast<Engine*>(userPointer);
        engine->callKeyCallback(window, key, scancode, action, mods);
    }
}

void mouseMoveCallbackWrapper(GLFWwindow* window, double xpos, double ypos) {
    void* userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        Engine* engine = static_cast<Engine*>(userPointer);
        engine->callMouseMoveCallback(window, xpos, ypos);
    }
}

void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods) {
    void* userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        Engine* engine = static_cast<Engine*>(userPointer);
        engine->callMouseButtonCallback(window, button, action, mods);
    }
}

void resizeCallbackWrapper(GLFWwindow* window, int width, int height) {
    void* userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        Engine* engine = static_cast<Engine*>(userPointer);
        engine->callResizeCallback(window, width, height);
    }
    glViewport(0, 0, width, height);
}

// Konstruktor
Engine::Engine(int width, int height, int fps, bool fullscreen)
    : m_resX(width), m_resY(height), m_targetFPS(fps),
      m_isFullscreen(fullscreen), m_isRunning(false),
      m_window(nullptr), m_monitor(nullptr), m_videoMode(nullptr),
      m_projectionMode(ProjectionMode::PERSPECTIVE),
      m_fov(45.0f), m_nearPlane(0.1f), m_farPlane(100.0f),
      m_enableDoubleBuffering(true), m_enableDepthBuffer(true),
      m_lastFrameTime(0.0), m_frameTimeTarget(1.0 / fps),
      m_deltaTime(0.0), m_currentTime(0.0), m_lastTime(0.0) {

    // Domyslny kolor tla
    m_clearColor[0] = 0.1f;
    m_clearColor[1] = 0.2f;
    m_clearColor[2] = 0.3f;
    m_clearColor[3] = 1.0f;
}

// Destruktor
Engine::~Engine() {
    close();
}

// Inicjalizacja GLFW
bool Engine::initializeGLFW() {
    if (!glfwInit()) {
        std::cerr << "Blad: Nie udalo sie zainicjalizowac GLFW!" << std::endl;
        return false;
    }

    // Konfiguracja GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, m_enableDoubleBuffering ? GL_TRUE : GL_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    std::cout << "GLFW zainicjalizowane pomyslnie" << std::endl;
    return true;
}

// Tworzenie okna
bool Engine::createWindow() {
    m_monitor = glfwGetPrimaryMonitor();
    m_videoMode = glfwGetVideoMode(m_monitor);

    if (m_isFullscreen) {
        m_window = glfwCreateWindow(m_videoMode->width, m_videoMode->height,
                                   "Silnik 3D", m_monitor, nullptr);
        if (m_window) {
            m_resX = m_videoMode->width;
            m_resY = m_videoMode->height;
        }
    } else {
        m_window = glfwCreateWindow(m_resX, m_resY, "Silnik 3D", nullptr, nullptr);
    }

    if (!m_window) {
        std::cerr << "Blad: Nie udalo sie utworzyc okna GLFW!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    // Inicjalizacja GLEW PO utworzeniu kontekstu OpenGL
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Blad inicjalizacji GLEW: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return false;
    }

    // Sprawdz wersje OpenGL
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW: " << glewGetString(GLEW_VERSION) << std::endl;

    glfwSwapInterval(1); // V-Sync

    return true;
}

// Ustawienie callbackow
void Engine::setupCallbacks() {
    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, keyCallbackWrapper);
    glfwSetCursorPosCallback(m_window, mouseMoveCallbackWrapper);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallbackWrapper);
    glfwSetFramebufferSizeCallback(m_window, resizeCallbackWrapper);
}

// Konfiguracja OpenGL
void Engine::setupOpenGL() {
    // Ustawienie koloru czyszczenia
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);

    // Bufor glebokosci (Z-buffer)
    if (m_enableDepthBuffer) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }

    // Usuwanie niewidocznych powierzchni
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Przezroczystosc
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Glowna inicjalizacja
bool Engine::initialize() {
    if (!initializeGLFW()) {
        return false;
    }

    if (!createWindow()) {
        return false;
    }

    setupCallbacks();
    setupOpenGL();

    m_lastFrameTime = glfwGetTime();
    m_isRunning = true;

    std::cout << "Silnik 3D zainicjalizowany pomyslnie" << std::endl;
    std::cout << "Rozdzielczosc: " << m_resX << "x" << m_resY << std::endl;
    std::cout << "Celowe FPS: " << m_targetFPS << std::endl;
    std::cout << "Tryb pelnoekranowy: " << (m_isFullscreen ? "Tak" : "Nie") << std::endl;

    return true;
}

// Glowna petla gry
void Engine::run(std::function<void()> updateCallback, std::function<void()> renderCallback) {
    if (!m_window || !m_isRunning) return;

    while (!glfwWindowShouldClose(m_window) && m_isRunning) {
        // Obliczanie czasu
        m_currentTime = glfwGetTime();
        m_deltaTime = m_currentTime - m_lastTime;
        m_lastTime = m_currentTime;

        // Sprawdzenie, czy nadszedl czas na nastepna klatke
        double currentTime = glfwGetTime();
        double elapsed = currentTime - m_lastFrameTime;

        if (elapsed >= m_frameTimeTarget) {
            // Czyszczenie ekranu
            clearScreen();

            // Zastosowanie rzutowania
            applyProjection();

            // Callback aktualizacji logiki
            if (updateCallback) {
                updateCallback();
            }

            // Callback renderowania
            if (renderCallback) {
                renderCallback();
            }

            // Zamiana buforow i obsluga zdarzen
            swapBuffers();
            pollEvents();

            m_lastFrameTime = currentTime;
        } else {
            // Uspienie dla utrzymania docelowego FPS
            double sleepTime = (m_frameTimeTarget - elapsed) * 1000.0;
            if (sleepTime > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleepTime)));
            }
        }
    }

    close();
}

// Zamkniecie silnika
void Engine::close() {
    m_isRunning = false;

    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        std::cout << "Okno zamkniete" << std::endl;
    }

    glfwTerminate();
    std::cout << "GLFW zakonczone" << std::endl;
}

// Ustawienie trybu pelnoekranowego
void Engine::setFullscreen(bool fullscreen) {
    if (fullscreen == m_isFullscreen || !m_window) return;

    m_isFullscreen = fullscreen;

    if (fullscreen) {
        // Przejscie do pelnego ekranu
        glfwSetWindowMonitor(m_window, m_monitor, 0, 0,
                            m_videoMode->width, m_videoMode->height,
                            m_videoMode->refreshRate);
        m_resX = m_videoMode->width;
        m_resY = m_videoMode->height;
    } else {
        // Przejscie do okna
        int xPos = 100;
        int yPos = 100;
        glfwSetWindowMonitor(m_window, nullptr, xPos, yPos, m_resX, m_resY, 0);
    }
}

// Zmiana rozdzielczosci
void Engine::setResolution(int width, int height) {
    if (!m_window) return;

    m_resX = width;
    m_resY = height;

    if (m_isFullscreen) {
        glfwSetWindowMonitor(m_window, m_monitor, 0, 0, width, height,
                            m_videoMode->refreshRate);
    } else {
        glfwSetWindowSize(m_window, width, height);
    }
}

// Ustawienie docelowego FPS
void Engine::setFPS(int fps) {
    m_targetFPS = fps;
    m_frameTimeTarget = 1.0 / fps;
}

// Wlaczenie/wylaczenie podwojnego buforowania
void Engine::enableDoubleBuffering(bool enable) {
    m_enableDoubleBuffering = enable;
    if (m_window) {
        // Nalezy ponownie utworzyc okno, aby zmienic to ustawienie
        std::cout << "Uwaga: Zmiana wymaga ponownego utworzenia okna" << std::endl;
    }
}

// Wlaczenie/wylaczenie bufora glebokosci
void Engine::enableDepthBuffer(bool enable) {
    m_enableDepthBuffer = enable;
    if (m_window) {
        if (enable) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }
}

// Ustawienie callbacka klawiatury
void Engine::setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> callback) {
    m_keyCallback = callback;
}

// Ustawienie callbacka ruchu myszy
void Engine::setMouseMoveCallback(std::function<void(GLFWwindow*, double, double)> callback) {
    m_mouseMoveCallback = callback;
}

// Ustawienie callbacka przyciskow myszy
void Engine::setMouseButtonCallback(std::function<void(GLFWwindow*, int, int, int)> callback) {
    m_mouseButtonCallback = callback;
}

// Ustawienie callbacka zmiany rozmiaru okna
void Engine::setResizeCallback(std::function<void(GLFWwindow*, int, int)> callback) {
    m_resizeCallback = callback;
}

// Ustawienie koloru czyszczenia
void Engine::setClearColor(float r, float g, float b, float a) {
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
    glClearColor(r, g, b, a);
}

// Czyszczenie ekranu
void Engine::clearScreen() {
    GLbitfield mask = GL_COLOR_BUFFER_BIT;
    if (m_enableDepthBuffer) {
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    glClear(mask);
}

// Ustawienie rzutowania perspektywicznego
void Engine::setPerspectiveProjection(float fov, float near, float far) {
    m_projectionMode = ProjectionMode::PERSPECTIVE;
    m_fov = fov;
    m_nearPlane = near;
    m_farPlane = far;
}

// Ustawienie rzutowania ortograficznego
void Engine::setOrthographicProjection(float left, float right, float bottom, float top, float near, float far) {
    m_projectionMode = ProjectionMode::ORTHOGRAPHIC;
    m_nearPlane = near;
    m_farPlane = far;
}

// Zastosowanie rzutowania
void Engine::applyProjection() {
    // Tymczasowo puste - implementacja zalezy od tego, jak zarzadzasz macierzami
}

// Sprawdzenie, czy okno powinno zostac zamkniete
bool Engine::shouldClose() const {
    return glfwWindowShouldClose(m_window) || !m_isRunning;
}

// Pobranie wskaznika do okna
GLFWwindow* Engine::getWindow() const {
    return m_window;
}

// Zamiana buforow
void Engine::swapBuffers() {
    glfwSwapBuffers(m_window);
}

// Obsluga zdarzen
void Engine::pollEvents() {
    glfwPollEvents();
}

// Ustawienie trybu kursora
void Engine::setCursorMode(int mode) {
    if (m_window) {
        glfwSetInputMode(m_window, GLFW_CURSOR, mode);
    }
}

// Wlaczenie/wylaczenie obslugi myszy
void Engine::enableMouseInput(bool enable) {
    if (m_window) {
        glfwSetInputMode(m_window, GLFW_CURSOR,
                        enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}

// Pobranie czasu
double Engine::getTime() const {
    return glfwGetTime();
}