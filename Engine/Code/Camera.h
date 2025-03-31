#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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

public:
	Camera();
	//-------- Matrix --------//
	glm::mat4 GetProjectionMatrix() const;
	glm::mat4 GetViewMatrix() const;

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
