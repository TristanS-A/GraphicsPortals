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
#include <glm/gtc/matrix_access.hpp>

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

		//function here becasue tristan is bad cringe and smells like carrots

		glm::mat4 const ObliqueClippingMat(glm::mat4& const viewMatrix, glm::mat4& const projectionMatrix, const ew::Transform& trans)
		{
			float d = glm::length(trans.position);

			glm::vec3 newClipPlaneNormal = trans.rotation * glm::vec3(0.0, 0.0, -1.0);
			
			glm::vec4 newClipPlane(newClipPlaneNormal, d);

			newClipPlane = glm::inverse(glm::transpose(viewMatrix)) * newClipPlane;
			//newClipPlane = glm::inverse(glm::transpose(viewMat)) * newClipPlane;

			if (newClipPlane.w > 0.0f)
			{
				return projectionMatrix;
			}

			// Far plane
			glm::vec4 q = glm::inverse(projectionMatrix) * glm::vec4(glm::sign(newClipPlane.x), glm::sign(newClipPlane.y), 1.0f, 1.0f);

			//scales new matrix by the angle so that it fits in the orginal frustum 
			glm::vec4 c = newClipPlane * (2.0f / (glm::dot(newClipPlane, q)));

			glm::mat4 newProjMat = projectionMatrix; 

			//replace the clipping plane 
			newProjMat = glm::row(newProjMat, 2, c - glm::row(newProjMat, 3));

			return newProjMat;
		}
};

Portal coolPortal;
Portal coolerAwesomePortal;
GLint rockNormal;

ew::Mesh theCoolSphere;
void renderScene(ew::Shader& shader, ew::Shader& portalShader, GLuint tex, glm::mat4 view)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, coolPortal.framebuffer.colorBuffer[0]);

	//GFX Pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);


	portalShader.use();

	portalShader.setMat4("_Model", coolPortal.regularPortalTransform.modelMatrix());
	portalShader.setMat4("camera_viewProj", view);
	portalShader.setInt("_MainTex", 0);

	coolPortal.portalMesh.draw();
	
	glBindTexture(GL_TEXTURE_2D, coolerAwesomePortal.framebuffer.colorBuffer[0]);
	portalShader.setMat4("_Model", coolerAwesomePortal.regularPortalTransform.modelMatrix());
	coolerAwesomePortal.portalMesh.draw();
	

	//shader.setMat4("_Model", glm::translate(glm::vec3(10, 0, 0)));
	//theCoolSphere.draw();


	glCullFace(GL_BACK);
	glBindTexture(GL_TEXTURE_2D, tex);

	shader.use();
	shader.setMat4("_Model", coolPortal.regularPortalTransform.modelMatrix());
	shader.setMat4("camera_viewProj", view);
	shader.setInt("_MainTex", 0);
	shader.setMat4("_Model", suzanneTransform.modelMatrix());
	pSuzanne->draw();

	
}

void RenderPortalView(Portal& p, ew::Shader& sceneShader, ew::Shader& portalShader)
{
	//calcualte cam
	ew::Camera portal = camera;

	/*glm::mat4 destinationView =
		camera.viewMatrix() * p.regularPortalTransform.modelMatrix()
		* glm::rotate(glm::mat4(1.0f), glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::inverse(p.linkedPortal->regularPortalTransform.modelMatrix());*/

	glm::vec3 toPortal = p.regularPortalTransform.position - camera.position;
	glm::vec3 translatedTarget = p.regularPortalTransform.position - camera.target;
	portal.position = p.linkedPortal->regularPortalTransform.position - toPortal;
	portal.target = p.linkedPortal->regularPortalTransform.position - translatedTarget;

	glBindFramebuffer(GL_FRAMEBUFFER, p.framebuffer.fbo);
	{
		glEnable(GL_DEPTH_TEST);
		renderScene(sceneShader, portalShader, rockNormal, portal.projectionMatrix() * portal.viewMatrix());

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
	ew::Shader portalView = ew::Shader("assets/Portal/PortalView.vert", "assets/Portal/PortalView.frag");

	pSuzanne = new ew::Model("assets/suzanne.obj");

	theCoolSphere = ew::createSphere(3, 10);

	coolPortal.portalMesh = ew::createPlane(5,5, 10);
	coolPortal.regularPortalTransform.position = glm::vec3(0, 0, 0);
	coolPortal.regularPortalTransform.rotation = glm::vec3(glm::radians(90.f), glm::radians(180.f), 0);
	coolPortal.linkedPortal = &coolerAwesomePortal;
	coolPortal.framebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);

	coolerAwesomePortal.portalMesh = ew::createPlane(5, 5, 10);
	coolerAwesomePortal.regularPortalTransform.position = glm::vec3(10, 0, 0);
	coolerAwesomePortal.regularPortalTransform.rotation = glm::vec3(glm::radians(90.f), 0, 0);
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
		RenderPortalView(coolPortal, defaultLit, portalView);
		RenderPortalView(coolerAwesomePortal, defaultLit, portalView);
		renderScene(defaultLit, portalView, rockNormal, camera.projectionMatrix() * camera.viewMatrix());


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

