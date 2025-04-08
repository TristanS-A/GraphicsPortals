#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include <ew/transform.h>
#include <ew/texture.h>
#include <ew/procGen.h>

#include <tsa/framebuffer.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;



//Caching things
ew::Camera camera;
ew::CameraController camController;

ew::Model* pSuzanne;
ew::Transform suzanneTransform;

bool usingNormalMap = true;

struct Portal 
{
	public:
		Portal* linkedPortal;
		ew::Mesh portalMesh;
		ew::Transform regularPortalTransform;
		glm::vec3 normal;
		tsa::FrameBuffer framebuffer;
};

Portal coolPortal;
Portal coolerAwesomePortal;
GLint rockNormal;

void renderScene(ew::Shader& shader, GLuint tex, glm::mat4 view)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	//GFX Pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);


	shader.use();

	shader.setMat4("_Model", coolPortal.regularPortalTransform.modelMatrix());
	shader.setMat4("camera_viewProj",view);
	shader.setInt("_MainTex", 0);

	coolPortal.portalMesh.draw();
	
	shader.setMat4("_Model", coolerAwesomePortal.regularPortalTransform.modelMatrix());
	coolerAwesomePortal.portalMesh.draw();
	
	glCullFace(GL_BACK);

	shader.setMat4("_Model", suzanneTransform.modelMatrix());
	pSuzanne->draw();
}

void renderPortalView(Portal& p, ew::Shader& sceneShader)
{
	//calcualte cam
	ew::Camera portal;

	glm::mat4 destinationView =
		camera.viewMatrix() * p.regularPortalTransform.modelMatrix()
		* glm::rotate(glm::mat4(1.0f), glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::inverse(p.linkedPortal->regularPortalTransform.modelMatrix());

	glBindFramebuffer(GL_FRAMEBUFFER, p.framebuffer.fbo);
	{
		glEnable(GL_DEPTH_TEST);
		renderScene(sceneShader, rockNormal, destinationView);

	}glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

int main() {
	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	camera.position = { 0.0f, 0.0f, 5.0f };
	camera.target = { 0.0f, 0.0f, 0.0f };
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	ew::Shader lit_Shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader defaultLit = ew::Shader("assets/Portal/Default.vert", "assets/Portal/Default.frag");

	pSuzanne = new ew::Model("assets/suzanne.obj");

	coolPortal.portalMesh = ew::createCube(5);
	coolPortal.regularPortalTransform.position = glm::vec3(0, 0, 0);
	coolPortal.linkedPortal = &coolerAwesomePortal;
	coolPortal.framebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);

	coolerAwesomePortal.portalMesh = ew::createCube(5);
	coolerAwesomePortal.regularPortalTransform.position = glm::vec3(10, 0, 0);
	coolerAwesomePortal.framebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);
	coolerAwesomePortal.linkedPortal = &coolPortal;

	GLint Rock_Color = ew::loadTexture("assets/Rock_Color.png");
	rockNormal = ew::loadTexture("assets/Rock_Normal.png");

	suzanneTransform.position = glm::vec3(10, 0, -7);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//RENDER
		camController.move(window, &camera, deltaTime);
		//thing(lit_Shader, suzanne, suzanneTransform, Rock_Color, rockNormal, deltaTime);
		renderPortalView(coolPortal, defaultLit);
		renderPortalView(coolerAwesomePortal, defaultLit);
		renderScene(defaultLit, rockNormal, camera.projectionMatrix() * camera.viewMatrix());


		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}


void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	
	ImGui::Image((ImTextureID)(intptr_t)coolPortal.framebuffer.colorBuffer[0], ImVec2(screenWidth, screenHeight));
	ImGui::Image((ImTextureID)(intptr_t)coolerAwesomePortal.framebuffer.colorBuffer[0], ImVec2(screenWidth, screenHeight));
	ImGui::Image((ImTextureID)(intptr_t)coolerAwesomePortal.framebuffer.depthBuffer, ImVec2(screenWidth, screenHeight));

	ImGui::Checkbox("Using Normal Map", &usingNormalMap);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

