#pragma once
#include "ViewHeader.h"

constexpr bool PERFORMANCETEST = false;					// whether use performance test

class ViewWorld : public ViewBase, public std::enable_shared_from_this<ViewWorld>
{
// pure virtual function zone
protected:
	virtual void InitGLSetting() = 0;
	virtual void InitTexture() = 0;
	virtual void InitGlobalUniformData() = 0;
	virtual void InitModules() = 0;
	virtual void InitGroups() = 0;
	virtual void InitCallback() = 0;

	virtual void UpdateData() = 0;


// non-pure virtual function zone
public:
	// initial groups and modules
	virtual void Init();
	// main loop
	virtual void Loop();

// other function zone
public:
	// main render
	virtual void Render() final;

	// get a object from all the group
	std::shared_ptr<ViewObject> GetObject(const HandleT& ID) const;

	// return the handle id of object
	HandleT AddGroup(std::shared_ptr<ViewGroup>&& group);
	// remove group
	void RemoveGroup(const int & groupID);
	// return the handle id of module
	GLuint AddModule(std::shared_ptr<ViewModule>&& mod);
	// remove module
	void RemoveModule(int modID);

	// get world name
	std::string GetName() const;
	// set world name
	void SetName(const std::string& name);

	ViewWorld() {}
	virtual ~ViewWorld();

// data zone
protected:
	GLFWwindow* _window = nullptr;
	std::string _name = "CG Project";
	int _height = 768;
	int _width = 768;

	std::vector<std::shared_ptr<ViewGroup>> _groups;
	std::vector<std::shared_ptr<ViewModule>> _modules;
};

class GameWorld final : public ViewWorld
{
public:
	GameWorld();
protected:
	virtual void InitGLSetting();
	virtual void InitTexture();
	virtual void InitGlobalUniformData();
	virtual void InitModules();
	virtual void InitGroups();
	virtual void InitCallback();
	virtual void UpdateData();
	virtual void UpdateCamera();
	void UpdateAirplane();

	//static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	//static void MouseCallback(GLFWwindow* window, double xpos, double ypos) {}
	//static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {}
	//static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}
private:
	//ViewCamera _camera = 
	//glm::mat4 _model = glm::mat4(1.0f);
	const GLfloat viewDist = 30;
	glm::mat4 _view = glm::mat4(1.0f);
	glm::mat4 _projection = glm::perspective(glm::radians(45.0f), (float)_width / (float)_height, 0.1f, 500.0f);
	DirectionalLight directionalLight;
	std::vector<std::shared_ptr<GameObject>> GameObjects;
	std::shared_ptr<Airplane> airplane;
};

void printVec(const glm::vec3& vec);