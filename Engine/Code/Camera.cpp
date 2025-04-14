#include"Camera.h"
#include "engine.h"

Camera::Camera()
    :
    m_WorldUp(glm::vec3(0, 1, 0)),

    m_position(glm::vec3(0, 3.5, -18)),
    m_zVector(glm::vec3(0, 0, 1)),
    m_yVector(glm::vec3(0, 1, 0.)),
    m_xVector(glm::normalize(glm::cross(m_WorldUp, m_zVector))),

    m_Sensitivity(5.0f),

    m_Yaw(90.0f),
    m_Pitch(0.0f),

    m_verticalFov(glm::radians(60.0f)),
    m_aspectRatio(16.0f / 9.0f),

    m_nearPlane(0.1f),
    m_farPlane(100.0f),

    m_projectionMat(glm::perspective(m_verticalFov, m_aspectRatio, m_nearPlane, m_farPlane)),
    m_viewMat(glm::lookAt(m_position, m_position + m_zVector, m_yVector)),

    projectionMatChanged(false),
    c_componentsChanged(false)
{
    UpdateVectors();
}

void Camera::Update(App * app)
{
    glm::vec3 newPos(0, 0, 0);
    float speed = m_Sensitivity * app->deltaTime;
    
    if (app->input.keys[K_LEFT_SHIFT] == BUTTON_PRESSED) speed = (m_Sensitivity * 2) * app->deltaTime;

    //-------- Camera Rotation --------//
    if (app->input.mouseButtons[LEFT] == BUTTON_PRESSED && app->input.mouseButtons[RIGHT] == BUTTON_PRESSED) // Both mouse buttons held
    {
        CameraRotation(app, app->deltaTime);
        c_componentsChanged = true;
    }

    //if (app->input.keys[K_LEFT_ALT] == BUTTON_PRESSED && app->input.mouseButtons[LEFT] == BUTTON_PRESSED) // Alt + left mouse
    //{
    //    glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

    //    LookAt(center);
    //    CameraRotation(app, app->deltaTime);
    //    c_componentsChanged = true;
    //}

    //-------- Camera Zoom --------//
    if (app->input.mouseScrollY != 0)
    {
        newPos = CameraZoom(app, m_position, speed * 5);
        c_componentsChanged = true;
    }

    //-------- FPS Movement --------//
    if (app->input.keys[K_LEFT_ALT] == BUTTON_IDLE && app->input.mouseButtons[RIGHT] == BUTTON_PRESSED) // Right mouse held
    {
        newPos = FPSMovement(app, m_position, speed);

        c_componentsChanged = true;
    }

    if (newPos != glm::vec3(0.0f))
    {
        SetPosition(newPos);
    }
   
    //-------- Center Camera --------//
    if (app->input.keys[K_F] == BUTTON_PRESSED)
    {
        glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

        LookAt(center);

        SetPosition(glm::vec3 (0.0f, 3.5f, -18.0f));

        c_componentsChanged = true;
    }

    if (c_componentsChanged)
    {
        SetViewMatrix();
        app->UpdateCameraUniforms(app);
        c_componentsChanged = false;
    }
}

glm::vec3 Camera::FPSMovement(App* app, glm::vec3 newPos, float speed)
{
    if (app->input.keys[K_Q] == BUTTON_PRESSED) newPos += m_yVector * speed;
    if (app->input.keys[K_E] == BUTTON_PRESSED) newPos -= m_yVector * speed;

    if (app->input.keys[K_W] == BUTTON_PRESSED) newPos += m_zVector * speed;
    if (app->input.keys[K_S] == BUTTON_PRESSED) newPos -= m_zVector * speed;

    if (app->input.keys[K_A] == BUTTON_PRESSED) newPos += m_xVector * speed;
    if (app->input.keys[K_D] == BUTTON_PRESSED) newPos -= m_xVector * speed;

    return newPos;
}

void Camera::CameraRotation(App* app, float dt) 
{
    float dx = app->input.mouseDelta.x * (m_Sensitivity / 10);
    float dy = app->input.mouseDelta.y * (m_Sensitivity / 10);

    m_Yaw += dx;
    m_Pitch -= dy;

    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;

    UpdateVectors();
}

void Camera::UpdateVectors() 
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_zVector = glm::normalize(front);
    m_xVector = glm::normalize(glm::cross(m_WorldUp, m_zVector));
    m_yVector = glm::normalize(glm::cross(m_zVector, m_xVector));
}

glm:: vec3 Camera::CameraZoom(App* app, glm::vec3& newPos, float speed)
{
    if (app->input.mouseScrollY > 0) newPos += GetZvector() * speed;
    if (app->input.mouseScrollY < 0) newPos -= GetZvector() * speed;

    // Reset scroll after use to prevent continuous zooming
    app->input.mouseScrollY = 0;
    return newPos;
}

// --- Matrix --- //
glm::mat4 Camera::GetProjectionMatrix() const
{
    return m_projectionMat;
    //return glm::perspective(m_verticalFov, m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::SetProjectionMatrix()
{
    m_projectionMat = glm::perspective(m_verticalFov, m_aspectRatio, m_nearPlane, m_farPlane);
}

glm::mat4 Camera::GetViewMatrix() const 
{
    return m_viewMat;
    //return glm::lookAt(m_position, m_zVector, m_yVector);
}

void Camera::SetViewMatrix()
{
    m_viewMat = glm::lookAt(m_position, m_position + m_zVector, m_yVector);
}

//-------- Aspect Ratio --------//
void Camera::SetAspectRatio(App* app, float aspectRatio) {
    m_aspectRatio = aspectRatio;
    SetProjectionMatrix();
    SetViewMatrix();
    app->UpdateCameraUniforms(app);
}

//-------- Camera View --------//
void Camera::LookAt(glm::vec3& reference) {
    glm::vec3 direction = glm::normalize(reference - m_position);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 right = glm::normalize(glm::cross(worldUp, direction));
    glm::vec3 up = glm::normalize(glm::cross(direction, right));

    m_zVector = direction;
    m_yVector = up;
    m_xVector = right;

    m_Yaw = glm::degrees(atan2f(direction.z, direction.x));
    m_Pitch = glm::degrees(asinf(direction.y));
}

//-------- Camera Position --------//
glm::vec3 Camera::GetPosition() const 
{
    return m_position;
}
void Camera::SetPosition(const glm::vec3 position) {
    m_position = position;
}

//-------- Camera Vectors --------//
glm::vec3 Camera::GetXvector() 
{
    return glm::normalize(glm::cross(glm::normalize(m_zVector), glm::normalize(m_yVector)));
}

glm::vec3 Camera::GetYvector() 
{
    return m_yVector;
}

void Camera::SetYvector(glm::vec3 y) 
{
    m_yVector = glm::normalize(y);
}

void Camera::SetZvector(glm::vec3 z) 
{
    m_zVector = glm::normalize(z);
}

glm::vec3 Camera::GetZvector() {
    return m_zVector;
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
