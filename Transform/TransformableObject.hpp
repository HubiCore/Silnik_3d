// TransformableObject.hpp
#ifndef TRANSFORMABLE_OBJECT_HPP
#define TRANSFORMABLE_OBJECT_HPP

#include "Transform.hpp"
#include "../GeometryRenderer.hpp"
#include <memory>

/**
 * @class TransformableObject
 * @brief Bazowa klasa dla wszystkich transformowalnych obiektów 3D
 *
 * Klasa abstrakcyjna dostarczająca podstawowe funkcjonalności transformacji
 * (przesunięcie, obrót, skalowanie) oraz hierarchii obiektów
 */
class TransformableObject {
protected:
    std::unique_ptr<Transform> m_transform;  /**< Transformacja obiektu */
    GeometryRenderer* m_renderer;            /**< Wskaźnik do renderera */

public:
    /**
     * @brief Konstruktor TransformableObject
     *
     * Inicjalizuje transformację i ustawia renderer na nullptr
     */
    TransformableObject();

    /**
     * @brief Wirtualny destruktor TransformableObject
     */
    virtual ~TransformableObject();

    // Transformacje

    /**
     * @brief Ustawia pozycję obiektu w przestrzeni świata
     * @param position Nowa pozycja (wektor 3D)
     */
    virtual void setPosition(const glm::vec3& position);

    /**
     * @brief Ustawia rotację obiektu za pomocą kwaternionu
     * @param rotation Nowa rotacja (kwaternion)
     */
    virtual void setRotation(const glm::quat& rotation);

    /**
     * @brief Ustawia rotację obiektu za pomocą kątów Eulera
     * @param eulerAngles Kąty Eulera w stopniach (pitch, yaw, roll)
     */
    virtual void setRotation(const glm::vec3& eulerAngles);

    /**
     * @brief Ustawia skalę obiektu
     * @param scale Nowa skala (wektor 3D)
     */
    virtual void setScale(const glm::vec3& scale);

    /**
     * @brief Przesuwa obiekt o podany wektor
     * @param translation Wektor przesunięcia
     * @param space Przestrzeń w której wykonywana jest transformacja (domyślnie lokalna)
     */
    virtual void translate(const glm::vec3& translation, Space space = Space::LOCAL);

    /**
     * @brief Obraca obiekt za pomocą kwaternionu
     * @param rotation Kwaternion rotacji
     * @param space Przestrzeń w której wykonywana jest transformacja (domyślnie lokalna)
     */
    virtual void rotate(const glm::quat& rotation, Space space = Space::LOCAL);

    /**
     * @brief Obraca obiekt wokół osi o podany kąt
     * @param axis Oś obrotu (wektor jednostkowy)
     * @param angleDegrees Kąt obrotu w stopniach
     * @param space Przestrzeń w której wykonywana jest transformacja (domyślnie lokalna)
     */
    virtual void rotate(const glm::vec3& axis, float angleDegrees, Space space = Space::LOCAL);

    /**
     * @brief Skaluje obiekt o podany współczynnik
     * @param scaleFactor Współczynnik skalowania (wektor 3D)
     */
    virtual void scale(const glm::vec3& scaleFactor);

    /**
     * @brief Zwraca pozycję obiektu
     * @return Pozycja obiektu w przestrzeni świata
     */
    glm::vec3 getPosition() const;

    /**
     * @brief Zwraca rotację obiektu
     * @return Rotacja obiektu jako kwaternion
     */
    glm::quat getRotation() const;

    /**
     * @brief Zwraca rotację obiektu jako kąty Eulera
     * @return Kąty Eulera w stopniach (pitch, yaw, roll)
     */
    glm::vec3 getEulerAngles() const;

    /**
     * @brief Zwraca skalę obiektu
     * @return Skala obiektu (wektor 3D)
     */
    glm::vec3 getScale() const;

    /**
     * @brief Zwraca macierz modelu obiektu
     * @return Macierz transformacji modelu (world matrix)
     */
    glm::mat4 getModelMatrix() const;

    // Hierarchia

    /**
     * @brief Ustawia rodzica dla obiektu
     * @param parent Wskaźnik do obiektu-rodzica (nullptr dla brak rodzica)
     */
    void setParent(TransformableObject* parent);

    /**
     * @brief Zwraca rodzica obiektu
     * @return Wskaźnik do obiektu-rodzica
     *
     * @note W obecnej implementacji zawsze zwraca nullptr, ponieważ
     * wymagana byłaby dodatkowa mapa Transform -> TransformableObject
     */
    TransformableObject* getParent() const;

    /**
     * @brief Dodaje dziecko do obiektu
     * @param child Wskaźnik do obiektu-dziecka
     */
    void addChild(TransformableObject* child);

    // Renderowanie

    /**
     * @brief Czysto wirtualna metoda do rysowania obiektu
     */
    virtual void draw() const = 0;

    /**
     * @brief Ustawia renderer dla obiektu
     * @param renderer Wskaźnik do renderera
     */
    void setRenderer(GeometryRenderer* renderer);

    // Właściwości

    /**
     * @brief Zwraca kolor obiektu
     * @return Kolor obiektu (wektor 3D)
     */
    virtual glm::vec3 getColor() const = 0;

    /**
     * @brief Ustawia kolor obiektu
     * @param color Nowy kolor obiektu
     */
    virtual void setColor(const glm::vec3& color) = 0;

protected:
    /**
     * @brief Zwraca wskaźnik do renderera
     * @return Wskaźnik do aktualnego renderera
     */
    GeometryRenderer* getRenderer() const;
};

#endif // TRANSFORMABLE_OBJECT_HPP