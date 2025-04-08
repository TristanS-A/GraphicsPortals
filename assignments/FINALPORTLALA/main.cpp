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

struct Material
{
	float ambientK = 1.0;
	float diffuseK = 0.5;
	float specularK = 0.5;
	float shininess = 128;
	char* name = "Default";
};

enum Materials {
	METAL,
	ROCK,
	PLASTIC
};

//Caching things
ew::Camera camera;
ew::CameraController camController;
ew::Transform suzanneTransform;
Material mats[3] = { 
	{1.0, 0.6, 1.0, 30, "Metal"},
	{1.0, 0.8, 0.3, 128, "Rock"},
	{1.0, 0.8, 0.4, 30, "Plastic"}
};


short matIndex = 0;
Material* currMat = &mats[matIndex];
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


void thing(ew::Shader shader, ew::Model &model, ew::Transform &modelTransform, GLint tex, GLint normalMap, const float dt)
{
	//Pipeline defenitions
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	//GFX Pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);

	shader.use();
	modelTransform.rotation = glm::rotate(modelTransform.rotation, dt, glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("_Model", modelTransform.modelMatrix());
	shader.setMat4("camera_viewProj", camera.projectionMatrix() * camera.viewMatrix());
	shader.setInt("_MainTex", 0);
	shader.setBool("_Use_NormalMap", usingNormalMap);
	shader.setVec3("_EyePos", camera.position);

	shader.setFloat("_Material.ambientK", currMat->ambientK);
	shader.setFloat("_Material.diffuseK", currMat->diffuseK);
	shader.setFloat("_Material.specularK", currMat->specularK);
	shader.setFloat("_Material.shininess", currMat->shininess);

	model.draw();
}

void renderPortal(ew::Shader& shader, GLuint tex, glm::mat4 view)
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

		renderPortal(sceneShader, rockNormal, destinationView);

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

	ew::Model suzanne = ew::Model("assets/suzanne.obj");

	coolPortal.portalMesh = ew::createCube(5);
	coolPortal.regularPortalTransform.position = {0, 0, 0};
	coolPortal.linkedPortal = &coolerAwesomePortal;
	coolPortal.framebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);

	coolerAwesomePortal.portalMesh = ew::createCube(5);
	coolerAwesomePortal.regularPortalTransform.position = {10, 0, 0};
	coolerAwesomePortal.framebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);
	coolerAwesomePortal.linkedPortal = &coolPortal;

	GLint Rock_Color = ew::loadTexture("assets/Rock_Color.png");
	rockNormal = ew::loadTexture("assets/Rock_Normal.png");

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
		renderPortal(defaultLit, rockNormal, camera.projectionMatrix() * camera.viewMatrix());
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
	if (ImGui::BeginCombo("Select Material", currMat->name))
	{
		for (int i = 0; i < sizeof(mats) / sizeof(mats[0]); i++)
		{
			bool is_selected = (currMat->name == mats[i].name);
			if (ImGui::Selectable(mats[i].name, is_selected))
			{
				currMat = &mats[i];
			}
		}
		ImGui::EndCombo();
	}
	
	ImGui::Image((ImTextureID)(intptr_t)coolPortal.framebuffer.colorBuffer[0], ImVec2(screenWidth, screenHeight));
	ImGui::Image((ImTextureID)(intptr_t)coolerAwesomePortal.framebuffer.colorBuffer[0], ImVec2(screenWidth, screenHeight));

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

