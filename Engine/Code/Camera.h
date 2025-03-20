#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Camera
{
	glm::vec3 position;
	glm::vec3 target;

	glm::vec3 upVector;
};

glm::mat4 TransformScale(const glm::vec3& scaleFactors);

glm::mat4 TransformPositionScale(const glm::vec3& pos, const glm::vec3& scaleFactors);

#endif // CAMERA_H
