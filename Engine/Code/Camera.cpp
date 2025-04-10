#include"Camera.h"
#include "engine.h"

Camera::Camera()
    : m_position(glm::vec3(0, 1, 2)),
    m_zVector(glm::vec3(0, 0, 0)),
    m_yVector(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_verticalFov(glm::radians(60.0f)),
    m_aspectRatio(16.0f / 9.0f),
    m_nearPlane(0.1f),
    m_farPlane(100.0f),
    c_componentsChanged(false)

{}

void Camera::Update(App * app)
{
    glm::vec3 newPos(0, 0, 0);
    float speed = 10.0f * app->deltaTime;
    
    if (app->input.keys[K_LEFT_SHIFT] == BUTTON_PRESSED) speed = 20.0f * app->deltaTime;

    //-------- Camera Rotation --------//
    if (app->input.mouseButtons[LEFT] == BUTTON_PRESSED && app->input.mouseButtons[RIGHT] == BUTTON_PRESSED) // Both mouse buttons held
    {
        CameraRotation(app, app->deltaTime);
        c_componentsChanged = true;
    }

    if (app->input.keys[K_LEFT_ALT] == BUTTON_PRESSED && app->input.mouseButtons[LEFT] == BUTTON_PRESSED) // Alt + left mouse
    {
        CameraRotation(app, app->deltaTime);
        c_componentsChanged = true;
    }

    //-------- Camera Zoom --------//
    if (app->input.mouseScrollY != 0)
    {
        CameraZoom(app, newPos, speed);
        c_componentsChanged = true;
    }

    //-------- FPS Movement --------//
    if (app->input.mouseButtons[MIDDLE] == BUTTON_IDLE && app->input.keys[K_LEFT_ALT] == BUTTON_IDLE && app->input.mouseButtons[RIGHT] == BUTTON_PRESSED) // Right mouse held
    {
        FPSMovement(app, newPos, speed);

        c_componentsChanged = true;
    }

    //-------- Center Camera --------//
    if (app->input.keys[K_F] == BUTTON_PRESSED)
    {
        glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

        LookAt(center);

        SetPosition(glm::vec3 (0.0f, 1.0f, 5.0f));

        c_componentsChanged = true;
    }

    MoveCamera(newPos);

    if (c_componentsChanged)
    {
        app->UpdateCameraUniforms(app);
        c_componentsChanged = false;
    }
}

void Camera::FPSMovement(App* app, glm::vec3& newPos, float speed)
{
    if (app->input.keys[K_Q] == BUTTON_PRESSED) newPos += GetYvector() * speed;
    if (app->input.keys[K_E] == BUTTON_PRESSED) newPos -= GetYvector() * speed;

    if (app->input.keys[K_W] == BUTTON_PRESSED) newPos += GetZvector() * speed;
    if (app->input.keys[K_S] == BUTTON_PRESSED) newPos -= GetZvector() * speed;

    if (app->input.keys[K_A] == BUTTON_PRESSED) newPos -= GetXvector() * speed;
    if (app->input.keys[K_D] == BUTTON_PRESSED) newPos += GetXvector() * speed;
}

void Camera::CameraRotation(App* app, float dt)
{
    float dx = -app->input.mouseDelta.x;
    float dy = -app->input.mouseDelta.y;

    float Sensitivity = 0.35f * dt;

    // Get current camera vectors
    glm::vec3 Y = GetYvector(); // Up
    glm::vec3 Z = GetZvector(); // Forward
    glm::vec3 X = GetXvector(); // Right

    // Horizontal rotation (around world Y-axis)
    if (dx != 0.0f) {
        float deltaX = dx * Sensitivity;
        glm::quat yRot = glm::angleAxis(deltaX, glm::vec3(0.0f, 1.0f, 0.0f));

        // Apply rotation to camera vectors
        Y = yRot * Y;
        Z = yRot * Z;
    }

    // Vertical rotation (around camera's local X-axis)
    if (dy != 0.0f) {
        float deltaY = dy * Sensitivity;
        glm::quat xRot = glm::angleAxis(deltaY, X);

        // Apply rotation to camera vectors
        Y = xRot * Y;
        Z = xRot * Z;

        // Prevent camera flipping (up/down)
        if (glm::dot(Y, glm::vec3(0.0f, 1.0f, 0.0f)) < 0.0f) {
            // Clamp to horizon
            Z = glm::vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
            Y = glm::cross(Z, X);
        }
    }

    // Update camera vectors
    SetYvector(Y);
    SetZvector(Z);
}

void Camera::CameraZoom(App* app, glm::vec3& newPos, float speed)
{
    if (app->input.mouseScrollY > 0) newPos += GetZvector() * speed;
    if (app->input.mouseScrollY < 0) newPos -= GetZvector() * speed;

    // Reset scroll after use to prevent continuous zooming
    app->input.mouseScrollY = 0;
}

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