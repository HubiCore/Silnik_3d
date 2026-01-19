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

/**
@mainpage Silnik 3D - Dokumentacja

@tableofcontents

@section sec_overview Przegląd projektu

**Silnik 3D** to aplikacja grafiki komputerowej napisana w C++ z wykorzystaniem bibliotek OpenGL i GLM.
Projekt demonstruje zaawansowane techniki renderowania 3D, w tym:

- Obsługa klawiatury i myszy
- Zmienna szybkość odświeżania
- Rysowanie prymitywów (3D)
- Obsługa kamery
- Hierarchia klas
- Obsługa transformacji geometrycznych na prymitywach
- Oświetlenie (można dodać obsługę przycisku, który wyłącza i włącza oświetlenie)
- Cieniowanie (można dodać obsługę przycisku, który wyłącza i włącza cieniowanie)
- Teksturowanie obiektów
- Demo technologiczne (do obrony)
- Sprawozdanie i dokumentacja

@section sec_features Funkcjonalności
- **Renderowanie 3D**: Zaawansowane techniki renderowania z użyciem OpenGL
- **System kamery**: Swobodne poruszanie się po scenie 3D
- **Oświetlenie**: Implementacja różnych modeli oświetlenia
- **Teksturowanie**: Wczytywanie i nakładanie tekstur na obiekty
- **Interakcja**: Pełna obsługa klawiatury i myszy
- **Transformacje**: Skalowanie, obracanie i przesuwanie obiektów

@section sec_requirements Wymagania systemowe
- System operacyjny: Windows 10/11, Linux
- Kompilator wspierający C++17
- Karta graficzna z obsługą OpenGL 4.6
- Biblioteki: GLFW, GLEW, GLM, STB

@section sec_usage Szybki start
1. Sklonuj repozytorium projektu
2. Zainstaluj wymagane zależności
3. Wygeneruj pliki build przy użyciu CMake
4. Skompiluj projekt
5. Uruchom plik wykonywalny

@section sec_links Przydatne linki
- [Repozytorium GitHub](https://github.com/przyklad/silnik3d)
- [Dokumentacja OpenGL](https://www.khronos.org/opengl/)
- [Dokumentacja GLM](https://glm.g-truc.net/)

@author Andrzej Raumiagi, Hubert Stojek, Hubert Wilczyński
@date 20.01.2026
@version 1.0.0
*/
/**
 * @brief Vertex shader source for flat shading
 *
 * Shader przetwarzający wierzchołki z atrybutami pozycji, normalnych i koordynatów tekstury.
 * Wykorzystuje kwalifikator 'flat' dla normalnych, co daje efekt płaskiego cieniowania.
 */
const char* vertexShaderSourceFlat = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

flat out vec3 Normal;  // Kwalifikator 'flat' dla płaskiego cieniowania
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

/**
 * @brief Fragment shader source for flat shading
 *
 * Shader implementujący model oświetlenia Phonga z obsługą wielu świateł.
 * Wykorzystuje płaskie cieniowanie dzięki kwalifikatorowi 'flat' dla normalnych.
 */
const char* fragmentShaderSourceFlat = R"(
#version 330 core
out vec4 FragColor;

flat in vec3 Normal;  // Płaskie interpolowane normalne
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

/**
 * @brief Vertex shader source for Phong shading
 *
 * Standardowy shader wierzchołków dla modelu Phonga z interpolowanymi normalnymi.
 */
const char* vertexShaderSourcePhong = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;  // Normalne interpolowane przez rasterizer
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

/**
 * @brief Fragment shader source for Phong shading
 *
 * Shader implementujący model oświetlenia Phonga z gładko interpolowanymi normalnymi.
 */
const char* fragmentShaderSourcePhong = R"(
#version 330 core
out vec4 FragColor;

in vec3 Normal;  // Gładko interpolowane normalne
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

static Engine* globalEngine = nullptr; ///< Wskaźnik do globalnej instancji silnika

bool autoBackgroundChange = false; ///< Flaga automatycznej zmiany tła
glm::vec4 currentBackgroundColor(0.1f, 0.1f, 0.1f, 1.0f); ///< Aktualny kolor tła

// Obiekty teksturowane
TexturedCube texturedCube;    ///< Teksturowany sześcian
TexturedSphere texturedSphere; ///< Teksturowana kula
TexturedCylinder texturedCylinder; ///< Teksturowany cylinder

BitmapHandler textureSphere;  ///< Handler tekstury dla kuli
BitmapHandler textureCylinder; ///< Handler tekstury dla cylindra
BitmapHandler texture;        ///< Handler tekstury dla sześcianu
bool useTextures = true;      ///< Flaga użycia tekstur

// Shadery i tryby cieniowania
GLuint shaderProgramFlat;     ///< Program shaderowy dla cieniowania płaskiego
GLuint shaderProgramPhong;    ///< Program shaderowy dla cieniowania Phonga
GLuint currentShaderProgram;  ///< Aktualnie używany program shaderowy
bool flatShading = false;     ///< Flaga trybu cieniowania (false = PHONG, true = FLAT)

// Kamera
Camera camera(glm::vec3(0.0f, 2.0f, 13.0f)); ///< Główna kamera sceny
bool firstMouse = true;       ///< Flaga pierwszego ruchu myszy
float lastX = 400, lastY = 300; ///< Poprzednie pozycje myszy

// Kontrola kamery
bool cameraEnabled = true;    ///< Flaga włączenia sterowania kamerą
Camera::CameraType cameraType = Camera::FPS; ///< Typ kamery

// Zmienne globalne dla animacji
float rotationAngle = 0.0f;   ///< Kąt rotacji dla animacji
float cubeRotation = 0.0f;    ///< Rotacja sześcianu
GeometryRenderer* geometryRenderer = nullptr; ///< Wskaźnik do renderera geometrii
int renderMode = 0; ///< Tryb renderowania (0 = wszystkie kształty, 1 = tylko zadania z instrukcji)

// Macierze transformacji
glm::mat4 projection; ///< Macierz projekcji
glm::mat4 view;       ///< Macierz widoku
glm::mat4 model;      ///< Macierz modelu

// Struktura reprezentująca światło
struct Light {
    glm::vec3 position;           ///< Pozycja światła
    glm::vec3 direction;          ///< Kierunek światła (dla kierunkowego i stożkowego)
    glm::vec3 color;              ///< Kolor światła
    float ambientIntensity;       ///< Intensywność składowej ambient
    float diffuseIntensity;       ///< Intensywność składowej diffuse
    float specularIntensity;      ///< Intensywność składowej specular
    float constant;               ///< Stały współczynnik tłumienia
    float linear;                 ///< Liniowy współczynnik tłumienia
    float quadratic;              ///< Kwadratowy współczynnik tłumienia
    float cutoff;                 ///< Kąt wewnętrzny stożka światła
    float outerCutoff;            ///< Kąt zewnętrzny stożka światła
    int type;                     ///< Typ światła (0 = punktowe, 1 = kierunkowe, 2 = stożkowe)
};

Light lights[8];                 ///< Tablica świateł (maksymalnie 8)
int activeLightCount = 2;        ///< Liczba aktywnych świateł
int currentLightMode = 2;        ///< Tryb oświetlenia (0 = tylko pierwsze, 1 = tylko drugie, 2 = wszystkie)
glm::vec3 viewPos(0.0f, 3.0f, 8.0f); ///< Pozycja obserwatora (kamera)

// Obiekty transformowalne
SceneManager* sceneManager = nullptr; ///< Menadżer sceny
TransformableObject* rotatingCube = nullptr; ///< Obracający się sześcian
TransformableObject* rotatecylinder = nullptr; ///< Obracający się cylinder
TransformableObject* orbitingSphere = nullptr; ///< Orbitująca kula
ComplexObjectWithTransform* letterHObject = nullptr; ///< Litera H jako złożony obiekt
TransformableObject* wagonik1 = nullptr; ///< Pierwszy wagonik (rodzic)
TransformableObject* wagonik2 = nullptr; ///< Drugi wagonik (dziecko)
TransformableObject* wagonik3 = nullptr; ///< Trzeci wagonik (dziecko)
TransformableObject* wagonik4 = nullptr; ///< Czwarty wagonik (dziecko)

/**
 * @brief Callback klawiatury
 *
 * Obsługuje wszystkie zdarzenia klawiatury w aplikacji.
 *
 * @param window Okno GLFW
 * @param key Kod klawisza
 * @param scancode Kod skanowania klawisza
 * @param action Akcja (naciśnięcie, zwolnienie)
 * @param mods Modyfikatory (Ctrl, Shift, Alt)
 */
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
                currentBackgroundColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
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

    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        flatShading = !flatShading;
        currentShaderProgram = flatShading ? shaderProgramFlat : shaderProgramPhong;
        if (flatShading == true) {
            std::cout<<"Tryb cienowania flat\n";
        }
        else {
            std::cout<<"Tryb cienowania phong\n";
        }
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
        camera.setPosition(glm::vec3(0.0f, 2.0f, 13.0f));
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
            rotatingCube->translate(glm::vec3(0.0f, 0.0f, -0.5f));
            std::cout << "Przesunieto szescian do przodu" << std::endl;
        }
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->translate(glm::vec3(0.0f, 0.0f, 0.5f));
            std::cout << "Przesunieto szescian do tylu" << std::endl;
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

    if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS || key == GLFW_KEY_KP_9 && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->translate(glm::vec3(0.0f, 0.5f, 0.0f));
            std::cout << "Przesunieto szescian w gore" << std::endl;
        }
    }

    if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS  || key == GLFW_KEY_KP_3 && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->translate(glm::vec3(0.0f, -0.5f, 0.0f));
            std::cout << "Przesunieto szescian w dol" << std::endl;
        }
    }

    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->scale(glm::vec3(1.1f));
            std::cout << "Powiekszono szescian" << std::endl;
        }
    }

    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
        if (rotatingCube) {
            rotatingCube->scale(glm::vec3(0.9f));
            std::cout << "Pomniejszono szescian" << std::endl;
        }
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL) {
        // Ctrl+R - można dodać funkcjonalność resetu
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

/**
 * @brief Callback ruchu myszy
 *
 * Obsługuje zdarzenia ruchu myszy dla sterowania kamerą.
 *
 * @param window Okno GLFW
 * @param xpos Pozycja X kursora
 * @param ypos Pozycja Y kursora
 */
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

/**
 * @brief Callback scrolla myszy
 *
 * Obsługuje zdarzenia scrolla myszy dla zmiany zoomu kamery.
 *
 * @param window Okno GLFW
 * @param xoffset Przesunięcie X scrolla
 * @param yoffset Przesunięcie Y scrolla
 */
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!cameraEnabled) return;
    camera.processMouseScroll(yoffset);
}

/**
 * @brief Callback przycisków myszy
 *
 * Obsługuje zdarzenia przycisków myszy.
 *
 * @param window Okno GLFW
 * @param button Kod przycisku myszy
 * @param action Akcja (naciśnięcie, zwolnienie)
 * @param mods Modyfikatory
 */
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

/**
 * @brief Callback zmiany rozmiaru okna
 *
 * Obsługuje zdarzenia zmiany rozmiaru okna aplikacji.
 *
 * @param window Okno GLFW
 * @param width Nowa szerokość okna
 * @param height Nowa wysokość okna
 */
void resizeCallback(GLFWwindow* window, int width, int height) {
    std::cout << "Rozmiar okna zmieniony na: " << width << "x" << height << std::endl;
    glViewport(0, 0, width, height);
}

/**
 * @brief Kompiluje shader OpenGL
 *
 * @param type Typ shadera (GL_VERTEX_SHADER lub GL_FRAGMENT_SHADER)
 * @param source Kod źródłowy shadera
 * @return GLuint ID skompilowanego shadera
 */
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

/**
 * @brief Tworzy i linkuje programy shaderowe
 *
 * Tworzy dwa programy shaderowe: dla trybu FLAT i PHONG.
 */
void createShaderProgram() {
    // Kompilacja shaderów dla trybu FLAT
    GLuint vertexShaderFlat = compileShader(GL_VERTEX_SHADER, vertexShaderSourceFlat);
    GLuint fragmentShaderFlat = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSourceFlat);

    shaderProgramFlat = glCreateProgram();
    glAttachShader(shaderProgramFlat, vertexShaderFlat);
    glAttachShader(shaderProgramFlat, fragmentShaderFlat);
    glLinkProgram(shaderProgramFlat);

    // Kompilacja shaderów dla trybu PHONG
    GLuint vertexShaderPhong = compileShader(GL_VERTEX_SHADER, vertexShaderSourcePhong);
    GLuint fragmentShaderPhong = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSourcePhong);

    shaderProgramPhong = glCreateProgram();
    glAttachShader(shaderProgramPhong, vertexShaderPhong);
    glAttachShader(shaderProgramPhong, fragmentShaderPhong);
    glLinkProgram(shaderProgramPhong);

    // Ustaw domyślny program na PHONG
    currentShaderProgram = shaderProgramPhong;
    flatShading = false;

    // Usuń shadery (już niepotrzebne po linkowaniu)
    glDeleteShader(vertexShaderFlat);
    glDeleteShader(fragmentShaderFlat);
    glDeleteShader(vertexShaderPhong);
    glDeleteShader(fragmentShaderPhong);
}

/**
 * @brief Inicjalizuje światła w scenie
 *
 * Konfiguruje dwa źródła światła z różnymi parametrami.
 */
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
    lights[1].color = glm::vec3(1.0f, 0.0f, 0.0f); // niebieskawa barwa (teraz czerwona)
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

/**
 * @brief Funkcja aktualizacji logiki aplikacji
 *
 * Wywoływana co klatkę, aktualizuje stan wszystkich obiektów w scenie.
 *
 * @param engine Referencja do silnika gry
 */
void update(Engine& engine) {
    static float timeAccumulator = 0.0f;
    timeAccumulator += engine.getDeltaTime();

    // Automatyczna zmiana koloru tła
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

    // Animacja obracającego się krążka
    if (rotatecylinder) {
        rotatecylinder->setRotation(glm::vec3(cubeRotation, cubeRotation * 0.7f, 0.0f));
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
    if (wagonik1) {
        float time = glfwGetTime();
        wagonik1->setRotation(glm::vec3(rotationAngle * 1.5f, 0.0f, 0.0f));
        wagonik1->translate(glm::vec3(sin(time * 0.8f) * 0.2f, 0.0f, 0.0f));
    }
}

/**
 * @brief Funkcja renderowania sceny
 *
 * Wywoływana co klatkę, renderuje wszystkie obiekty w scenie.
 */
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
    glUseProgram(currentShaderProgram);

    // Pobierz lokalizacje uniformow z AKTUALNEGO programu shaderowego
    GLint modelLoc = glGetUniformLocation(currentShaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(currentShaderProgram, "view");
    GLint projLoc = glGetUniformLocation(currentShaderProgram, "projection");
    GLint objectColorLoc = glGetUniformLocation(currentShaderProgram, "objectColor");
    GLint viewPosLoc = glGetUniformLocation(currentShaderProgram, "viewPos");
    GLint useTextureLoc = glGetUniformLocation(currentShaderProgram, "useTexture");
    GLint texture1Loc = glGetUniformLocation(currentShaderProgram, "texture1");
    GLint activeLightCountLoc = glGetUniformLocation(currentShaderProgram, "activeLightCount");
    GLint currentLightModeLoc = glGetUniformLocation(currentShaderProgram, "currentLightMode");

    // Ustaw uniformy wspolne dla wszystkich obiektow
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));
    glUniform1i(activeLightCountLoc, activeLightCount);
    glUniform1i(currentLightModeLoc, currentLightMode);

    // Ustawienia tekstury dla teksturowanego sześcianu
    glUniform1i(useTextureLoc, useTextures ? 1 : 0);
    glUniform1i(texture1Loc, 0); // Jednostka teksturująca 0

    // Ustaw uniformy dla świateł - UWAGA: też z currentShaderProgram!
    for (int i = 0; i < activeLightCount; i++) {
        std::string lightStr = "lights[" + std::to_string(i) + "].";

        glUniform3fv(glGetUniformLocation(currentShaderProgram, (lightStr + "position").c_str()), 1, glm::value_ptr(lights[i].position));
        glUniform3fv(glGetUniformLocation(currentShaderProgram, (lightStr + "direction").c_str()), 1, glm::value_ptr(lights[i].direction));
        glUniform3fv(glGetUniformLocation(currentShaderProgram, (lightStr + "color").c_str()), 1, glm::value_ptr(lights[i].color));
        glUniform1f(glGetUniformLocation(currentShaderProgram, (lightStr + "ambientIntensity").c_str()), lights[i].ambientIntensity);
        glUniform1f(glGetUniformLocation(currentShaderProgram, (lightStr + "diffuseIntensity").c_str()), lights[i].diffuseIntensity);
        glUniform1f(glGetUniformLocation(currentShaderProgram, (lightStr + "specularIntensity").c_str()), lights[i].specularIntensity);
        glUniform1f(glGetUniformLocation(currentShaderProgram, (lightStr + "constant").c_str()), lights[i].constant);
        glUniform1f(glGetUniformLocation(currentShaderProgram, (lightStr + "linear").c_str()), lights[i].linear);
        glUniform1f(glGetUniformLocation(currentShaderProgram, (lightStr + "quadratic").c_str()), lights[i].quadratic);
        glUniform1f(glGetUniformLocation(currentShaderProgram, (lightStr + "cutoff").c_str()), lights[i].cutoff);
        glUniform1f(glGetUniformLocation(currentShaderProgram, (lightStr + "outerCutoff").c_str()), lights[i].outerCutoff);
        glUniform1i(glGetUniformLocation(currentShaderProgram, (lightStr + "type").c_str()), lights[i].type);
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

    // Rysowanie teksturowanej kuli
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

    // Rysowanie teksturowanego cylindra
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

/**
 * @brief Główna funkcja programu
 *
 * Inicjalizuje aplikację, konfiguruje scenę i uruchamia główną pętlę gry.
 *
 * @return int Kod wyjścia programu
 */
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
    wagonik1 = sceneManager->createCube("wagonik1",
                                          glm::vec3(0.0f, 2.0f, 0.0f),
                                          glm::vec3(0.0f),
                                          glm::vec3(1.0f),
                                          glm::vec3(0.8f, 0.2f, 0.8f));

    wagonik2 = sceneManager->createCube("wagonik2",
                                          glm::vec3(1.2f, 0.0f, 0.0f),
                                          glm::vec3(0.0f),
                                          glm::vec3(1.0f),
                                          glm::vec3(0.8f, 0.2f, 0.8f));

    wagonik3 = sceneManager->createCube("wagonik3",
                                          glm::vec3(2.4f, 0.0f, 0.0f),
                                          glm::vec3(0.0f),
                                          glm::vec3(1.0f),
                                          glm::vec3(0.8f, 0.2f, 0.8f));

    wagonik4 = sceneManager->createCube("wagonik4",
                                          glm::vec3(3.6f, 0.0f, 0.0f),
                                          glm::vec3(0.0f),
                                          glm::vec3(1.0f),
                                          glm::vec3(0.8f, 0.2f, 0.8f));

    // Ustawienie hierarchii (dziecko będzie poruszać się razem z rodzicem)
    wagonik2->setParent(wagonik1);
    wagonik3->setParent(wagonik1);
    wagonik4->setParent(wagonik1);

    // Dodanie więcej obiektów do demonstracji
    rotatecylinder = sceneManager->createCylinder("Cylinder1",
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
    if (!texture.loadTexture("../Texture/Texture4.png")) {
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

    texturedCylinder.create(0.5f, 2.0f); // Cylinder: radius=1.0, height=3.0
    texturedCylinder.rotate(glm::vec3(0.0f, 140.0f, 0.0f));
    if (!textureCylinder.loadTexture("../Texture/harnas.png")) {
        if (!textureCylinder.loadTexture("../Texture/Wood_Texture.png")) {
            std::cout << "Nie udalo sie zaladowac tekstury" << std::endl;
        }    }
    texturedCylinder.setTexture(std::make_shared<BitmapHandler>(std::move(textureCylinder)));
    texturedCylinder.setPosition(glm::vec3(-3.0f, 1.5f, 6.0f));

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
    std::cout << "Strzalki: Przesuwanie szescianu (przod/tyl/lewo/prawo)" << std::endl;
    std::cout << "PageUp/PageDown: Przesuwanie szescianu (gora/dol)" << std::endl;
    std::cout << "+ (numeryczne): Powieksz szescian" << std::endl;
    std::cout << "- (numeryczne): Pomniejsz szescian" << std::endl;
    std::cout << "H: Przelacz widocznosc litery H" << std::endl;
    std::cout << "M: Zmien tryb renderowania" << std::endl;
    std::cout << "B: Zmien kolor tla (5 opcji)" << std::endl;
    std::cout << "V: Wlacz/wylacz automatyczna zmiane tla" << std::endl;
    std::cout << "Lewy przycisk myszy: Zmien kolor kuli na czerwony" << std::endl;
    std::cout << "Prawy przycisk myszy: Przywroc kolor kuli" << std::endl;
    std::cout << "\n=== OSWIETLENIE ===" << std::endl;
    std::cout << "G: Zmien tryb cieniowania (PHONG/FLAT)"<<std::endl;
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