#include"Camera.h"

Camera::Camera()
    : m_position(glm::vec3(0, 10, 25)),
    m_zVector(glm::vec3(0, 0, 0)),
    m_yVector(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_verticalFov(glm::radians(60.0f)),
    m_aspectRatio(16.0f / 9.0f),
    m_nearPlane(0.1f),
    m_farPlane(100.0f)
{}

glm::mat4 Camera::GetProjectionMatrix() const 
{
    return glm::perspective(m_verticalFov, m_aspectRatio, m_nearPlane, m_farPlane);
}

glm::mat4 Camera::GetViewMatrix() const 
{
    return glm::lookAt(m_position, m_zVector, m_yVector);
}

//-------- Vertical FOV --------//
void Camera::SetVerticalFov(float vertical_fov) {
    m_verticalFov = vertical_fov;
}

float Camera::GetVerticalFov() const {
    return m_verticalFov;
}

//-------- Horizontal FOV --------//
void Camera::SetHorizontalFov(float horizontal_fov) {
    m_verticalFov = 2.0f * std::atan(std::tan(horizontal_fov / 2.0f) / m_aspectRatio);
}

float Camera::GetHorizontalFov() const {
    return 2.0f * std::atan(std::tan(m_verticalFov / 2.0f) * m_aspectRatio);
}

//-------- Aspect Ratio --------//
void Camera::SetAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
}

//-------- Far Plane --------//
void Camera::SetFarPlane(float far_plane) {
    m_farPlane = far_plane;
}

float Camera::GetFarPlane() const {
    return m_farPlane;
}

//-------- Near Plane --------//
void Camera::SetNearPlane(float near_plane) {
    m_nearPlane = near_plane;
}

float Camera::GetNearPlane() const {
    return m_nearPlane;
}

//-------- Camera Position --------//
void Camera::SetPosition(const glm::vec3 position) {
    m_position = position;
}

glm::vec3 Camera::GetPosition() const {
    return m_position;
}

void Camera::MoveCamera(glm::vec3 newPos) {
    m_position = newPos;
}

//-------- Camera Vectors --------//
void Camera::SetZvector(glm::vec3 z) {
    m_zVector = glm::normalize(z);
}

void Camera::SetYvector(glm::vec3 y) {
    m_yVector = glm::normalize(y);
}

glm::vec3 Camera::GetXvector() {
    return glm::normalize(glm::cross(glm::normalize(m_zVector), glm::normalize(m_yVector)));
}

glm::vec3 Camera::GetZvector() {
    return m_zVector;
}

glm::vec3 Camera::GetYvector() {
    return m_yVector;
}

//-------- Camera View --------//
void Camera::LookAt(glm::vec3& reference) {
    glm::vec3 direction = glm::normalize(reference - m_position);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f); // Default world up

    glm::vec3 right = glm::normalize(glm::cross(direction, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, direction));

    m_zVector = direction;
    m_yVector = up;
}

glm::mat4 TransformScale(const glm::vec3& scaleFactors)
{
    glm::mat4 transform = scale(scaleFactors);
    return transform;
}

glm::mat4 TransformPositionScale(const glm::vec3& pos, const glm::vec3& scaleFactors)
{
    glm::mat4 transform = translate(pos);
    transform = scale(transform, scaleFactors);
    return transform;
}