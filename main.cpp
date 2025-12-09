#include "Engine.hpp"
#include <iostream>

// Callback klawiatury
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        std::cout << "Klawisz F naciśnięty" << std::endl;
    }
}

// Callback myszy
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    // Można tu dodać obsługę kamery
    // std::cout << "Mysz: x=" << xpos << ", y=" << ypos << std::endl;
}

// Callback przycisku myszy
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "Lewy przycisk myszy naciśnięty" << std::endl;
    }
}

// Callback zmiany rozmiaru okna
void resizeCallback(GLFWwindow* window, int width, int height) {
    std::cout << "Rozmiar okna zmieniony na: " << width << "x" << height << std::endl;
}

// Funkcja aktualizacji
void update() {
    // Logika gry tutaj
}

// Funkcja renderowania
void render() {
    // Prosty render trójkąta (używając starego OpenGL dla prostoty)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    glEnd();
}

int main() {
    // Utworzenie silnika
    Engine engine(800, 600, 60, false);

    // Inicjalizacja
    if (!engine.initialize()) {
        std::cerr << "Nie udało się zainicjalizować silnika!" << std::endl;
        return -1;
    }

    // Ustawienie callbacków
    engine.setKeyCallback(keyCallback);
    engine.setMouseMoveCallback(mouseCallback);
    engine.setMouseButtonCallback(mouseButtonCallback);
    engine.setResizeCallback(resizeCallback);

    // Ustawienie koloru tła
    engine.setClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    std::cout << "Naciśnij ESC aby wyjść" << std::endl;
    std::cout << "Naciśnij F aby testować" << std::endl;

    // Uruchomienie głównej pętli
    engine.run(update, render);

    return 0;
}
