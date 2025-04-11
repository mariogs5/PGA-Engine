#ifndef CAMERA_H
#define CAMERA_H

#include "platform.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct App;

class Camera
{
private:
	// --- World --- //
	glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// --- Camera Vectors --- //
	glm::vec3 m_position; // Position
	glm::vec3 m_xVector;  // xVector
	glm::vec3 m_yVector;  // yVector
	glm::vec3 m_zVector;  // zVector

	float m_Yaw;
	float m_Pitch;

	// --- Projection parameters --- //
	float m_verticalFov;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;

	// --- Matrix --- //
	glm::mat4 m_projectionMat;
	bool projectionMatChanged;

	glm::mat4 m_viewMat;
	
	float m_Sensitivity;
	bool c_componentsChanged;

public:
	Camera();

	void Update(App* app);

	//-------- Movement --------//
	glm::vec3 FPSMovement(App* app, glm::vec3 newPos, float speed);
	void CameraRotation(App* app, float dt);
	void UpdateVectors();
	glm::vec3 CameraZoom(App* app, glm::vec3& newPos, float speed);

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

	//-------- Camera Vectors --------//
	void SetZvector(glm::vec3 z);
	void SetYvector(glm::vec3 y);

	glm::vec3 GetXvector();
	glm::vec3 GetZvector();
	glm::vec3 GetYvector();

	//-------- Camera View --------//
	void LookAt(glm::vec3& reference);
};

#endif // CAMERA_H
