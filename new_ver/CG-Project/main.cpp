#include "Scene.h"
#include "Interaction.h"

GLFWwindow* Init();
void RenderShadow();
void RenderGBuffer();
void RenderOITBufferTM();
void RenderBlendOIT();
void RenderGBufferLight();

void RenderAllObject(const ViewPassEnum & pass);

void UpdateCamera() {
	if (Interaction::key_y_flag) {
		// follow ground object
	}
	else {
		// follow airplane
		// locate camera at the back of the airplane
		glm::vec3 objFront = airplane->getFrontDir();
		glm::vec3 objPos = airplane->getPosition();
		glm::vec3 cameraDelta = objFront * (-100.0f) + glm::vec3({ 0, 1, 0 });
		Interaction::camera.SetPosition(objPos + cameraDelta);
		if (!(Interaction::left_button_pressed || Interaction::key_y_flag)) {
			// force camera to look at the airplane
			Interaction::camera.SetFrontDir(objFront);
			Interaction::camera.SetWorldUpDir(airplane->getUpDir());
		}
	}
}

void UpdateAirplane() {
	if (Interaction::key_y_flag) {
		// control ground object
	}
	else {
		GLfloat deltaPower = 0.01f;
		GLfloat deltaYaw = 0.5f;
		airplane->changePitch(-Interaction::ReadYoffset());
		airplane->changeRoll(Interaction::ReadXoffset());
		if (Interaction::key_w_pressed) {
			airplane->changePower(deltaPower);
		}
		if (Interaction::key_s_pressed) {
			airplane->changePower(-deltaPower);
		}
		if (Interaction::key_a_pressed) {
			airplane->changeYaw(deltaYaw);
		}
		if (Interaction::key_d_pressed) {
			airplane->changeYaw(-deltaYaw);
		}
		if (Interaction::key_space_pressed) {
			airplane->setPower(0);
			airplane->setVelocity({ 0, 0, 0 });
			airplane->setPosition({ 0, 100, 0 });
		}
		airplane->simulate();
	}
}

void UpdateData()
{
	UpdateAirplane();
	UpdateCamera();
	GlobalDataPool::SetData<GLfloat>("systemTime", (GLfloat)glfwGetTime());
	for (const auto& plane : fitPlaneGroup.GetObjectList())
	{
		plane->UpdateHeight(radius, 0, camera.GetViewMatrix());
	}
	if (Interaction::screenShotFlag)
	{
		try
		{
			time_t now = time(nullptr);
			struct tm info;
			localtime_s(&info, &now);
			char timeBuf[256];
			strftime(timeBuf, 256, "%Y-%m-%d_%H-%M-%S", &info);
			std::cout << "Time Now = " << timeBuf << std::endl;
			//std::string filename = std::string("IMG_") + "test" + ".jpg";
			std::string filename = std::string("IMG_") + timeBuf + ".jpg";
			ScreenShot(Scene::width, Scene::height, filename);
		}
		catch (std::string msg)
		{
			std::cout << msg << std::endl;
		}
		Interaction::screenShotFlag = false;
	}
}

int main(int argc, char** argv) {
	GLFWwindow* window = Init();

	while (!glfwWindowShouldClose(window)) {
		UpdateData();
		
		RenderShadow();
		RenderGBuffer();
		RenderOITBufferTM();
		RenderBlendOIT();
		RenderGBufferLight();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}
GLuint gBufferShowProgram;
GLuint gBufferLightProgram;
GLFWwindow* Init() {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(width, height, "CG Project", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	InitGlobalData();
	InitOIT(); 
	InitShadow();
	InitGBuffer();
	InitScene();
	InitGameObject();

	gBufferShowProgram = GenerateProgram({
		{GL_VERTEX_SHADER,"GBufferShow.vert"} ,
		{GL_FRAGMENT_SHADER,"GBufferShow.frag"}
	});

	gBufferLightProgram = GenerateProgram({
		{ GL_VERTEX_SHADER,"GBufferLight.vert" },
		{ GL_FRAGMENT_SHADER,"GBufferLight.frag" }
	});

	glfwSetKeyCallback(window, Interaction::KeyCallback);
	glfwSetCursorPosCallback(window, Interaction::MouseCallback);
	glfwSetMouseButtonCallback(window, Interaction::MouseButtonCallback);
	glfwSetScrollCallback(window, Interaction::ScrollCallback);
	return window;
}

void RenderAllObject(const ViewPassEnum& pass)
{
	for (const auto& object : skyBoxGroup.GetObjectList())
	{
		object->Render(pass);
	}
	for (const auto& object : planeGroup.GetObjectList())
	{
		object->Render(pass);
	}
	for (const auto& object : transparentGroup.GetObjectList())
	{
		object->Render(pass);
	}
	for (const auto& object : fitPlaneGroup.GetObjectList())
	{
		object->Render(pass);
	}
	for (const auto& object : waterPlaneGroup.GetObjectList())
	{
		object->Render(pass);
	}

}

void RenderGBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, GBuffers[GBuffer]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderAllObject(ViewPassEnum::GBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void RenderOITBufferTM() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glClearBufferfv(GL_COLOR, 0, bgColor); 
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, OITBuffers[HeadPointerInit]);
	glBindTexture(GL_TEXTURE_2D, OITTextures[HeadPointerTexture]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);

	// Bind head-pointer image for read-write
	glBindImageTexture(0, OITTextures[HeadPointerTexture], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	// Bind linked-list buffer for write
	glBindImageTexture(1, OITTextures[StorageTexture], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32UI);
	glBindImageTexture(2, OITTextures[StorageWorldPosTexture], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(3, OITTextures[StorageNormalTexture], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, OITBuffers[AtomicCounter]);
	const GLuint zero = 0;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(zero), &zero);

	RenderAllObject(ViewPassEnum::OITBuffer);

	glEnable(GL_DEPTH_TEST);

}
unsigned int quadVAO = 0;
unsigned int quadVBO;
void RenderBlendOIT() {
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, GBuffers[GBuffer]);
	glUseProgram(OITPrograms[RankOIT]);
	GLuint location;
	location = glGetUniformLocation(OITPrograms[RankOIT], "gPosition");
	glUniform1i(location, GPositionTexture);
	location = glGetUniformLocation(OITPrograms[RankOIT], "gNormal");
	glUniform1i(location, GNormalTexture);
	location = glGetUniformLocation(OITPrograms[RankOIT], "gAlbedoSpec");
	glUniform1i(location, GAlbedoSpecTexture);
	location = glGetUniformLocation(OITPrograms[RankOIT], "gDepthID");
	glUniform1i(location, GDepthIDTexture);
	location = glGetUniformLocation(OITPrograms[RankOIT], "gTransColor");
	glUniform1i(location, GTransColorTexture);

	GLfloat uniNear = GlobalDataPool::GetData<GLfloat>("uniNear");
	location = glGetUniformLocation(OITPrograms[RankOIT], "uniNear");
	glUniform1f(location, uniNear);
	GLfloat uniFar = GlobalDataPool::GetData<GLfloat>("uniFar");
	location = glGetUniformLocation(OITPrograms[RankOIT], "uniFar");
	glUniform1f(location, uniFar);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GTextures[GPositionTexture]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GTextures[GNormalTexture]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GTextures[GAlbedoSpecTexture]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GTextures[GDepthIDTexture]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, GTextures[GTransColorTexture]);


	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	RenderAllObject(ViewPassEnum::BlendOIT);

	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);


}

void RenderGBufferLight() {
	glm::mat4 projection = GlobalDataPool::GetData<glm::mat4>("cameraProjection");
	glm::mat4 lightProjection = GlobalDataPool::GetData<glm::mat4>("lightProjection");
	glm::mat4 lightView = GlobalDataPool::GetData<glm::mat4>("lightView");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (quadVAO == 0) {
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glUseProgram(gBufferLightProgram);
	GLuint location;
	location = glGetUniformLocation(gBufferLightProgram, "gPosition");
	glUniform1i(location, GPositionTexture);
	location = glGetUniformLocation(gBufferLightProgram, "gNormal");
	glUniform1i(location, GNormalTexture);
	location = glGetUniformLocation(gBufferLightProgram, "gAlbedoSpec");
	glUniform1i(location, GAlbedoSpecTexture);
	location = glGetUniformLocation(gBufferLightProgram, "gDepthID");
	glUniform1i(location, GDepthIDTexture);
	location = glGetUniformLocation(gBufferLightProgram, "shadowMap");
	glUniform1i(location, 4);
	location = glGetUniformLocation(gBufferLightProgram, "gTransColor");
	glUniform1i(location, GTransColorTexture + 1);

	GLfloat uniNear = GlobalDataPool::GetData<GLfloat>("uniNear");
	location = glGetUniformLocation(gBufferLightProgram, "uniNear");
	glUniform1f(location, uniNear);
	GLfloat uniFar = GlobalDataPool::GetData<GLfloat>("uniFar");
	location = glGetUniformLocation(gBufferLightProgram, "uniFar");
	glUniform1f(location, uniFar);

	location = glGetUniformLocation(gBufferLightProgram, "lightSpaceMatrix");
	glm::mat4 temp = lightProjection * lightView;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(temp));
	location = glGetUniformLocation(gBufferLightProgram, "uniP");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));
	location = glGetUniformLocation(gBufferLightProgram, "uniV");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GTextures[GPositionTexture]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GTextures[GNormalTexture]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GTextures[GAlbedoSpecTexture]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GTextures[GDepthIDTexture]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Shadow::shadowBuffers[Shadow::ShadowBuffer]);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, GTextures[GTransColorTexture]);


	SendAllLightUniformsToShader(gBufferLightProgram, directionalLight);


	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	RenderAllObject(ViewPassEnum::GBufferIlluminate);

	glBindVertexArray(0);

	glUseProgram(0);
	glBindVertexArray(0);
}

void RenderShadow() {
	using namespace Shadow;
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffers[ShadowFrameBuffer]);
	glClear(GL_DEPTH_BUFFER_BIT);

	RenderAllObject(ViewPassEnum::ShadowBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glViewport(0, 0, width, height);
}