#include "GameScene.h"
#include "Scene.h"

GameSceneBase::GameSceneBase(const GLfloat& width, const GLfloat& height, const GLfloat& distance)
	: _width(width), _height(height), _distance(distance)
{
	std::vector<TextureInfo> planeTextures{
		{AmbientTexture,"Bricks036_2K-JPG/Bricks036_2K_Color.jpg"},
		{NormalTexture,"Bricks036_2K-JPG/Bricks036_2K_Normal.jpg"},
		{RoughnessTexture,"Bricks036_2K-JPG/Bricks036_2K_Roughness.jpg"},
		{Texture_NONE,""} };
	std::vector<GLfloat> points{
		-_width / 2.0f, GroundY, 0.0f, 0.0f, 0.0f,
		_width / 2.0f, GroundY, 0.0f, 1.0f, 0.0f,
		-_width / 2.0f, GroundY, _distance, 0.0f, 1.0f,
		_width / 2.0f, GroundY, _distance, 1.0f, 1.0f,
		-_width / 2.0f, BaseY, 0.0f, 0.0f, 0.0f,
		_width / 2.0f, BaseY, 0.0f, 1.0f, 0.0f,
		-_width / 2.0f, BaseY, _distance, 0.0f, 1.0f,
		_width / 2.0f, BaseY, _distance, 1.0f, 1.0f
	};
	std::vector<GLuint> indices{
		0, 1, 2,
		1, 2, 3,
		4, 5, 6,
		5, 6, 7,
		0, 2, 6,
		0, 4, 6,
		0, 1, 5,
		0, 4, 5,
		1, 5, 7,
		1, 3, 7,
		2, 3, 7,
		2, 6, 7
	};
	this->_roadGround.reset(new TexturedPlane(points, indices, planeTextures));
	this->_objects.push_back(this->_roadGround);
	Scene::AddGameObject(ViewObjectEnum::TextureObject, _roadGround);
}