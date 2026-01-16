// TransformableObject.cpp
#include "TransformableObject.hpp"

/**
 * @brief Konstruktor TransformableObject
 *
 * Inicjalizuje transformację i ustawia renderer na nullptr
 */
TransformableObject::TransformableObject()
    : m_transform(std::make_unique<Transform>()), m_renderer(nullptr) {
}

/**
 * @brief Destruktor TransformableObject
 */
TransformableObject::~TransformableObject() {
}

/**
 * @brief Ustawia pozycję obiektu w przestrzeni świata
 * @param position Nowa pozycja (wektor 3D)
 */
void TransformableObject::setPosition(const glm::vec3& position) {
    m_transform->setPosition(position);
}

/**
 * @brief Ustawia rotację obiektu za pomocą kwaternionu
 * @param rotation Nowa rotacja (kwaternion)
 */
void TransformableObject::setRotation(const glm::quat& rotation) {
    m_transform->setRotation(rotation);
}

/**
 * @brief Ustawia rotację obiektu za pomocą kątów Eulera
 * @param eulerAngles Kąty Eulera w stopniach (pitch, yaw, roll)
 */
void TransformableObject::setRotation(const glm::vec3& eulerAngles) {
    m_transform->setRotation(eulerAngles);
}

/**
 * @brief Ustawia skalę obiektu
 * @param scale Nowa skala (wektor 3D)
 */
void TransformableObject::setScale(const glm::vec3& scale) {
    m_transform->setScale(scale);
}

/**
 * @brief Przesuwa obiekt o podany wektor
 * @param translation Wektor przesunięcia
 * @param space Przestrzeń w której wykonywana jest transformacja
 */
void TransformableObject::translate(const glm::vec3& translation, Space space) {
    m_transform->translate(translation, space);
}

/**
 * @brief Obraca obiekt za pomocą kwaternionu
 * @param rotation Kwaternion rotacji
 * @param space Przestrzeń w której wykonywana jest transformacja
 */
void TransformableObject::rotate(const glm::quat& rotation, Space space) {
    m_transform->rotate(rotation, space);
}

/**
 * @brief Obraca obiekt wokół osi o podany kąt
 * @param axis Oś obrotu (wektor jednostkowy)
 * @param angleDegrees Kąt obrotu w stopniach
 * @param space Przestrzeń w której wykonywana jest transformacja
 */
void TransformableObject::rotate(const glm::vec3& axis, float angleDegrees, Space space) {
    m_transform->rotate(axis, angleDegrees, space);
}

/**
 * @brief Skaluje obiekt o podany współczynnik
 * @param scaleFactor Współczynnik skalowania (wektor 3D)
 */
void TransformableObject::scale(const glm::vec3& scaleFactor) {
    m_transform->scale(scaleFactor);
}

/**
 * @brief Zwraca pozycję obiektu
 * @return Pozycja obiektu w przestrzeni świata
 */
glm::vec3 TransformableObject::getPosition() const {
    return m_transform->getPosition();
}

/**
 * @brief Zwraca rotację obiektu
 * @return Rotacja obiektu jako kwaternion
 */
glm::quat TransformableObject::getRotation() const {
    return m_transform->getRotation();
}

/**
 * @brief Zwraca rotację obiektu jako kąty Eulera
 * @return Kąty Eulera w stopniach (pitch, yaw, roll)
 */
glm::vec3 TransformableObject::getEulerAngles() const {
    return m_transform->getEulerAngles();
}

/**
 * @brief Zwraca skalę obiektu
 * @return Skala obiektu (wektor 3D)
 */
glm::vec3 TransformableObject::getScale() const {
    return m_transform->getScale();
}

/**
 * @brief Zwraca macierz modelu obiektu
 * @return Macierz transformacji modelu (world matrix)
 */
glm::mat4 TransformableObject::getModelMatrix() const {
    return m_transform->getWorldMatrix();
}

/**
 * @brief Ustawia rodzica dla obiektu
 * @param parent Wskaźnik do obiektu-rodzica (nullptr dla brak rodzica)
 */
void TransformableObject::setParent(TransformableObject* parent) {
    if (parent) {
        m_transform->setParent(parent->m_transform.get());
    } else {
        m_transform->setParent(nullptr);
    }
}

/**
 * @brief Zwraca rodzica obiektu
 * @return Wskaźnik do obiektu-rodzica
 *
 * @note W obecnej implementacji zawsze zwraca nullptr, ponieważ
 * wymagana byłaby dodatkowa mapa Transform -> TransformableObject
 */
TransformableObject* TransformableObject::getParent() const {
    Transform* parentTransform = m_transform->getParent();
    // Uwaga: to wymaga dodatkowej mapy Transform -> TransformableObject
    // Dla uproszczenia zwracamy nullptr
    return nullptr;
}

/**
 * @brief Dodaje dziecko do obiektu
 * @param child Wskaźnik do obiektu-dziecka
 */
void TransformableObject::addChild(TransformableObject* child) {
    if (child) {
        child->setParent(this);
    }
}

/**
 * @brief Ustawia renderer dla obiektu
 * @param renderer Wskaźnik do renderera
 */
void TransformableObject::setRenderer(GeometryRenderer* renderer) {
    m_renderer = renderer;
}

/**
 * @brief Zwraca wskaźnik do renderera
 * @return Wskaźnik do aktualnego renderera
 */
GeometryRenderer* TransformableObject::getRenderer() const {
    return m_renderer;
}