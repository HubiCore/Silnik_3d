// main.cpp
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ComplexObject.hpp"
#include "Engine.hpp"
#include "GeometryRenderer.hpp"
#include "Camera/Camera.hpp"
#include "SceneManager/SceneManager.hpp"
#include "Transform/TransformableGeometry.hpp"
#include "BitmapHandler.hpp"
#include "TexturedObject.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

// Simple shader program with multiple lights
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform bool useTexture;

// Struktura dla światła
struct Light {
    vec3 position;
    vec3 direction;
    vec3 color;
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
    float constant;
    float linear;
    float quadratic;
    float cutoff; // dla światła stożkowego
    float outerCutoff; // dla światła stożkowego
    int type; // 0 = punktowe, 1 = kierunkowe, 2 = stożkowe
};

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int activeLightCount;
uniform int currentLightMode; // 0 = tylko pierwsze światło, 1 = tylko drugie światło, 2 = wszystkie

// Funkcja obliczająca oświetlenie Phonga dla danego światła
vec3 calculatePhongLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectColor) {
    vec3 lightDir;
    float attenuation = 1.0;

    // Obliczenie kierunku światła i tłumienia w zależności od typu
    if (light.type == 1) { // światło kierunkowe
        lightDir = normalize(-light.direction);
        attenuation = 1.0; // brak tłumienia dla światła kierunkowego
    } else { // światło punktowe lub stożkowe
        lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance +
                           light.quadratic * (distance * distance));

        // Sprawdzenie dla światła stożkowego
        if (light.type == 2) {
            float theta = dot(lightDir, normalize(-light.direction));
            float epsilon = light.cutoff - light.outerCutoff;
            float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
            attenuation *= intensity;
        }
    }

    // Ambient
    vec3 ambient = light.ambientIntensity * light.color;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuseIntensity * diff * light.color;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specularIntensity * spec * light.color;

    // Połącz wszystkie składowe
    return (ambient + diffuse + specular) * attenuation;
}

void main()
{
    vec3 color;
    if (useTexture) {
        color = texture(texture1, TexCoord).rgb;
    } else {
        color = objectColor;
    }

    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    // Oblicz oświetlenie dla aktywnych świateł
    if (currentLightMode == 0) {
        // Tylko pierwsze światło
        result += calculatePhongLight(lights[0], normal, FragPos, viewDir, color);
    } else if (currentLightMode == 1) {
        // Tylko drugie światło
        result += calculatePhongLight(lights[1], normal, FragPos, viewDir, color);
    } else if (currentLightMode == 2) {
        // Wszystkie światła
        for (int i = 0; i < min(activeLightCount, 2); i++) {
            result += calculatePhongLight(lights[i], normal, FragPos, viewDir, color);
        }
    }

    // Mieszanie z kolorem obiektu
    result *= color;

    FragColor = vec4(result, 1.0);
}
)";

static Engine* globalEngine = nullptr;

bool autoBackgroundChange = false;
glm::vec4 currentBackgroundColor(0.1f, 0.1f, 0.1f, 1.0f);

GLuint shaderProgram;

//Texture
TexturedCube texturedCube;
TexturedSphere texturedSphere;
TexturedCylinder texturedCylinder;

BitmapHandler textureSphere; //tekstura dla kuli
BitmapHandler textureCylinder; //tekstura dla cylindra
BitmapHandler texture; //teksura dla sześcianu
bool useTextures = true;

// Camera
Camera camera(glm::vec3(0.0f, 2.0f, 8.0f));
bool firstMouse = true;
float lastX = 400, lastY = 300;

// Variables for camera control
bool cameraEnabled = true;
Camera::CameraType cameraType = Camera::FPS;

// Zmienne globalne dla animacji
float rotationAngle = 0.0f;
float cubeRotation = 0.0f;
GeometryRenderer* geometryRenderer = nullptr;
int renderMode = 0; // 0 = wszystkie kształty, 1 = tylko zadania z instrukcji

// Macierze
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;

// Oświetlenie - dwa źródła światła
struct Light {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
    float constant;
    float linear;
    float quadratic;
    float cutoff;
    float outerCutoff;
    int type; // 0 = punktowe, 1 = kierunkowe, 2 = stożkowe
};

Light lights[8]; // Maksymalnie 8 świateł jak w OpenGL
int activeLightCount = 2; // Aktywnych 2 światła
int currentLightMode = 2; // 0 = tylko pierwsze, 1 = tylko drugie, 2 = wszystkie
glm::vec3 viewPos(0.0f, 3.0f, 8.0f);

// Zmienne globalne dla nowego systemu transformacji
SceneManager* sceneManager = nullptr;
TransformableObject* rotatingCube = nullptr;
TransformableObject* orbitingSphere = nullptr;
ComplexObjectWithTransform* letterHObject = nullptr;
TransformableObject* parentCube = nullptr;
TransformableObject* childSphere1 = nullptr;
TransformableObject* childSphere2 = nullptr;


// Callback klawiatury
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        static int bgColor = 0;
        bgColor = (bgColor + 1) % 5;

        switch(bgColor) {
            case 0:
                currentBackgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
                std::cout << "Kolor tla: Szary" << std::endl;
                break;
            case 1:
                currentBackgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                std::cout << "Kolor tla: Czarny" << std::endl;
                break;
            case 2:
                currentBackgroundColor = glm::vec4(1.0f,    1.0f, 1.0f, 1.0f);
                std::cout << "Kolor tla: Bialy" << std::endl;
                break;
            case 3:
                currentBackgroundColor = glm::vec4(0.0f, 0.1f, 0.3f, 1.0f);
                std::cout << "Kolor tla: Niebieski" << std::endl;
                break;
            case 4:
                currentBackgroundColor = glm::vec4(0.1f, 0.0f, 0.0f, 1.0f);
                std::cout << "Kolor tla: Czerwony" << std::endl;
                break;
        }

        // Ustaw kolor natychmiast
        globalEngine->setClearColor(currentBackgroundColor.r, currentBackgroundColor.g,
                            currentBackgroundColor.b, currentBackgroundColor.a);
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        useTextures = !useTextures;
        std::cout << "Tekstury: " << (useTextures ? "WLACZONE" : "WYLACZONE") << std::endl;
    }

    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        autoBackgroundChange = !autoBackgroundChange;
        std::cout << "Automatyczna zmiana tla: " << (autoBackgroundChange ? "WLACZONA" : "WYLACZONA") << std::endl;
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        std::cout << "Klawisz F nacisniety" << std::endl;
        std::cout << "--AUTODESTRUKCJA--" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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

    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        cameraEnabled = !cameraEnabled;
        std::cout << "Sterowanie kamera: " << (cameraEnabled ? "WLACZONE" : "WYLACZONE") << std::endl;
    }

    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        // Przełącz tryb pełnoekranowy
        static bool isFullscreen = false;
        static int savedWindowPosX, savedWindowPosY;
        static int savedWindowWidth, savedWindowHeight;

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        if (!isFullscreen) {
            // Zapamiętaj aktualną pozycję i rozmiar okna
            glfwGetWindowPos(window, &savedWindowPosX, &savedWindowPosY);
            glfwGetWindowSize(window, &savedWindowWidth, &savedWindowHeight);

            // Przełącz na pełny ekran
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            std::cout << "Pelny ekran wlaczony: " << mode->width << "x" << mode->height << std::endl;
        } else {
            // Przywróć okno
            glfwSetWindowMonitor(window, nullptr, savedWindowPosX, savedWindowPosY, savedWindowWidth, savedWindowHeight, 0);
            std::cout << "Pelny ekran wylaczony, przywrocono rozmiar: " << savedWindowWidth << "x" << savedWindowHeight << std::endl;
        }

        isFullscreen = !isFullscreen;

        // Uaktualnij viewport po zmianie rozmiaru
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        cameraType = static_cast<Camera::CameraType>((cameraType + 1) % 3);
        camera.setType(cameraType);

        std::string typeName;
        switch (cameraType) {
            case Camera::FREE: typeName = "WOLNA"; break;
            case Camera::FPS: typeName = "FPS"; break;
            case Camera::ORBIT:
                typeName = "ORBITALNA";
                camera.setOrbitTarget(glm::vec3(0.0f, 0.0f, 0.0f));
                camera.setOrbitRadius(10.0f);
                break;
        }
        std::cout << "Tryb kamery: " << typeName << std::endl;
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        camera.setPosition(glm::vec3(0.0f, 2.0f, 8.0f));
        camera.setYaw(-90.0f);
        camera.setPitch(0.0f);
        std::cout << "Kamera zresetowana" << std::endl;
    }

    // Przełączanie między trybami renderowania
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        renderMode = (renderMode + 1) % 2;
        std::cout << "Tryb renderowania: " << (renderMode == 0 ? "Wszystkie kształty" : "Tylko zadania z instrukcji") << std::endl;
    }

    // Sterowanie transformacjami obiektów
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->translate(glm::vec3(0.0f, 0.5f, 0.0f));
            std::cout << "Przesunieto szescian w gore" << std::endl;
        }
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->translate(glm::vec3(0.0f, -0.5f, 0.0f));
            std::cout << "Przesunieto szescian w dol" << std::endl;
        }
    }

    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->translate(glm::vec3(-0.5f, 0.0f, 0.0f));
            std::cout << "Przesunieto szescian w lewo" << std::endl;
        }
    }

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->translate(glm::vec3(0.5f, 0.0f, 0.0f));
            std::cout << "Przesunieto szescian w prawo" << std::endl;
        }
    }

    if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->translate(glm::vec3(0.0f, 0.0f, -0.5f));
            std::cout << "Przesunieto szescian do przodu" << std::endl;
        }
    }

    if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->translate(glm::vec3(0.0f, 0.0f, 0.5f));
            std::cout << "Przesunieto szescian do tylu" << std::endl;
        }
    }

    if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->scale(glm::vec3(1.1f));
            std::cout << "Powiększono szescian" << std::endl;
        }
    }

    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->scale(glm::vec3(0.9f));
            std::cout << "Pomniejszono szescian" << std::endl;
        }
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL) {
        if (rotatingCube) {
            rotatingCube->setRotation(glm::vec3(0.0f));
            std::cout << "Zresetowano rotację szescianu" << std::endl;
        }
    }

    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        // Przełącz widoczność litery H
        static bool letterHVisible = true;
        letterHVisible = !letterHVisible;
        if (letterHObject) {
            letterHObject->setScale(letterHVisible ? glm::vec3(1.0f) : glm::vec3(0.0f));
            std::cout << "Litera H: " << (letterHVisible ? "widoczna" : "niewidoczna") << std::endl;
        }
    }

    // Sterowanie oświetleniem - klawisz L do zmiany trybu oświetlenia
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        currentLightMode = (currentLightMode + 1) % 3;
        std::string modeName;
        switch(currentLightMode) {
            case 0:
                modeName = "TYLKO PIERWSZE SWIATLO (punktowe)";
                break;
            case 1:
                modeName = "TYLKO DRUGIE SWIATLO (kierunkowe)";
                break;
            case 2:
                modeName = "WSZYSTKIE SWIATLA";
                break;
        }
        std::cout << "Tryb oswietlenia: " << modeName << std::endl;
    }

    // Zmiana typu pierwszego światła - klawisz O
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        lights[0].type = (lights[0].type + 1) % 3;
        std::string typeName;
        switch(lights[0].type) {
            case 0: typeName = "PUNKTOWE"; break;
            case 1: typeName = "KIERUNKOWE"; break;
            case 2: typeName = "STOZKOWE"; break;
        }
        std::cout << "Pierwsze swiatlo: " << typeName << std::endl;
    }

    // Zmiana typu drugiego światła - klawisz P
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        lights[1].type = (lights[1].type + 1) % 3;
        std::string typeName;
        switch(lights[1].type) {
            case 0: typeName = "PUNKTOWE"; break;
            case 1: typeName = "KIERUNKOWE"; break;
            case 2: typeName = "STOZKOWE"; break;
        }
        std::cout << "Drugie swiatlo: " << typeName << std::endl;
    }
}

// Callback myszy
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouseCall = true;

    if (!cameraEnabled) return;

    if (firstMouseCall) {
        lastX = xpos;
        lastY = ypos;
        firstMouseCall = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Odwrócone, bo współrzędne Y rosną od dołu do góry

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

// Callback scrolla myszy
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!cameraEnabled) return;
    camera.processMouseScroll(yoffset);
}

// Callback przycisku myszy
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "Lewy przycisk myszy nacisniety" << std::endl;

        // Przykład interakcji z obiektem
        if (orbitingSphere) {
            orbitingSphere->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
            std::cout << "Zmieniono kolor kuli na czerwony" << std::endl;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        std::cout << "Prawy przycisk myszy nacisniety" << std::endl;

        if (orbitingSphere) {
            orbitingSphere->setColor(glm::vec3(0.2f, 0.8f, 0.2f));
            std::cout << "Przywrocono kolor kuli" << std::endl;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        cameraEnabled = !cameraEnabled;
        if (cameraEnabled) {
            firstMouse = true;
        }
        std::cout << "Środkowy przycisk myszy nacisniety" << std::endl;
        std::cout << "Sterowanie kamera: " << (cameraEnabled ? "WLACZONE" : "WYLACZONE") << std::endl;
    }
}

// Callback zmiany rozmiaru okna
void resizeCallback(GLFWwindow* window, int width, int height) {
    std::cout << "Rozmiar okna zmieniony na: " << width << "x" << height << std::endl;
    glViewport(0, 0, width, height);
}

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

// Inicjalizacja świateł
void initializeLights() {
    // Pierwsze światło - punktowe (jak w oryginalnym kodzie)
    lights[0].position = glm::vec3(5.0f, 5.0f, 5.0f);
    lights[0].direction = glm::vec3(0.0f, -1.0f, 0.0f);
    lights[0].color = glm::vec3(1.0f, 1.0f, 1.0f);
    lights[0].ambientIntensity = 0.1f;
    lights[0].diffuseIntensity = 0.8f;
    lights[0].specularIntensity = 0.5f;
    lights[0].constant = 1.0f;
    lights[0].linear = 0.09f;
    lights[0].quadratic = 0.032f;
    lights[0].cutoff = glm::cos(glm::radians(12.5f));
    lights[0].outerCutoff = glm::cos(glm::radians(17.5f));
    lights[0].type = 0; // punktowe

    // Drugie światło - kierunkowe
    lights[1].position = glm::vec3(0.0f, 10.0f, 0.0f);
    lights[1].direction = glm::vec3(0.0f, -1.0f, 0.0f);
    lights[1].color = glm::vec3(0.8f, 0.8f, 1.0f); // niebieskawa barwa
    lights[1].ambientIntensity = 0.2f;
    lights[1].diffuseIntensity = 0.6f;
    lights[1].specularIntensity = 0.3f;
    lights[1].constant = 1.0f;
    lights[1].linear = 0.09f;
    lights[1].quadratic = 0.032f;
    lights[1].cutoff = glm::cos(glm::radians(12.5f));
    lights[1].outerCutoff = glm::cos(glm::radians(17.5f));
    lights[1].type = 1; // kierunkowe
}

// Funkcja aktualizacji
void update(Engine& engine) {

    static float timeAccumulator = 0.0f;
    timeAccumulator += engine.getDeltaTime();

    if (autoBackgroundChange) {
        float time = glfwGetTime();
        float r = 0.5f + 0.5f * sin(time);
        float g = 0.5f + 0.5f * sin(time + 2.0f);
        float b = 0.5f + 0.5f * sin(time + 4.0f);
        currentBackgroundColor = glm::vec4(r, g, b, 1.0f);
        engine.setClearColor(r, g, b, 1.0f);
    } else {
        // Używamy koloru ustawionego ręcznie
        engine.setClearColor(currentBackgroundColor.r, currentBackgroundColor.g,
                           currentBackgroundColor.b, currentBackgroundColor.a);
    }

    // Aktualizacja kątów obrotu
    rotationAngle += 0.5f;
    if (rotationAngle > 360.0f) rotationAngle -= 360.0f;

    cubeRotation += 1.0f;
    if (cubeRotation > 360.0f) cubeRotation -= 360.0f;

    // Oświetlenie krąży wokół sceny (pierwsze światło)
    float lightX = sin(glfwGetTime()) * 5.0f;
    float lightZ = cos(glfwGetTime()) * 5.0f;
    lights[0].position = glm::vec3(lightX, 5.0f, lightZ);

    // Drugie światło (kierunkowe) porusza się w pionie
    lights[1].position.y = 8.0f + sin(glfwGetTime() * 0.5f) * 2.0f;

    // Obsługa klawiatury dla kamery
    if (cameraEnabled) {
        GLFWwindow* window = engine.getWindow();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.processKeyboard(Camera::FORWARD, engine.getDeltaTime());
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.processKeyboard(Camera::BACKWARD, engine.getDeltaTime());
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.processKeyboard(Camera::LEFT, engine.getDeltaTime());
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.processKeyboard(Camera::RIGHT, engine.getDeltaTime());
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.processKeyboard(Camera::DOWN, engine.getDeltaTime());
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.processKeyboard(Camera::UP, engine.getDeltaTime());

        // Zmiana prędkości kamery
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.setMovementSpeed(5.0f);
        else
            camera.setMovementSpeed(2.5f);
    }

    // Animacja obracającego się sześcianu
    if (rotatingCube) {
        rotatingCube->setRotation(glm::vec3(cubeRotation, cubeRotation * 0.7f, 0.0f));
    }

    // Animacja orbitującej kuli
    if (orbitingSphere) {
        float time = glfwGetTime();
        float orbitRadius = 3.0f;
        glm::vec3 orbitPosition(
            sin(time) * orbitRadius,
            1.0f + cos(time * 0.7f) * 0.5f,
            cos(time) * orbitRadius
        );
        orbitingSphere->setPosition(orbitPosition);
    }

    // Animacja litery H
    if (letterHObject) {
        letterHObject->setRotation(glm::vec3(0.0f, rotationAngle * 0.5f, 0.0f));
    }

    // Animacja obiektu z hierarchią
    if (parentCube) {
        float time = glfwGetTime();
        parentCube->setRotation(glm::vec3(0.0f, rotationAngle * 0.3f, 0.0f));
        parentCube->translate(glm::vec3(sin(time * 0.5f) * 0.1f, 0.0f, 0.0f));
    }
}


void render() {
    if (!geometryRenderer) return;

    // Ustaw macierze
    float aspectRatio = 800.0f / 600.0f;
    projection = glm::perspective(glm::radians(camera.getZoom()), aspectRatio, 0.1f, 100.0f);

    // Użyj widoku z kamery
    view = camera.getViewMatrix();
    viewPos = camera.getPosition();

    geometryRenderer->setProjectionMatrix(projection);
    geometryRenderer->setViewMatrix(view);

    // Uzywaj shadera
    glUseProgram(shaderProgram);

    // Pobierz lokalizacje uniformow
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    GLint useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");
    GLint texture1Loc = glGetUniformLocation(shaderProgram, "texture1");
    GLint activeLightCountLoc = glGetUniformLocation(shaderProgram, "activeLightCount");
    GLint currentLightModeLoc = glGetUniformLocation(shaderProgram, "currentLightMode");

    // Ustaw uniformy wspolne dla wszystkich obiektow
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));
    glUniform1i(activeLightCountLoc, activeLightCount);
    glUniform1i(currentLightModeLoc, currentLightMode);

    // Ustawienia tekstury dla teksturowanego sześcianu
    glUniform1i(useTextureLoc, useTextures ? 1 : 0);
    glUniform1i(texture1Loc, 0); // Jednostka teksturująca 0

    // Ustaw uniformy dla świateł
    for (int i = 0; i < activeLightCount; i++) {
        std::string lightStr = "lights[" + std::to_string(i) + "].";

        glUniform3fv(glGetUniformLocation(shaderProgram, (lightStr + "position").c_str()), 1, glm::value_ptr(lights[i].position));
        glUniform3fv(glGetUniformLocation(shaderProgram, (lightStr + "direction").c_str()), 1, glm::value_ptr(lights[i].direction));
        glUniform3fv(glGetUniformLocation(shaderProgram, (lightStr + "color").c_str()), 1, glm::value_ptr(lights[i].color));
        glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + "ambientIntensity").c_str()), lights[i].ambientIntensity);
        glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + "diffuseIntensity").c_str()), lights[i].diffuseIntensity);
        glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + "specularIntensity").c_str()), lights[i].specularIntensity);
        glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + "constant").c_str()), lights[i].constant);
        glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + "linear").c_str()), lights[i].linear);
        glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + "quadratic").c_str()), lights[i].quadratic);
        glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + "cutoff").c_str()), lights[i].cutoff);
        glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + "outerCutoff").c_str()), lights[i].outerCutoff);
        glUniform1i(glGetUniformLocation(shaderProgram, (lightStr + "type").c_str()), lights[i].type);
    }

    // Rysowanie teksturowanego sześcianu
    model = texturedCube.getModelMatrix();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

    if (useTextures) {
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        texturedCube.drawWithTexture();
    } else {
        texturedCube.draw();
    }

    //Rysowanie teksturowanej kuli
    model = texturedSphere.getModelMatrix();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

    if (useTextures) {
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        texturedSphere.drawWithTexture();
    } else {
        texturedSphere.draw();
    }

    //Rysowanie teksturowanego cylindra
    model = texturedCylinder.getModelMatrix();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

    if (useTextures) {
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        texturedCylinder.drawWithTexture();
    } else {
        texturedCylinder.draw();
    }

    // Dla pozostałych obiektów wyłącz tekstury i używaj kolorów
    glUniform1i(useTextureLoc, 0);
    glDisable(GL_TEXTURE_2D);

    // Przełączanie między trybami renderowania
    if (renderMode == 0) {
        // Tryb domyślny: wszystkie kształty używając nowego systemu

        // Renderowanie wszystkich obiektów w scenie
        if (sceneManager) {
            // Musimy ustawić macierz modelu dla każdego obiektu
            // W klasach TransformableObject to robią wewnętrznie,
            // ale tu musimy przekazać ją do shadera
            for (size_t i = 0; i < sceneManager->getObjectCount(); ++i) {
                TransformableObject* obj = sceneManager->getObject(i);
                if (obj) {
                    glm::mat4 modelMatrix = obj->getModelMatrix();
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

                    // Ustaw kolor obiektu
                    glm::vec3 color = obj->getColor();
                    glUniform3f(objectColorLoc, color.r, color.g, color.b);

                    // Rysuj obiekt
                    obj->draw();
                }
            }
        }

        // Rysowanie podłogi (płaszczyzny) starym systemem dla zachowania kompatybilności
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(objectColorLoc, 0.3f, 0.3f, 0.3f); // Szary
        geometryRenderer->drawPlane(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec2(20.0f, 20.0f));

        // Rysowanie siatki
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(objectColorLoc, 0.5f, 0.5f, 0.5f); // Szary
        geometryRenderer->setDrawMode(GL_LINES);
        geometryRenderer->drawGrid(glm::vec3(0.0f, -2.0f, 0.0f), 20, 1.0f);
        geometryRenderer->setDrawMode(GL_TRIANGLES);

    } else {
        // Tryb zadań z instrukcji (stary system)
        // Tu można dodać kod dla trybu zadań z instrukcji
    }

    // Rysowanie linii (układ współrzędnych)
    geometryRenderer->setDrawMode(GL_LINES);

    // Oś X - czerwona
    glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
    geometryRenderer->drawLine(glm::vec3(0.0f), glm::vec3(3.0f, 0.0f, 0.0f));

    // Oś Y - zielona
    glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);
    geometryRenderer->drawLine(glm::vec3(0.0f), glm::vec3(0.0f, 3.0f, 0.0f));

    // Oś Z - niebieska
    glUniform3f(objectColorLoc, 0.0f, 0.0f, 1.0f);
    geometryRenderer->drawLine(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 3.0f));

    // Rysowanie punktów (źródła światła)
    geometryRenderer->setDrawMode(GL_POINTS);

    // Pierwsze światło (białe)
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
    geometryRenderer->drawPoint(lights[0].position, 10.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    // Drugie światło (niebieskawe)
    glUniform3f(objectColorLoc, 0.8f, 0.8f, 1.0f);
    geometryRenderer->drawPoint(lights[1].position, 10.0f, glm::vec3(0.8f, 0.8f, 1.0f));

    // Rysowanie pozycji kamery (opcjonalnie, dla debugowania)
    geometryRenderer->setDrawMode(GL_POINTS);
    glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f); // Cyjan
    geometryRenderer->drawPoint(camera.getPosition(), 5.0f, glm::vec3(0.0f, 1.0f, 1.0f));

    // Przywróć tryb rysowania
    geometryRenderer->setDrawMode(GL_TRIANGLES);
}

int main() {
    std::cout << "=== SILNIK 3D ===" << std::endl;
    std::cout << "\nInicjalizacja..." << std::endl;

    // Utworzenie silnika
    Engine engine(800, 600, 60, false);
    globalEngine = &engine;
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

    // Utworzenie menadżera sceny
    sceneManager = new SceneManager(geometryRenderer);

    // Inicjalizacja świateł
    initializeLights();

    // Tworzenie obiektów 3D z transformacjami
    rotatingCube = sceneManager->createCube("RotatingCube",
                                            glm::vec3(-2.0f, 1.0f, 0.0f),
                                            glm::vec3(0.0f),
                                            glm::vec3(1.0f),
                                            glm::vec3(1.0f, 0.5f, 0.2f));

    orbitingSphere = sceneManager->createSphere("OrbitingSphere",
                                                glm::vec3(3.0f, 1.0f, 0.0f),
                                                0.8f,
                                                glm::vec3(0.2f, 0.8f, 0.2f));

    letterHObject = sceneManager->createLetterH("LetterH",
                                                glm::vec3(0.0f, 1.5f, 0.0f),
                                                2.0f, 3.0f, 0.5f,
                                                glm::vec3(0.9f, 0.2f, 0.2f));

    // Tworzenie hierarchii obiektów
    parentCube = sceneManager->createCube("ParentCube",
                                          glm::vec3(-5.0f, 2.0f, 0.0f),
                                          glm::vec3(0.0f),
                                          glm::vec3(1.5f),
                                          glm::vec3(0.8f, 0.2f, 0.8f));

    childSphere1 = sceneManager->createSphere("ChildSphere1",
                                             glm::vec3(1.5f, 0.0f, 0.0f),
                                             0.5f,
                                             glm::vec3(0.8f, 0.8f, 0.2f));

    childSphere2 = sceneManager->createSphere("ChildSphere2",
                                             glm::vec3(-3.0f, 0.0f, 0.0f),
                                             0.5f,
                                             glm::vec3(0.8f, 0.8f, 0.2f));

    // Ustawienie hierarchii (dziecko będzie poruszać się razem z rodzicem)
    childSphere1->setParent(parentCube);
    childSphere2->setParent(childSphere1);

    // Dodanie więcej obiektów do demonstracji
    sceneManager->createCylinder("Cylinder1",
                                 glm::vec3(4.0f, 0.0f, -3.0f),
                                 2.0f, 0.5f,
                                 glm::vec3(0.2f, 0.5f, 1.0f));

    sceneManager->createCube("StaticCube1",
                             glm::vec3(-4.0f, 0.5f, 3.0f),
                             glm::vec3(45.0f, 30.0f, 0.0f),
                             glm::vec3(1.2f, 0.8f, 0.8f),
                             glm::vec3(0.8f, 0.6f, 0.2f));

    // Inicjalizacja teksturowanego sześcianu
    texturedCube.create(1.0f);
    if (!texture.loadTexture("../Texture/harnas.png")) {
        if (!texture.loadTexture("../Texture/Wood_Texture.png")) {
            std::cout << "Nie udalo sie zaladowac tekstury" << std::endl;
        }
    }
    texturedCube.setTexture(std::make_shared<BitmapHandler>(std::move(texture)));
    texturedCube.setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

    texturedSphere.create(0.5f); // Kula o promieniu 1.5
    if (!textureSphere.loadTexture("../Texture/Wood_Texture.png")) {
        if (!textureSphere.loadTexture("../Texture/Wood_Texture.png")) {
            std::cout << "Nie udalo sie zaladowac tekstury" << std::endl;
        }    }
    texturedSphere.setTexture(std::make_shared<BitmapHandler>(std::move(textureSphere)));
    texturedSphere.setPosition(glm::vec3(0.0f, 1.5f, -5.0f));

    texturedCylinder.create(1.0f, 3.0f); // Cylinder: radius=1.0, height=3.0
    texturedCylinder.rotate(glm::vec3(0.0f, 120.0f, 0.0f));
    if (!textureCylinder.loadTexture("../Texture/harnas.png")) {
        if (!textureCylinder.loadTexture("../Texture/Wood_Texture.png")) {
            std::cout << "Nie udalo sie zaladowac tekstury" << std::endl;
        }    }
    texturedCylinder.setTexture(std::make_shared<BitmapHandler>(std::move(textureCylinder)));
    texturedCylinder.setPosition(glm::vec3(-3.0f, 1.5f, 4.0f));

    // Utworz shadery
    createShaderProgram();

    // Ustawienie callbackow
    engine.setKeyCallback(keyCallback);
    engine.setMouseMoveCallback(mouseCallback);
    engine.setMouseButtonCallback(mouseButtonCallback);
    engine.setResizeCallback(resizeCallback);

    // Ustawienie callbacka scrolla myszy
    glfwSetScrollCallback(engine.getWindow(), scrollCallback);

    // Ustawienie kursora
    engine.setCursorMode(GLFW_CURSOR_DISABLED);

    // Ustawienie koloru tla
    engine.setClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Ciemnoszary

    std::cout << "\n=== STEROWANIE ===" << std::endl;
    std::cout << "ESC: Wyjscie" << std::endl;
    std::cout << "F: Autodestrukcja" << std::endl;
    std::cout << "1: Tryb wypelniony (GL_FILL)" << std::endl;
    std::cout << "2: Tryb linie (GL_LINE)" << std::endl;
    std::cout << "3: Tryb punkty (GL_POINT)" << std::endl;
    std::cout << "C: Wlacz/Wylacz sterowanie kamera" << std::endl;
    std::cout << "T: Zmien tryb kamery (WOLNA/FPS/ORBITALNA)" << std::endl;
    std::cout << "R: Resetuj kamere" << std::endl;
    std::cout << "Srodkowy przycisk myszy: Wlacz/Wylacz kamere" << std::endl;
    std::cout << "WASD: Poruszanie kamera" << std::endl;
    std::cout << "Ctrl: Dol" << std::endl;
    std::cout << "Spacja: Gora" << std::endl;
    std::cout << "Shift: Przyspiesz ruch" << std::endl;
    std::cout << "Mysz: Patrzenie" << std::endl;
    std::cout << "Scroll: Zoom" << std::endl;
    std::cout << "\n=== TRANSFORMACJE OBIEKTOW ===" << std::endl;
    std::cout << "Strzalki: Przesuwanie szescianu (gora/dol/lewo/prawo)" << std::endl;
    std::cout << "PageUp/PageDown: Przesuwanie szescianu (przod/tyl)" << std::endl;
    std::cout << "+ (numeryczne): Powiększ szescian" << std::endl;
    std::cout << "- (numeryczne): Pomniejsz szescian" << std::endl;
    std::cout << "Ctrl+R: Resetuj rotację szescianu" << std::endl;
    std::cout << "H: Przelacz widocznosc litery H" << std::endl;
    std::cout << "M: Zmien tryb renderowania" << std::endl;
    std::cout << "B: Zmien kolor tla (5 opcji)" << std::endl;
    std::cout << "V: Wlacz/wylacz automatyczna zmiane tla" << std::endl;
    std::cout << "Lewy przycisk myszy: Zmien kolor kuli na czerwony" << std::endl;
    std::cout << "Prawy przycisk myszy: Przywroc kolor kuli" << std::endl;
    std::cout << "\n=== OŚWIETLENIE ===" << std::endl;
    std::cout << "L: Przelacz tryb oswietlenia (tylko pierwsze/tylko drugie/wszystkie)" << std::endl;
    std::cout << "O: Zmien typ pierwszego swiatla (punktowe/kierunkowe/stozkowe)" << std::endl;
    std::cout << "P: Zmien typ drugiego swiatla (punktowe/kierunkowe/stozkowe)" << std::endl;
    std::cout << "==================" << std::endl;

    std::cout << "\n=== INFORMACJE ===" << std::endl;
    std::cout << "Pozycja kamery: (" << camera.getPosition().x << ", "
              << camera.getPosition().y << ", " << camera.getPosition().z << ")" << std::endl;
    std::cout << "Tryb kamery: FPS" << std::endl;
    std::cout << "Sterowanie kamera: WLACZONE" << std::endl;
    std::cout << "Liczba obiektow w scenie: " << sceneManager->getObjectCount() << std::endl;
    std::cout << "Liczba zrodel swiatla: " << activeLightCount << std::endl;
    std::cout << "Tryb oswietlenia: WSZYSTKIE SWIATLA" << std::endl;
    std::cout << "Typ pierwszego swiatla: PUNKTOWE" << std::endl;
    std::cout << "Typ drugiego swiatla: KIERUNKOWE" << std::endl;
    std::cout << "==================" << std::endl;

    // Lambda dla aktualizacji z referencją do silnika
    auto updateWrapper = [&engine]() {
        update(engine);
    };

    // Lambda dla renderowania
    auto renderWrapper = []() {
        render();
    };

    // Uruchomienie glownej petli
    engine.run(updateWrapper, renderWrapper);

    // Sprzątanie
    delete sceneManager;
    sceneManager = nullptr;

    geometryRenderer = nullptr;

    std::cout << "\nProgram zakonczony" << std::endl;

    return 0;
}