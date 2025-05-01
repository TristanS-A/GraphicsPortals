#include <stdio.h>
#include <math.h>
#include <iostream>

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
#include <glm/gtc/quaternion.hpp>

#include <tuple>

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

ew::Model* pCoolSuzanne;
ew::Transform coolSuzanneTransform;
ew::Transform coolSuzanneTransformDup;

ew::Model* pCoolerSnazzySuzanne;
ew::Transform coolerSnazzySuzanneTransform;
ew::Transform coolerSnazzySuzanneTransformDup;

//island
ew::Model* pIsland; 
ew::Transform islandTrans;

ew::Model* pRecursiveSuzzane;
ew::Transform recursiveSuzzaneTransform;

bool usingNormalMap = true;

float clipRange1 = 5.f;
float clipRange2 = 10.f;

glm::vec2 colors = glm::vec2(0.4, 0.15);

typedef struct
{

	glm::vec3 highlight;
	glm::vec3 shadow;

}Palette;

static int palette_index = 0;
static std::vector<std::tuple<std::string, Palette>> palette{
	{"Sunny Day", {{1.00f, 1.00f, 1.00f}, {0.60f, 0.54f, 0.52f}}},
	{"Bright Night", {{0.47f, 0.58f, 0.68f}, {0.32f, 0.39f, 0.57f}}},
	{"Rainy Day", {{0.62f, 0.69f, 0.67f},{0.50f, 0.55f, 0.50f}}},
	{"Rainy Night", {{0.24f, 0.36f, 0.54f},{0.25f, 0.31f, 0.31f}}},
};

std::vector<GLuint> shdrTextures;
struct Portal 
{
public:
	Portal* linkedPortal;
	ew::Mesh portalMesh;
	ew::Transform regularPortalTransform;
	glm::vec3 virtualCameraRotOffset = glm::vec3(glm::radians(180.f), glm::radians(0.f), 0.0);
	glm::vec3 normal;
	tsa::FrameBuffer framebuffer;
	tsa::FrameBuffer theStupidFramebuffer;

	//function here becasue tristan is bad cringe and smells like carrots
  glm::mat4 const ObliqueClippingMat(glm::mat4& const viewMatrix, glm::mat4& const projectionMatrix, const ew::Transform& trans)
  {
    float d = glm::length(trans.position);

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
Portal recursivePortal1;
Portal recursivePortal2;
GLint rockNormal;
GLuint zaToon;

Portal portals[4];

ew::Mesh theCoolSphere;

float offset1 = 0;
float offset2 = 0;
void RenderScene(ew::Shader& shader, ew::Shader& portalShader, ew::Shader SceneShader, GLuint tex, glm::mat4 view)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glCullFace(GL_BACK);
	glBindTexture(GL_TEXTURE_2D, tex);

	glActiveTexture(GL_TEXTURE1);
	glActiveTexture(GL_TEXTURE2);


	SceneShader.use();

	//island
	SceneShader.setMat4("_Model", islandTrans.modelMatrix());
	
	SceneShader.setMat4("camera_viewProj", view);
	SceneShader.setInt("_MainTex", 0);
	SceneShader.setInt("zatoon", zaToon);
	SceneShader.setVec3("_Pallet.highlight", std::get<Palette>(palette[palette_index]).highlight);
	SceneShader.setVec3("_Pallet.shadow", std::get<Palette>(palette[palette_index]).shadow);


	pIsland->draw(shdrTextures, SceneShader);

	//first cool suzan
	shader.use();

	shader.setMat4("_Model", coolPortal.regularPortalTransform.modelMatrix());

	shader.setVec3("_CullPos", coolerAwesomePortal.regularPortalTransform.position);
	shader.setVec3("_CullNormal", coolerAwesomePortal.normal);

	shader.setMat4("camera_viewProj", view);
	shader.setInt("_MainTex", 0);

	shader.setMat4("_Model", coolSuzanneTransform.modelMatrix());
	shader.setVec3("_ColorOffset", glm::vec3(0, 0, 1));
	
	pCoolSuzanne->draw();
	//draw dup
	//add the offset
	shader.setMat4("_Model", coolSuzanneTransformDup.modelMatrix());
	shader.setVec3("_CullPos", coolPortal.regularPortalTransform.position);
	shader.setVec3("_CullNormal", coolPortal.normal);
	shader.setFloat("_ClipRange", clipRange1);
	pCoolSuzanne->draw();

	//draw other suzan
	shader.setMat4("_Model", coolerSnazzySuzanneTransform.modelMatrix());
	shader.setVec3("_CullPos", coolPortal.regularPortalTransform.position);
	shader.setVec3("_CullNormal", coolPortal.normal);
	shader.setVec3("_ColorOffset", glm::vec3(1, 0, 0));
	
	pCoolerSnazzySuzanne->draw();

	//draw dup
	shader.setMat4("_Model", coolerSnazzySuzanneTransformDup.modelMatrix());
	shader.setVec3("_CullPos", coolerAwesomePortal.regularPortalTransform.position);
	shader.setVec3("_CullNormal", coolerAwesomePortal.normal);
	shader.setFloat("_ClipRange", clipRange2);

	pCoolerSnazzySuzanne->draw();

	//draw portal frame?
	glCullFace(GL_FRONT);

	portalShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, coolPortal.framebuffer.colorBuffer[0]);
	portalShader.setMat4("_Model", coolPortal.regularPortalTransform.modelMatrix());
	portalShader.setMat4("camera_viewProj", view);
	portalShader.setInt("_MainTex", 0);
  portalShader.setFloat("_Time", (float)glfwGetTime());
	portalShader.setVec2("colors", colors);
	coolPortal.portalMesh.draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, coolerAwesomePortal.framebuffer.colorBuffer[0]);
	portalShader.setMat4("_Model", coolerAwesomePortal.regularPortalTransform.modelMatrix());
	coolerAwesomePortal.portalMesh.draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, recursivePortal1.framebuffer.colorBuffer[0]);
	portalShader.setMat4("camera_viewProj", view);
	portalShader.setMat4("_Model", recursivePortal1.regularPortalTransform.modelMatrix());
	recursivePortal1.portalMesh.draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, recursivePortal2.framebuffer.colorBuffer[0]);
	portalShader.setMat4("_Model", recursivePortal2.regularPortalTransform.modelMatrix());
	recursivePortal2.portalMesh.draw();
}

void DrawRecursivePortals(Portal portalToRender, glm::mat4& const viewMat, glm::mat4& const projMat, int maxRecursion, int currentRecursion, ew::Shader& sceneShader, ew::Shader& portalShader, int t)
{
	//Adds an offset to get the correct virtual camera rotation (not just the portals rotation)
	ew::Transform linkedPTrans = portalToRender.linkedPortal->regularPortalTransform;
	linkedPTrans.rotation = glm::vec3(glm::eulerAngles(linkedPTrans.rotation) + portalToRender.linkedPortal->virtualCameraRotOffset);

	//Get the virtual camera view matrix (rotates camera by current portal rotation and then linked portal rotation)
	glm::mat4 destinationView =
		viewMat * portalToRender.regularPortalTransform.modelMatrix()
		* glm::rotate(glm::mat4(1.0f), glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::inverse(linkedPTrans.modelMatrix());

	// Base case, render inside of inner portal
	if (currentRecursion == maxRecursion)
	{
		//bind the portal frame buffer then draw the scene with color attachemnt
		glBindFramebuffer(GL_FRAMEBUFFER, portalToRender.framebuffer.fbo);
		RenderScene(sceneShader, portalShader, rockNormal, projMat * destinationView);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, portalToRender.framebuffer.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, portalToRender.theStupidFramebuffer.fbo);

		glReadBuffer(portalToRender.framebuffer.fbo);
		glDrawBuffer(portalToRender.theStupidFramebuffer.fbo);
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		// Recursion case
		// Pass our new view matrix and the clipped projection matrix (see above)
		DrawRecursivePortals(portalToRender, destinationView, projMat, maxRecursion, currentRecursion + 1, sceneShader, portalShader, t);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, portalToRender.framebuffer.fbo);
	RenderScene(sceneShader, portalShader, rockNormal, camera.projectionMatrix() * destinationView);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, portalToRender.framebuffer.fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, portalToRender.theStupidFramebuffer.fbo);

	glReadBuffer(portalToRender.framebuffer.fbo);
	glDrawBuffer(portalToRender.theStupidFramebuffer.fbo);
	glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPortalView(Portal& p, ew::Shader& sceneShader, ew::Shader& portalShader, ew::Shader SceneShader)
{
	//calcualte cam
	ew::Camera portal = camera;

	/*glm::mat4 destinationView =
		camera.viewMatrix() * p.regularPortalTransform.modelMatrix()
		* glm::rotate(glm::mat4(1.0f), glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::inverse(p.linkedPortal->regularPortalTransform.modelMatrix());*/

			//Translates camera position and target in relation to linked portal. This only works for specific portal rotations
	glm::vec3 toPortal = p.regularPortalTransform.position - camera.position;
	glm::vec3 translatedTarget = p.regularPortalTransform.position - camera.target;
	portal.position = p.linkedPortal->regularPortalTransform.position - toPortal;
	portal.target = p.linkedPortal->regularPortalTransform.position - translatedTarget;

	//Adds an offset to get the correct virtual camera rotation (not just the portals rotation)
	ew::Transform linkedPTrans = p.linkedPortal->regularPortalTransform;
	linkedPTrans.rotation = glm::vec3(glm::eulerAngles(linkedPTrans.rotation) + p.virtualCameraRotOffset);

	//Get the virtual camera view matrix (rotates camera by current portal rotation and then linked portal rotation)
	glm::mat4 destinationView =
		camera.viewMatrix() * p.regularPortalTransform.modelMatrix()
		* glm::rotate(glm::mat4(1.0f), glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::inverse(linkedPTrans.modelMatrix());

	////A reminder that late night quick maths are not always going to be scalable :(
	/*glm::quat roation = glm::vec3(glm::radians(-90.f), 0, 0);
	glm::mat4 rotMatrix = glm::mat4_cast(roation);

	glm::vec4 rotatedPosition = rotMatrix * glm::vec4(portal.position - p.linkedPortal->regularPortalTransform.position, 1);
	glm::vec4 rotatedTarget = rotMatrix * glm::vec4(portal.target - p.linkedPortal->regularPortalTransform.position, 1);

	portal.position = glm::vec3(rotatedPosition) + p.linkedPortal->regularPortalTransform.position;
	portal.target = glm::vec3(rotatedTarget) + p.linkedPortal->regularPortalTransform.position;*/

	glBindFramebuffer(GL_FRAMEBUFFER, p.framebuffer.fbo);
	{
		glEnable(GL_DEPTH_TEST);
		RenderScene(sceneShader, portalShader,SceneShader, rockNormal, camera.projectionMatrix() * destinationView);

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
	ew::Shader SceneShader = ew::Shader("assets/SceneShader.vert", "assets/SceneShader.frag");

	pCoolSuzanne = new ew::Model("assets/suzanne.obj");
	pCoolerSnazzySuzanne = new ew::Model("assets/suzanne.obj");
	pRecursiveSuzzane = new ew::Model("assets/suzanne.obj");

	theCoolSphere = ew::createSphere(3, 10);

	coolPortal.portalMesh = ew::createPlane(5, 5, 10);
	coolPortal.regularPortalTransform.position = glm::vec3(0, 0, 0);
  
	//coolPortal.portalMesh = ew::createPlane(5,5, 10);
	//coolPortal.regularPortalTransform.position = glm::vec3(0, 1, 0);

  coolPortal.regularPortalTransform.rotation = glm::vec3(glm::radians(90.f), glm::radians(180.f), 0);
	coolPortal.linkedPortal = &coolerAwesomePortal;
	coolPortal.framebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);

	//set the normals for the object clipping
	coolPortal.normal = glm::vec3(0, 1, 0);
	coolPortal.normal = coolPortal.regularPortalTransform.rotation * coolPortal.normal;

	coolerAwesomePortal.portalMesh = ew::createPlane(5, 5, 10);
	coolerAwesomePortal.regularPortalTransform.position = glm::vec3(10, 0, 0);
	coolerAwesomePortal.regularPortalTransform.rotation = glm::vec3(glm::radians(0.f), 0, 0);
	coolerAwesomePortal.framebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);
	coolerAwesomePortal.linkedPortal = &coolPortal;

  //set the normals for object clipping
	coolerAwesomePortal.normal = glm::vec3(0, 1, 0);
	coolerAwesomePortal.normal = coolerAwesomePortal.regularPortalTransform.rotation * coolerAwesomePortal.normal;
  
  recursivePortal1.portalMesh = ew::createPlane(5, 5, 10);
	recursivePortal1.regularPortalTransform.position = glm::vec3(-10, 20, -3);
	recursivePortal1.regularPortalTransform.rotation = glm::vec3(glm::radians(90.0f), 0, 0);
	recursivePortal1.framebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);
	recursivePortal1.theStupidFramebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);
	recursivePortal1.linkedPortal = &recursivePortal2;

	recursivePortal2.portalMesh = ew::createPlane(5, 5, 10);
	recursivePortal2.regularPortalTransform.position = glm::vec3(-10, 20, -9);
	recursivePortal2.regularPortalTransform.rotation = glm::vec3(glm::radians(200.0f), 0, 0);
	recursivePortal2.framebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);
	recursivePortal2.theStupidFramebuffer = tsa::createHDR_FramBuffer(screenWidth, screenHeight);
	recursivePortal2.linkedPortal = &recursivePortal1;

	GLint Rock_Color = ew::loadTexture("assets/Rock_Color.png");
	rockNormal = ew::loadTexture("assets/Rock_Normal.png");

	coolSuzanneTransform.position = glm::vec3(10, -2, 0);
	coolSuzanneTransformDup.position = glm::vec3(0, 1, -2);

	coolerSnazzySuzanneTransform.position = glm::vec3(0, 0, 7);
	recursiveSuzzaneTransform.position = glm::vec3(-10, 20, -6);
	coolerSnazzySuzanneTransformDup.position = glm::vec3(10, 7, 0);
  
  portals[0] = recursivePortal1;
	portals[1] = recursivePortal2;
	portals[2] = coolPortal;
	portals[3] = coolerAwesomePortal;
	
	//maybe try to get dynamic locations
	coolerSnazzySuzanneTransformDup.rotation = glm::vec3(glm::radians(270.f), 0, 0);
	coolSuzanneTransformDup.rotation = glm::vec3(glm::radians(90.f), 0, 0);

	//draw island
	pIsland = new ew::Model("assets/island/Island.obj");

	islandTrans.position = glm::vec3(0,0,-200);
	islandTrans.scale = glm::vec3(0.01);

	//load textures
	std::string path = "assets/";

	shdrTextures.push_back(ew::loadTexture((path + "island/OutsSS00.png").c_str()));
	shdrTextures.push_back(ew::loadTexture((path + "island/OutsMM03.png").c_str()));
	shdrTextures.push_back(ew::loadTexture((path + "island/OutsMM02.png").c_str()));
	shdrTextures.push_back(ew::loadTexture((path + "island/OutsSS01.png").c_str()));
	shdrTextures.push_back(ew::loadTexture((path + "island/OutsSS05.png").c_str()));
	shdrTextures.push_back(ew::loadTexture((path + "island/OutsSS04.png").c_str()));
	shdrTextures.push_back(ew::loadTexture((path + "island/OutsSS07.png").c_str()));
	shdrTextures.push_back(ew::loadTexture((path + "island/OutsSS06.png").c_str()));

	zaToon = ew::loadTexture("ZAToon.png");

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//RENDER
		camController.move(window, &camera, deltaTime);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// GFX Pass
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.6f, 0.8f, 0.92f, 1.0f);

		for (int i = 0; i < 4; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, portals[i].framebuffer.fbo);

			// GFX Pass
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.6f, 0.8f, 0.92f, 1.0f);

			glBindFramebuffer(GL_FRAMEBUFFER, portals[i].theStupidFramebuffer.fbo);

			// GFX Pass
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		}

		RenderPortalView(coolPortal, defaultLit, portalView, SceneShader);
		RenderPortalView(coolerAwesomePortal, defaultLit, portalView, SceneShader);
		RenderScene(defaultLit, portalView, SceneShader, rockNormal, camera.projectionMatrix() * camera.viewMatrix());

		//DrawRecursivePortals(camera.viewMatrix(), camera.projectionMatrix(), 5, 0, defaultLit, portalView);
		DrawRecursivePortals(recursivePortal1, camera.viewMatrix(), camera.projectionMatrix(), 10, 0, defaultLit, portalView, 1);
		DrawRecursivePortals(recursivePortal2, camera.viewMatrix(), camera.projectionMatrix(), 10, 0, defaultLit, portalView, 0);
	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		RenderScene(defaultLit, portalView, SceneShader, rockNormal, camera.projectionMatrix() * camera.viewMatrix());

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

	//ImGui::Image((ImTextureID)(intptr_t)coolPortal.framebuffer.colorBuffer[0], ImVec2(screenWidth, screenHeight));
	//ImGui::Image((ImTextureID)(intptr_t)coolerAwesomePortal.framebuffer.colorBuffer[0], ImVec2(screenWidth, screenHeight));
	//ImGui::Image((ImTextureID)(intptr_t)coolerAwesomePortal.framebuffer.depthBuffer, ImVec2(screenWidth, screenHeight));
	ImGui::Image((ImTextureID)(intptr_t)recursivePortal1.framebuffer.colorBuffer[0], ImVec2(screenWidth, screenHeight));
	ImGui::Image((ImTextureID)(intptr_t)recursivePortal1.theStupidFramebuffer.colorBuffer[0], ImVec2(screenWidth, screenHeight));
	ImGui::Image((ImTextureID)(intptr_t)recursivePortal2.framebuffer.colorBuffer[0], ImVec2(screenWidth, screenHeight));
	ImGui::Image((ImTextureID)(intptr_t)recursivePortal2.theStupidFramebuffer.colorBuffer[0], ImVec2(screenWidth, screenHeight));


	if (ImGui::Button("coolerPortalMonkey UP"))
	{
		coolSuzanneTransform.position.y += 1;
		coolSuzanneTransformDup.position.z += 1;
	}
	if (ImGui::Button("coolerPortalMonkey DOWN"))
	{
		coolSuzanneTransform.position.y -= 1;
		coolSuzanneTransformDup.position.z -= 1;
	}
	if (ImGui::Button("AwsomePortalMonkey BACK"))
	{
		coolerSnazzySuzanneTransform.position.z -= 1;
		coolerSnazzySuzanneTransformDup.position.y -= 1;
	}
	if (ImGui::Button("AwsomePortalMonkey FORWARD"))
	{
		coolerSnazzySuzanneTransform.position.z += 1;
		coolerSnazzySuzanneTransformDup.position.y += 1;
	}
	ImGui::SliderFloat2("Colors", &colors.x, 0.1, 1);
	ImGui::SliderFloat("Portal 1 Clip Range", &clipRange1, 0.1, 20);
	ImGui::SliderFloat("Portal w Clip Range", &clipRange2, 0.1, 20);

	if (ImGui::BeginCombo("Palette", std::get<std::string>(palette[palette_index]).c_str()))
	{
		for (auto n = 0; n < palette.size(); ++n)
		{
			auto is_selected = (std::get<0>(palette[palette_index]) == std::get<0>(palette[n]));
			if (ImGui::Selectable(std::get<std::string>(palette[n]).c_str(), is_selected))
			{
				palette_index = n;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::ColorEdit3("Highlight", &std::get<Palette>(palette[palette_index]).highlight[0]);
	ImGui::ColorEdit3("Shadow", &std::get<Palette>(palette[palette_index]).shadow[0]);
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