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

/**
 * @enum ProjectionMode
 * @brief Tryby rzutowania 3D dostępne w silniku
 */
enum class ProjectionMode {
    PERSPECTIVE,    /**< Rzutowanie perspektywiczne (realistyczne) */
    ORTHOGRAPHIC    /**< Rzutowanie ortograficzne (bez perspektywy) */
};

// Deklaracje funkcji wrapperów dla callbacków GLFW
void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseMoveCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
void resizeCallbackWrapper(GLFWwindow* window, int width, int height);

/**
 * @class Engine
 * @brief Główna klasa silnika 3D oparta na GLFW i OpenGL
 *
 * Zapewnia zarządzanie oknem, renderowaniem, obsługę zdarzeń i pętle główną.
 * Umożliwia konfigurację rozdzielczości, trybu pełnoekranowego, FPS oraz
 * obsługę różnych trybów rzutowania.
 */
class Engine {
private:
    // Konfiguracja
    int m_resX;                 /**< Szerokość okna w pikselach */
    int m_resY;                 /**< Wysokość okna w pikselach */
    int m_targetFPS;            /**< Docelowa liczba klatek na sekundę */
    bool m_isFullscreen;        /**< Flaga trybu pełnoekranowego */
    bool m_isRunning;           /**< Flaga aktywności silnika */

    // GLFW
    GLFWwindow* m_window;       /**< Wskaźnik do okna GLFW */
    GLFWmonitor* m_monitor;     /**< Wskaźnik do monitora głównego */
    const GLFWvidmode* m_videoMode; /**< Informacje o trybie wideo monitora */

    // Callbacki
    std::function<void(GLFWwindow*, int, int, int, int)> m_keyCallback;           /**< Callback klawiatury */
    std::function<void(GLFWwindow*, double, double)> m_mouseMoveCallback;         /**< Callback ruchu myszy */
    std::function<void(GLFWwindow*, int, int, int)> m_mouseButtonCallback;        /**< Callback przycisków myszy */
    std::function<void(GLFWwindow*, int, int)> m_resizeCallback;                  /**< Callback zmiany rozmiaru okna */

    // Ustawienia graficzne
    float m_clearColor[4];      /**< Kolor czyszczenia ekranu (RGBA) */
    ProjectionMode m_projectionMode; /**< Aktualny tryb rzutowania */
    float m_fov;                /**< Pole widzenia dla rzutowania perspektywicznego (w stopniach) */
    float m_nearPlane;          /**< Bliska płaszczyzna przycinania */
    float m_farPlane;           /**< Daleka płaszczyzna przycinania */

    // Buforowanie
    bool m_enableDoubleBuffering; /**< Flaga podwójnego buforowania */
    bool m_enableDepthBuffer;     /**< Flaga bufora głębokości (Z-buffer) */

    // Timer
    double m_lastFrameTime;     /**< Czas ostatniej klatki */
    double m_frameTimeTarget;   /**< Docelowy czas trwania klatki (1/FPS) */

    // Delta time
    double m_deltaTime;         /**< Czas od ostatniej klatki */
    double m_currentTime;       /**< Aktualny czas */
    double m_lastTime;          /**< Czas poprzedniej klatki */

    /**
     * @brief Inicjalizacja biblioteki GLFW
     * @return true jeśli inicjalizacja się powiodła, false w przeciwnym razie
     */
    bool initializeGLFW();

    /**
     * @brief Tworzy okno GLFW
     * @return true jeśli tworzenie się powiodło, false w przeciwnym razie
     */
    bool createWindow();

    /**
     * @brief Konfiguruje callbacki dla okna GLFW
     */
    void setupCallbacks();

    /**
     * @brief Konfiguruje ustawienia OpenGL
     */
    void setupOpenGL();

    // Metody dostępowe dla wrapperów

    /**
     * @brief Wywołuje callback klawiatury
     * @param window Okno GLFW
     * @param key Kod klawisza
     * @param scancode Kod skanowania klawisza
     * @param action Akcja (naciśnięcie, zwolnienie, przytrzymanie)
     * @param mods Modyfikatory (Shift, Ctrl, Alt)
     */
    void callKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (m_keyCallback) {
            m_keyCallback(window, key, scancode, action, mods);
        }
    }

    /**
     * @brief Wywołuje callback ruchu myszy
     * @param window Okno GLFW
     * @param xpos Pozycja X kursora
     * @param ypos Pozycja Y kursora
     */
    void callMouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
        if (m_mouseMoveCallback) {
            m_mouseMoveCallback(window, xpos, ypos);
        }
    }

    /**
     * @brief Wywołuje callback przycisków myszy
     * @param window Okno GLFW
     * @param button Przycisk myszy
     * @param action Akcja (naciśnięcie, zwolnienie)
     * @param mods Modyfikatory (Shift, Ctrl, Alt)
     */
    void callMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        if (m_mouseButtonCallback) {
            m_mouseButtonCallback(window, button, action, mods);
        }
    }

    /**
     * @brief Wywołuje callback zmiany rozmiaru okna
     * @param window Okno GLFW
     * @param width Nowa szerokość okna
     * @param height Nowa wysokość okna
     */
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
    /**
     * @brief Konstruktor silnika
     * @param width Szerokość okna (domyślnie 800)
     * @param height Wysokość okna (domyślnie 600)
     * @param fps Docelowa liczba klatek na sekundę (domyślnie 60)
     * @param fullscreen Czy uruchomić w trybie pełnoekranowym (domyślnie false)
     */
    Engine(int width = 800, int height = 600, int fps = 60, bool fullscreen = false);

    /**
     * @brief Destruktor silnika
     *
     * Zamyka okno i zwalnia zasoby GLFW
     */
    ~Engine();

    // Inicjalizacja i główna pętla

    /**
     * @brief Inicjalizuje silnik
     * @return true jeśli inicjalizacja się powiodła, false w przeciwnym razie
     */
    bool initialize();

    /**
     * @brief Uruchamia główną pętlę silnika
     * @param updateCallback Funkcja callback do aktualizacji logiki
     * @param renderCallback Funkcja callback do renderowania
     */
    void run(std::function<void()> updateCallback, std::function<void()> renderCallback);

    /**
     * @brief Zamyka silnik i zwalnia zasoby
     */
    void close();

    // Konfiguracja

    /**
     * @brief Przełącza tryb pełnoekranowy
     * @param fullscreen true dla pełnego ekranu, false dla okna
     */
    void setFullscreen(bool fullscreen);

    /**
     * @brief Zmienia rozdzielczość okna
     * @param width Nowa szerokość
     * @param height Nowa wysokość
     */
    void setResolution(int width, int height);

    /**
     * @brief Ustawia docelową liczbę klatek na sekundę
     * @param fps Docelowe FPS
     */
    void setFPS(int fps);

    /**
     * @brief Włącza lub wyłącza podwójne buforowanie
     * @param enable true aby włączyć, false aby wyłączyć
     */
    void enableDoubleBuffering(bool enable);

    /**
     * @brief Włącza lub wyłącza bufor głębokości
     * @param enable true aby włączyć, false aby wyłączyć
     */
    void enableDepthBuffer(bool enable);

    // Obsługa wejścia

    /**
     * @brief Ustawia callback dla klawiatury
     * @param callback Funkcja callback
     */
    void setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> callback);

    /**
     * @brief Ustawia callback dla ruchu myszy
     * @param callback Funkcja callback
     */
    void setMouseMoveCallback(std::function<void(GLFWwindow*, double, double)> callback);

    /**
     * @brief Ustawia callback dla przycisków myszy
     * @param callback Funkcja callback
     */
    void setMouseButtonCallback(std::function<void(GLFWwindow*, int, int, int)> callback);

    /**
     * @brief Ustawia callback dla zmiany rozmiaru okna
     * @param callback Funkcja callback
     */
    void setResizeCallback(std::function<void(GLFWwindow*, int, int)> callback);

    // Obsługa grafiki

    /**
     * @brief Ustawia kolor czyszczenia ekranu
     * @param r Składowa czerwona (0.0 - 1.0)
     * @param g Składowa zielona (0.0 - 1.0)
     * @param b Składowa niebieska (0.0 - 1.0)
     * @param a Składowa alpha (0.0 - 1.0, domyślnie 1.0)
     */
    void setClearColor(float r, float g, float b, float a = 1.0f);

    /**
     * @brief Czyści ekran (bufor kolorów i głębokości)
     */
    void clearScreen();

    // Rzutowanie

    /**
     * @brief Ustawia rzutowanie perspektywiczne
     * @param fov Pole widzenia w stopniach (domyślnie 45.0)
     * @param near Bliska płaszczyzna przycinania (domyślnie 0.1)
     * @param far Daleka płaszczyzna przycinania (domyślnie 100.0)
     */
    void setPerspectiveProjection(float fov = 45.0f, float near = 0.1f, float far = 100.0f);

    /**
     * @brief Ustawia rzutowanie ortograficzne
     * @param left Lewa granica widoku
     * @param right Prawa granica widoku
     * @param bottom Dolna granica widoku
     * @param top Górna granica widoku
     * @param near Bliska płaszczyzna przycinania (domyślnie -1.0)
     * @param far Daleka płaszczyzna przycinania (domyślnie 1.0)
     */
    void setOrthographicProjection(float left, float right, float bottom, float top, float near = -1.0f, float far = 1.0f);

    /**
     * @brief Zastosowuje aktualne ustawienia rzutowania
     *
     * @note Ta metoda wymaga implementacji zarządzania macierzami
     */
    void applyProjection();

    // Stan

    /**
     * @brief Sprawdza, czy okno powinno zostać zamknięte
     * @return true jeśli okno powinno zostać zamknięte
     */
    bool shouldClose() const;

    /**
     * @brief Zwraca wskaźnik do okna GLFW
     * @return Wskaźnik do okna GLFW
     */
    GLFWwindow* getWindow() const;

    /**
     * @brief Zamienia bufory (podwójne buforowanie)
     */
    void swapBuffers();

    /**
     * @brief Obsługuje zdarzenia GLFW
     */
    void pollEvents();

    // Ustawienia myszy

    /**
     * @brief Ustawia tryb kursora myszy
     * @param mode Tryb kursora (GLFW_CURSOR_NORMAL, GLFW_CURSOR_HIDDEN, GLFW_CURSOR_DISABLED)
     */
    void setCursorMode(int mode);

    /**
     * @brief Włącza lub wyłącza obsługę myszy
     * @param enable true aby włączyć, false aby wyłączyć
     */
    void enableMouseInput(bool enable);

    // Informacje

    /**
     * @brief Zwraca szerokość okna
     * @return Szerokość okna w pikselach
     */
    int getWidth() const { return m_resX; }

    /**
     * @brief Zwraca wysokość okna
     * @return Wysokość okna w pikselach
     */
    int getHeight() const { return m_resY; }

    /**
     * @brief Zwraca proporcje ekranu
     * @return Proporcje ekranu (szerokość/wysokość)
     */
    float getAspectRatio() const { return static_cast<float>(m_resX) / m_resY; }

    /**
     * @brief Sprawdza, czy silnik jest w trybie pełnoekranowym
     * @return true jeśli tryb pełnoekranowy jest aktywny
     */
    bool isFullscreen() const { return m_isFullscreen; }

    // Czas

    /**
     * @brief Zwraca aktualny czas GLFW
     * @return Aktualny czas w sekundach
     */
    double getTime() const;

    /**
     * @brief Zwraca czas delta (czas od ostatniej klatki)
     * @return Czas delta w sekundach
     */
    double getDeltaTime() const { return m_deltaTime; }
};

#endif // ENGINE_HPP