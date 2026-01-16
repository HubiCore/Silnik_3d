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

/**
 * @brief Wrapper callbacku klawiatury dla GLFW
 * @param window Okno GLFW
 * @param key Kod klawisza
 * @param scancode Kod skanowania klawisza
 * @param action Akcja (naciśnięcie, zwolnienie, przytrzymanie)
 * @param mods Modyfikatory (Shift, Ctrl, Alt)
 */
void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    void* userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        Engine* engine = static_cast<Engine*>(userPointer);
        engine->callKeyCallback(window, key, scancode, action, mods);
    }
}

/**
 * @brief Wrapper callbacku ruchu myszy dla GLFW
 * @param window Okno GLFW
 * @param xpos Pozycja X kursora
 * @param ypos Pozycja Y kursora
 */
void mouseMoveCallbackWrapper(GLFWwindow* window, double xpos, double ypos) {
    void* userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        Engine* engine = static_cast<Engine*>(userPointer);
        engine->callMouseMoveCallback(window, xpos, ypos);
    }
}

/**
 * @brief Wrapper callbacku przycisków myszy dla GLFW
 * @param window Okno GLFW
 * @param button Przycisk myszy
 * @param action Akcja (naciśnięcie, zwolnienie)
 * @param mods Modyfikatory (Shift, Ctrl, Alt)
 */
void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods) {
    void* userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        Engine* engine = static_cast<Engine*>(userPointer);
        engine->callMouseButtonCallback(window, button, action, mods);
    }
}

/**
 * @brief Wrapper callbacku zmiany rozmiaru okna dla GLFW
 * @param window Okno GLFW
 * @param width Nowa szerokość okna
 * @param height Nowa wysokość okna
 */
void resizeCallbackWrapper(GLFWwindow* window, int width, int height) {
    void* userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        Engine* engine = static_cast<Engine*>(userPointer);
        engine->callResizeCallback(window, width, height);
    }
    glViewport(0, 0, width, height);
}

/**
 * @brief Konstruktor silnika
 * @param width Szerokość okna
 * @param height Wysokość okna
 * @param fps Docelowa liczba klatek na sekundę
 * @param fullscreen Czy uruchomić w trybie pełnoekranowym
 */
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

/**
 * @brief Destruktor silnika
 *
 * Zamyka okno i zwalnia zasoby GLFW
 */
Engine::~Engine() {
    close();
}

/**
 * @brief Inicjalizacja biblioteki GLFW
 * @return true jeśli inicjalizacja się powiodła, false w przeciwnym razie
 */
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

/**
 * @brief Tworzy okno GLFW
 * @return true jeśli tworzenie się powiodło, false w przeciwnym razie
 */
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

/**
 * @brief Konfiguruje callbacki dla okna GLFW
 */
void Engine::setupCallbacks() {
    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, keyCallbackWrapper);
    glfwSetCursorPosCallback(m_window, mouseMoveCallbackWrapper);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallbackWrapper);
    glfwSetFramebufferSizeCallback(m_window, resizeCallbackWrapper);
}

/**
 * @brief Konfiguruje ustawienia OpenGL
 */
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

/**
 * @brief Inicjalizuje silnik
 * @return true jeśli inicjalizacja się powiodła, false w przeciwnym razie
 */
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

/**
 * @brief Uruchamia główną pętlę silnika
 * @param updateCallback Funkcja callback do aktualizacji logiki
 * @param renderCallback Funkcja callback do renderowania
 *
 * @details Pętla główna zarządza czasem, wywołuje callbacki, czyści ekran,
 * stosuje rzutowanie, zamienia bufory i obsługuje zdarzenia.
 * Implementuje kontrolę FPS z usypianiem wątku.
 */
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

/**
 * @brief Zamyka silnik i zwalnia zasoby
 */
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

/**
 * @brief Przełącza tryb pełnoekranowy
 * @param fullscreen true dla pełnego ekranu, false dla okna
 */
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

/**
 * @brief Zmienia rozdzielczość okna
 * @param width Nowa szerokość
 * @param height Nowa wysokość
 */
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

/**
 * @brief Ustawia docelową liczbę klatek na sekundę
 * @param fps Docelowe FPS
 */
void Engine::setFPS(int fps) {
    m_targetFPS = fps;
    m_frameTimeTarget = 1.0 / fps;
}

/**
 * @brief Włącza lub wyłącza podwójne buforowanie
 * @param enable true aby włączyć, false aby wyłączyć
 *
 * @note Zmiana tego ustawienia wymaga ponownego utworzenia okna
 */
void Engine::enableDoubleBuffering(bool enable) {
    m_enableDoubleBuffering = enable;
    if (m_window) {
        // Nalezy ponownie utworzyc okno, aby zmienic to ustawienie
        std::cout << "Uwaga: Zmiana wymaga ponownego utworzenia okna" << std::endl;
    }
}

/**
 * @brief Włącza lub wyłącza bufor głębokości
 * @param enable true aby włączyć, false aby wyłączyć
 */
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

/**
 * @brief Ustawia callback dla klawiatury
 * @param callback Funkcja callback
 */
void Engine::setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> callback) {
    m_keyCallback = callback;
}

/**
 * @brief Ustawia callback dla ruchu myszy
 * @param callback Funkcja callback
 */
void Engine::setMouseMoveCallback(std::function<void(GLFWwindow*, double, double)> callback) {
    m_mouseMoveCallback = callback;
}

/**
 * @brief Ustawia callback dla przycisków myszy
 * @param callback Funkcja callback
 */
void Engine::setMouseButtonCallback(std::function<void(GLFWwindow*, int, int, int)> callback) {
    m_mouseButtonCallback = callback;
}

/**
 * @brief Ustawia callback dla zmiany rozmiaru okna
 * @param callback Funkcja callback
 */
void Engine::setResizeCallback(std::function<void(GLFWwindow*, int, int)> callback) {
    m_resizeCallback = callback;
}

/**
 * @brief Ustawia kolor czyszczenia ekranu
 * @param r Składowa czerwona (0.0 - 1.0)
 * @param g Składowa zielona (0.0 - 1.0)
 * @param b Składowa niebieska (0.0 - 1.0)
 * @param a Składowa alpha (0.0 - 1.0, domyślnie 1.0)
 */
void Engine::setClearColor(float r, float g, float b, float a) {
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
    glClearColor(r, g, b, a);
}

/**
 * @brief Czyści ekran (bufor kolorów i głębokości)
 */
void Engine::clearScreen() {
    GLbitfield mask = GL_COLOR_BUFFER_BIT;
    if (m_enableDepthBuffer) {
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    glClear(mask);
}

/**
 * @brief Ustawia rzutowanie perspektywiczne
 * @param fov Pole widzenia w stopniach (domyślnie 45.0)
 * @param near Bliska płaszczyzna przycinania (domyślnie 0.1)
 * @param far Daleka płaszczyzna przycinania (domyślnie 100.0)
 */
void Engine::setPerspectiveProjection(float fov, float near, float far) {
    m_projectionMode = ProjectionMode::PERSPECTIVE;
    m_fov = fov;
    m_nearPlane = near;
    m_farPlane = far;
}

/**
 * @brief Ustawia rzutowanie ortograficzne
 * @param left Lewa granica widoku
 * @param right Prawa granica widoku
 * @param bottom Dolna granica widoku
 * @param top Górna granica widoku
 * @param near Bliska płaszczyzna przycinania (domyślnie -1.0)
 * @param far Daleka płaszczyzna przycinania (domyślnie 1.0)
 */
void Engine::setOrthographicProjection(float left, float right, float bottom, float top, float near, float far) {
    m_projectionMode = ProjectionMode::ORTHOGRAPHIC;
    m_nearPlane = near;
    m_farPlane = far;
}

/**
 * @brief Zastosowuje aktualne ustawienia rzutowania
 *
 * @note Ta metoda wymaga implementacji zarządzania macierzami.
 * W obecnej implementacji jest pusta.
 */
void Engine::applyProjection() {
    // Tymczasowo puste - implementacja zalezy od tego, jak zarzadzasz macierzami
}

/**
 * @brief Sprawdza, czy okno powinno zostać zamknięte
 * @return true jeśli okno powinno zostać zamknięte
 */
bool Engine::shouldClose() const {
    return glfwWindowShouldClose(m_window) || !m_isRunning;
}

/**
 * @brief Zwraca wskaźnik do okna GLFW
 * @return Wskaźnik do okna GLFW
 */
GLFWwindow* Engine::getWindow() const {
    return m_window;
}

/**
 * @brief Zamienia bufory (podwójne buforowanie)
 */
void Engine::swapBuffers() {
    glfwSwapBuffers(m_window);
}

/**
 * @brief Obsługuje zdarzenia GLFW
 */
void Engine::pollEvents() {
    glfwPollEvents();
}

/**
 * @brief Ustawia tryb kursora myszy
 * @param mode Tryb kursora (GLFW_CURSOR_NORMAL, GLFW_CURSOR_HIDDEN, GLFW_CURSOR_DISABLED)
 */
void Engine::setCursorMode(int mode) {
    if (m_window) {
        glfwSetInputMode(m_window, GLFW_CURSOR, mode);
    }
}

/**
 * @brief Włącza lub wyłącza obsługę myszy
 * @param enable true aby włączyć, false aby wyłączyć
 */
void Engine::enableMouseInput(bool enable) {
    if (m_window) {
        glfwSetInputMode(m_window, GLFW_CURSOR,
                        enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}

/**
 * @brief Zwraca aktualny czas GLFW
 * @return Aktualny czas w sekundach
 */
double Engine::getTime() const {
    return glfwGetTime();
}