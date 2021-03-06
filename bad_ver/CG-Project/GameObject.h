#pragma once
#include "ViewBase.h"

// alias
typedef std::array<GLfloat, POINTSIZE> vertex;
typedef std::array<vertex, 3> face;

// call this!
std::vector<std::shared_ptr<ViewTriangle>> getAllRenderData();

class GameObject {
// member variabels
private:
	bool		fixed;
	GLfloat		mass;
	glm::vec3	coordinate;
	glm::vec3	velocity;
	glm::vec3	acceleration;
	std::vector<std::shared_ptr<ViewTriangle>> faces;

// public functions
public:
	void		loadFromObj(std::string filename);
	bool		intersect(GameObject& obj);
	void		collide(GameObject& obj);
	void		scale(const glm::vec3& vec);
	void		translate(const glm::vec3& vec);
	void		rotate(const GLfloat& angle, const glm::vec3& vec);
	std::vector<std::shared_ptr<ViewTriangle>> getRenderData();
	GameObject();

// helper functions
private:
	void error(std::string msg, bool fatal = true);
};
