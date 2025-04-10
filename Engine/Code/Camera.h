#ifndef CAMERA_H
#define CAMERA_H

#include "platform.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct App;

struct CameraSettings
{
	glm::vec3 position;
	glm::vec3 target;

	glm::vec3 upVector;
};

class Camera
{
private:
	// Camera parameters
	glm::vec3 m_position;
	glm::vec3 m_zVector; // Forward vector
	glm::vec3 m_yVector; // Up vector

	// Projection parameters
	float m_verticalFov;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;

	glm::mat4 m_projectionMat;
	glm::mat4 m_viewMat;

	bool projectionMatChanged;
	bool viewMatChanged;
	bool c_componentsChanged;

public:
	Camera();

	void Update(App* app);

	//-------- Movement --------//
	void FPSMovement(App* app, glm::vec3& newPos, float speed);
	void CameraRotation(App* app, float dt);
	void CameraZoom(App* app, glm::vec3& newPos, float speed);

	//-------- Matrix --------//
	glm::mat4 GetProjectionMatrix() const;
	void SetProjectionMatrix();

	glm::mat4 GetViewMatrix() const;
	void SetViewMatrix();


	//-------- Vertical FOV --------//
	void SetVerticalFov(float vertical_fov);
	float GetVerticalFov() const;

	//-------- Horizontal FOV --------//
	void SetHorizontalFov(float horitzontal_fov);
	float GetHorizontalFov() const;

	//-------- Aspect Ratio --------//
	void SetAspectRatio(float AspectRatio);

	//-------- Far Plane --------//
	void SetFarPlane(float far_plane);
	float GetFarPlane() const;

	//-------- Near Plane --------//
	void SetNearPlane(float near_plane);
	float GetNearPlane() const;

	//-------- Camera Position --------//
	void SetPosition(const glm::vec3 position);
	glm::vec3 GetPosition() const;
	void MoveCamera(glm::vec3 newPos);

	//-------- Camera Vectors --------//
	void SetZvector(glm::vec3 z);
	void SetYvector(glm::vec3 y);

	glm::vec3 GetXvector();
	glm::vec3 GetZvector();
	glm::vec3 GetYvector();

	//-------- Camera View --------//
	void LookAt(glm::vec3& reference);
};

glm::mat4 TransformScale(const glm::vec3& scaleFactors);

glm::mat4 TransformPositionScale(const glm::vec3& pos, const glm::vec3& scaleFactors);

#endif // CAMERA_H
