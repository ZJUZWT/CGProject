#pragma once
#include "ViewHeader.h"

class ViewObject
{
public:
	// rotate this object around certain axis(x, y, z) and angle degree
	void Rotate(const GLfloat& angle, const glm::vec3& axis = glm::vec3(0.0f, 0.0f, 1.0f));
	// translate this object by displacement vector
	void Translate(const glm::vec3& displacement);
	// scale this object with (x, y, z)
	void Scale(const glm::vec3& scaler);
	// get model matrix of this object
	glm::mat4 GetM() const;
	// set model matrix of this object
	void SetM(const glm::mat4& M);

protected:
	glm::mat4 modelMat = glm::mat4(1.0f);
};