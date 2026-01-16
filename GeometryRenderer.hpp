#ifndef GEOMETRY_RENDERER_HPP
#define GEOMETRY_RENDERER_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

/**
 * @struct Vertex
 * @brief Struktura reprezentująca wierzchołek 3D
 *
 * Zawiera pozycję, normalną i współrzędne tekstury
 */
struct Vertex {
    glm::vec3 position;    /**< Pozycja wierzchołka w przestrzeni 3D */
    glm::vec3 normal;      /**< Wektor normalny wierzchołka */
    glm::vec2 texCoord;    /**< Współrzędne tekstury (UV) */
};

/**
 * @struct Material
 * @brief Struktura reprezentująca właściwości materiału
 *
 * Zawiera składowe światła dla modelu oświetlenia Phonga
 */
struct Material {
    glm::vec3 ambient;     /**< Składowa otoczenia (ambient) */
    glm::vec3 diffuse;     /**< Składowa rozproszenia (diffuse) */
    glm::vec3 specular;    /**< Składowa odbicia (specular) */
    float shininess;       /**< Współczynnik połysku (shininess) */
};

/**
 * @struct Mesh
 * @brief Struktura reprezentująca siatkę 3D w OpenGL
 *
 * Zawiera identyfikatory buforów VAO, VBO, EBO oraz liczbę indeksów
 */
struct Mesh {
    unsigned int VAO;      /**< Vertex Array Object */
    unsigned int VBO;      /**< Vertex Buffer Object */
    unsigned int EBO;      /**< Element Buffer Object (indices) */
    int indexCount;        /**< Liczba indeksów w siatce */
};

/**
 * @class GeometryRenderer
 * @brief Klasa renderująca geometryczne kształty 3D
 *
 * Udostępnia funkcje do rysowania podstawowych kształtów 3D (sześcian, sfera,
 * cylinder itp.) oraz kształtów pomocniczych (linie, siatki, strzałki).
 * Wykorzystuje buforowanie OpenGL do efektywnego renderowania.
 */
class GeometryRenderer {
private:
    GLenum m_drawMode;             /**< Aktualny tryb rysowania OpenGL (GL_TRIANGLES, GL_LINES itp.) */
    Material m_currentMaterial;    /**< Aktualne właściwości materiału */

    // Buforowane kształty geometryczne
    Mesh m_cubeMesh;               /**< Siatka sześcianu */
    Mesh m_sphereMesh;             /**< Siatka sfery */
    Mesh m_cylinderMesh;           /**< Siatka cylindra */
    Mesh m_coneMesh;               /**< Siatka stożka */
    Mesh m_planeMesh;              /**< Siatka płaszczyzny */
    Mesh m_torusMesh;              /**< Siatka torusa */
    Mesh m_pyramidMesh;            /**< Siatka piramidy */
    Mesh m_gridMesh;               /**< Siatka siatki pomocniczej */

    unsigned int m_lineVAO;        /**< VAO dla linii */
    unsigned int m_lineVBO;        /**< VBO dla linii */
    unsigned int m_pointVAO;       /**< VAO dla punktów */
    unsigned int m_pointVBO;       /**< VBO dla punktów */

    /**
     * @brief Konfiguruje siatkę 3D z podanych wierzchołków i indeksów
     * @param mesh Referencja do struktury Mesh
     * @param vertices Wektor wierzchołków
     * @param indices Wektor indeksów
     */
    void setupMesh(Mesh& mesh, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    /**
     * @brief Usuwa zasoby siatki 3D
     * @param mesh Referencja do struktury Mesh
     */
    void deleteMesh(Mesh& mesh);

    /**
     * @brief Tworzy siatkę sześcianu jednostkowego
     */
    void createCube();

    /**
     * @brief Tworzy siatkę sfery jednostkowej
     * @param sectors Liczba sektorów (dokładność wokół osi Z)
     * @param stacks Liczba warstw (dokładność wzdłuż osi Y)
     */
    void createSphere(int sectors = 32, int stacks = 32);

    /**
     * @brief Tworzy siatkę cylindra jednostkowego
     * @param sectors Liczba sektorów (dokładność okręgu)
     */
    void createCylinder(int sectors = 32);

    /**
     * @brief Tworzy siatkę stożka jednostkowego
     * @param sectors Liczba sektorów (dokładność okręgu)
     */
    void createCone(int sectors = 32);

    /**
     * @brief Tworzy siatkę płaszczyzny jednostkowej
     */
    void createPlane();

    /**
     * @brief Tworzy siatkę torusa
     * @param radius Główny promień torusa
     * @param tubeRadius Promień rury torusa
     * @param sectors Liczba sektorów
     * @param rings Liczba pierścieni
     */
    void createTorus(float radius = 0.5f, float tubeRadius = 0.2f, int sectors = 32, int rings = 32);

    /**
     * @brief Tworzy siatkę piramidy (ostrosłupa kwadratowego)
     */
    void createPyramid();

    /**
     * @brief Tworzy siatkę pomocniczej siatki 2D
     * @param size Rozmiar siatki (liczba linii)
     */
    void createGrid(int size = 10);

public:
    /**
     * @brief Konstruktor GeometryRenderer
     */
    GeometryRenderer();

    /**
     * @brief Destruktor GeometryRenderer
     *
     * Zwalnia wszystkie zasoby OpenGL (VAO, VBO, EBO)
     */
    ~GeometryRenderer();

    /**
     * @brief Inicjalizuje renderer geometryczny
     * @return true jeśli inicjalizacja się powiodła, false w przeciwnym razie
     */
    bool initialize();

    // Podstawowe funkcje rysowania

    /**
     * @brief Rysuje sześcian z transformacją
     * @param position Pozycja sześcianu
     * @param scale Skala sześcianu
     * @param rotation Rotacja sześcianu (kąty Eulera w stopniach)
     */
    void drawCube(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation);

    /**
     * @brief Rysuje sferę
     * @param position Pozycja środka sfery
     * @param radius Promień sfery
     */
    void drawSphere(const glm::vec3& position, float radius);

    /**
     * @brief Rysuje cylinder
     * @param position Pozycja środka cylindra
     * @param height Wysokość cylindra
     * @param radius Promień cylindra
     */
    void drawCylinder(const glm::vec3& position, float height, float radius);

    /**
     * @brief Rysuje stożek
     * @param position Pozycja środka stożka
     * @param height Wysokość stożka
     * @param radius Promień podstawy stożka
     */
    void drawCone(const glm::vec3& position, float height, float radius);

    /**
     * @brief Rysuje płaszczyznę
     * @param position Pozycja środka płaszczyzny
     * @param size Rozmiar płaszczyzny (szerokość, głębokość)
     */
    void drawPlane(const glm::vec3& position, const glm::vec2& size);

    /**
     * @brief Rysuje torus
     * @param position Pozycja środka torusa
     * @param majorRadius Główny promień torusa
     * @param minorRadius Promień rury torusa
     */
    void drawTorus(const glm::vec3& position, float majorRadius, float minorRadius);

    /**
     * @brief Rysuje piramidę
     * @param position Pozycja środka podstawy piramidy
     * @param baseSize Rozmiar podstawy piramidy
     * @param height Wysokość piramidy
     */
    void drawPyramid(const glm::vec3& position, float baseSize, float height);

    /**
     * @brief Rysuje siatkę pomocniczą
     * @param position Pozycja środka siatki
     * @param size Rozmiar siatki (liczba komórek)
     * @param spacing Odstęp między liniami siatki
     */
    void drawGrid(const glm::vec3& position, int size, float spacing);

    // Funkcje pomocnicze

    /**
     * @brief Rysuje linię pomiędzy dwoma punktami
     * @param start Punkt początkowy linii
     * @param end Punkt końcowy linii
     * @param color Kolor linii (domyślnie biały)
     */
    void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.0f));

    /**
     * @brief Rysuje punkt
     * @param position Pozycja punktu
     * @param size Rozmiar punktu (domyślnie 5.0)
     * @param color Kolor punktu (domyślnie biały)
     */
    void drawPoint(const glm::vec3& position, float size = 5.0f, const glm::vec3& color = glm::vec3(1.0f));

    /**
     * @brief Rysuje układ współrzędnych 3D
     * @param length Długość osi (domyślnie 1.0)
     */
    void drawCoordinateSystem(float length = 1.0f);

    /**
     * @brief Rysuje 3-wymiarową siatkę pomocniczą
     * @param center Środek siatki
     * @param size Rozmiar siatki (liczba komórek)
     * @param spacing Odstęp między liniami siatki
     */
    void draw3DGrid(const glm::vec3& center, int size, float spacing);

    /**
     * @brief Rysuje strzałkę
     * @param start Punkt początkowy strzałki
     * @param end Punkt końcowy strzałki
     * @param headSize Rozmiar główki strzałki (domyślnie 0.1)
     */
    void drawArrow(const glm::vec3& start, const glm::vec3& end, float headSize = 0.1f);

    /**
     * @brief Rysuje prostopadłościan (kontur)
     * @param min Minimalny punkt (lewy-dolny-tylny)
     * @param max Maksymalny punkt (prawy-górny-przedni)
     */
    void drawBox(const glm::vec3& min, const glm::vec3& max);

    /**
     * @brief Rysuje szkielet sfery (wireframe)
     * @param position Pozycja środka sfery
     * @param radius Promień sfery
     * @param segments Liczba segmentów (dokładność)
     */
    void drawSphereWireframe(const glm::vec3& position, float radius, int segments = 32);

    /**
     * @brief Rysuje wielokąt
     * @param vertices Lista wierzchołków wielokąta
     * @param filled Czy wypełnić wielokąt (domyślnie false)
     */
    void drawPolygon(const std::vector<glm::vec3>& vertices, bool filled = false);

    /**
     * @brief Rysuje okrąg w płaszczyźnie XZ
     * @param center Środek okręgu
     * @param radius Promień okręgu
     * @param segments Liczba segmentów (dokładność)
     */
    void drawCircle(const glm::vec3& center, float radius, int segments = 32);

    /**
     * @brief Rysuje pierścień (dysk z otworem) w płaszczyźnie XZ
     * @param center Środek pierścienia
     * @param innerRadius Wewnętrzny promień
     * @param outerRadius Zewnętrzny promień
     * @param segments Liczba segmentów (dokładność)
     */
    void drawDisk(const glm::vec3& center, float innerRadius, float outerRadius, int segments = 32);

    /**
     * @brief Rysuje "harnas" (funkcja eksperymentalna)
     * @param position Pozycja harnasa
     * @param height Wysokość harnasa
     * @param radius Promień harnasa
     */
    void drawHarnas(const glm::vec3& position, float height, float radius);

    // Ustawienia

    /**
     * @brief Ustawia właściwości materiału
     * @param ambient Składowa otoczenia
     * @param diffuse Składowa rozproszenia
     * @param specular Składowa odbicia
     * @param shininess Współczynnik połysku
     */
    void setMaterial(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess);

    /**
     * @brief Ustawia kolor dla wszystkich składowych materiału
     * @param color Kolor bazowy
     */
    void setColor(const glm::vec3& color);

    /**
     * @brief Ustawia tryb rysowania OpenGL
     * @param mode Tryb rysowania (GL_TRIANGLES, GL_LINES, GL_POINTS itp.)
     */
    void setDrawMode(GLenum mode);

    /**
     * @brief Ustawia macierz modelu (do implementacji w shaderze)
     * @param model Macierz modelu
     */
    void setModelMatrix(const glm::mat4& model);

    /**
     * @brief Ustawia macierz widoku (do implementacji w shaderze)
     * @param view Macierz widoku
     */
    void setViewMatrix(const glm::mat4& view);

    /**
     * @brief Ustawia macierz rzutowania (do implementacji w shaderze)
     * @param projection Macierz rzutowania
     */
    void setProjectionMatrix(const glm::mat4& projection);

    /**
     * @brief Rysuje dowolną siatkę Mesh
     * @param mesh Referencja do siatki Mesh
     */
    void drawMesh(const Mesh& mesh);
};

#endif // GEOMETRY_RENDERER_HPP