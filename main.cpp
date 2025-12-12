// main.cpp
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Engine.hpp"
#include "GeometryRenderer.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

// Simple shader program
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec3 VertexColor;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
    VertexColor = aColor;
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 VertexColor;
in vec2 TexCoord;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform bool useVertexColor;

void main()
{
    vec3 finalColor = useVertexColor ? VertexColor : objectColor;

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * finalColor;
    FragColor = vec4(result, 1.0);
}
)";

GLuint shaderProgram;

// Callback klawiatury
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        std::cout << "Klawisz F nacisniety" << std::endl;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        std::cout << "Spacja nacisnieta" << std::endl;
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        std::cout << "Tryb: Wypelnione trojkaty" << std::endl;
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        std::cout << "Tryb: Linie (wireframe)" << std::endl;
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        std::cout << "Tryb: Punkty" << std::endl;
    }
}

// Callback myszy
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = xpos;
    static double lastY = ypos;

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
}

// Callback przycisku myszy
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "Lewy przycisk myszy nacisniety" << std::endl;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        std::cout << "Prawy przycisk myszy nacisniety" << std::endl;
    }
}

// Callback zmiany rozmiaru okna
void resizeCallback(GLFWwindow* window, int width, int height) {
    std::cout << "Rozmiar okna zmieniony na: " << width << "x" << height << std::endl;
    glViewport(0, 0, width, height);
}

// Zmienne globalne dla animacji
float rotationAngle = 0.0f;
float cubeRotation = 0.0f;
GeometryRenderer* geometryRenderer = nullptr;
int renderMode = 0; // 0 = wszystkie kształty, 1 = tylko zadania z instrukcji

// Macierze
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;

// Oswietlenie
glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 viewPos(0.0f, 3.0f, 8.0f);

// Funkcja kompilacji shadera
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Sprawdz bledy kompilacji
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Blad kompilacji shadera:\n" << infoLog << std::endl;
    }

    return shader;
}

// Utworz program shaderowy
void createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Sprawdz bledy linkowania
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Blad linkowania programu:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Funkcja aktualizacji
void update() {
    rotationAngle += 0.5f;
    if (rotationAngle > 360.0f) rotationAngle -= 360.0f;

    cubeRotation += 1.0f;
    if (cubeRotation > 360.0f) cubeRotation -= 360.0f;

    // Oswietlenie krazy wokol sceny
    float lightX = sin(glfwGetTime()) * 5.0f;
    float lightZ = cos(glfwGetTime()) * 5.0f;
    lightPos = glm::vec3(lightX, 5.0f, lightZ);
}



// Funkcja renderowania z uzyciem GeometryRenderer
void render() {
    if (!geometryRenderer) return;

    // Ustaw macierze
    projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
    view = glm::lookAt(
        viewPos,                       // Pozycja kamery
        glm::vec3(0.0f, 0.0f, 0.0f),  // Punkt, na ktory patrzy kamera
        glm::vec3(0.0f, 1.0f, 0.0f)   // Wektor "gora" kamery
    );

    geometryRenderer->setProjectionMatrix(projection);
    geometryRenderer->setViewMatrix(view);

    // Uzywaj shadera
    glUseProgram(shaderProgram);

    // Pobierz lokalizacje uniformow
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    GLint useVertexColorLoc = glGetUniformLocation(shaderProgram, "useVertexColor");

    // Ustaw uniformy wspolne dla wszystkich obiektow
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

    // Przełącz między trybami renderowania
    if (renderMode == 0) {
        // Tryb domyślny: wszystkie kształty
        geometryRenderer->setDrawMode(GL_TRIANGLES);

        // 1. Rysowanie szescianu w srodku sceny (obracajacy sie)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(cubeRotation), glm::vec3(1.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(useVertexColorLoc, 0); // Użyj koloru obiektu
        glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.2f); // Pomaranczowy
        geometryRenderer->drawCube(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(cubeRotation, cubeRotation * 0.7f, 0.0f));

        // 2. Rysowanie kuli po prawej stronie
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(useVertexColorLoc, 0); // Użyj koloru obiektu
        glUniform3f(objectColorLoc, 0.2f, 0.8f, 0.2f); // Zielony
        geometryRenderer->drawSphere(glm::vec3(3.0f, 1.0f, 0.0f), 0.8f);

        // 3. Rysowanie walca po lewej stronie - POPRAWIONE
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(useVertexColorLoc, 1); // Użyj kolorów wierzchołków z siatki
        glUniform3f(objectColorLoc, 0.2f, 0.5f, 1.0f); // Niebieski (backup)
        geometryRenderer->drawCylinder(glm::vec3(-3.0f, 0.0f, 0.0f), 2.0f, 0.5f);

        // 4. Rysowanie stozka
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(useVertexColorLoc, 1); // Użyj kolorów wierzchołków
        glUniform3f(objectColorLoc, 1.0f, 0.2f, 0.8f); // Rozowy (backup)
        geometryRenderer->drawCone(glm::vec3(0.0f, 0.0f, 3.0f), 1.5f, 0.7f);

        // 5. Rysowanie piramidy
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(useVertexColorLoc, 1); // Użyj kolorów wierzchołków
        glUniform3f(objectColorLoc, 1.0f, 1.0f, 0.0f); // Zolty (backup)
        geometryRenderer->drawPyramid(glm::vec3(0.0f, 0.0f, -3.0f), 1.0f, 1.5f);

        // 6. Rysowanie plaszczyzny (podlogi)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(useVertexColorLoc, 1); // Użyj kolorów wierzchołków
        glUniform3f(objectColorLoc, 0.3f, 0.3f, 0.3f); // Szary (backup)
        geometryRenderer->drawPlane(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec2(20.0f, 20.0f));

        // 7. Rysowanie torusa
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, 1.0f, 3.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(useVertexColorLoc, 1); // Użyj kolorów wierzchołków
        glUniform3f(objectColorLoc, 0.8f, 0.2f, 0.2f); // Czerwony (backup)
        geometryRenderer->drawTorus(glm::vec3(4.0f, 1.0f, 3.0f), 1.0f, 0.3f);

        // 8. Rysowanie siatki
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(useVertexColorLoc, 1); // Użyj kolorów wierzchołków
        glUniform3f(objectColorLoc, 0.5f, 0.5f, 0.5f); // Szary (backup)
        geometryRenderer->drawGrid(glm::vec3(0.0f, -2.0f, 0.0f), 20, 1.0f);
    } else {
        // Tryb zadań z instrukcji
        glUniform1i(useVertexColorLoc, 1); // Użyj kolorów wierzchołków
    }

    // 9. Rysowanie linii (uklad wspolrzednych)
    geometryRenderer->setDrawMode(GL_LINES);
    glUniform1i(useVertexColorLoc, 0); // Użyj koloru obiektu
    // Os X - czerwona
    glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
    geometryRenderer->drawLine(glm::vec3(0.0f), glm::vec3(3.0f, 0.0f, 0.0f));
    // Os Y - zielona
    glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);
    geometryRenderer->drawLine(glm::vec3(0.0f), glm::vec3(0.0f, 3.0f, 0.0f));
    // Os Z - niebieska
    glUniform3f(objectColorLoc, 0.0f, 0.0f, 1.0f);
    geometryRenderer->drawLine(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 3.0f));

    // 10. Rysowanie punktu (zrodlo swiatla)
    geometryRenderer->setDrawMode(GL_POINTS);
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f); // Bialy
    geometryRenderer->drawPoint(lightPos, 10.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    // Przywróć tryb rysowania
    geometryRenderer->setDrawMode(GL_TRIANGLES);
}

int main() {
    std::cout << "=== SILNIK 3D===" << std::endl;
    std::cout << "\nInicjalizacja..." << std::endl;

    // Utworzenie silnika
    Engine engine(800, 600, 60, false);

    // Inicjalizacja silnika
    if (!engine.initialize()) {
        std::cerr << "Nie udalo sie zainicjalizowac silnika!" << std::endl;
        return -1;
    }

    // Utworzenie i inicjalizacja GeometryRenderer
    GeometryRenderer renderer;
    if (!renderer.initialize()) {
        std::cerr << "Nie udalo sie zainicjalizowac GeometryRenderer!" << std::endl;
        return -1;
    }

    geometryRenderer = &renderer;

    // Utworz shadery
    createShaderProgram();

    // Ustawienie callbackow
    engine.setKeyCallback(keyCallback);
    engine.setMouseMoveCallback(mouseCallback);
    engine.setMouseButtonCallback(mouseButtonCallback);
    engine.setResizeCallback(resizeCallback);

    // Ustawienie koloru tla
    engine.setClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Ciemnoszary

    std::cout << "\n=== STEROWANIE ===" << std::endl;
    std::cout << "ESC: Wyjscie" << std::endl;
    std::cout << "1: Tryb wypelniony (GL_FILL)" << std::endl;
    std::cout << "2: Tryb linie (GL_LINE)" << std::endl;
    std::cout << "3: Tryb punkty (GL_POINT)" << std::endl;
    std::cout << "==================" << std::endl;

    // Uruchomienie glownej petli
    engine.run(update, render);

    geometryRenderer = nullptr;

    std::cout << "\nProgram zakonczony" << std::endl;

    return 0;
}