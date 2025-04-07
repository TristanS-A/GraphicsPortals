#pragma once
#include <stdio.h>
#include <math.h>
#include "../ew/shader.h"

namespace tsa
{
	struct PPShaderData 
	{
		ew::Shader shaderProgram;
		char* displayName = "Default";

		PPShaderData() {};

		virtual void display(Framebuffer framebuffer) 
		{
			shaderProgram.use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, framebuffer.color0);
			shaderProgram.setInt("_MainTex", 0);
		}

		virtual void ImGuiDisplay() {}
	};

	struct FogShaderData : PPShaderData
	{
		float nearPlane = 0;
		float farPlane = 0;
		glm::vec3 fogcolor = glm::vec3(0.0, 0.0, 0.0);

		FogShaderData() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, framebuffer.depthColor);
			shaderProgram.setInt("_DepthTex", 1);

			shaderProgram.setFloat("_Near", nearPlane);
			shaderProgram.setFloat("_Far", farPlane);
			shaderProgram.setVec3("_FogColor", fogcolor);
		}

		void ImGuiDisplay() override
		{

			ImGui::SliderFloat("Fog Color Red", &fogcolor.r, 0.0, 1.0);
			ImGui::SliderFloat("Fog Color Green", &fogcolor.g, 0.0, 1.0);
			ImGui::SliderFloat("Fog Color Blue", &fogcolor.b, 0.0, 1.0);
		}
	};

	struct EdgeDetectionData : PPShaderData
	{
		float strength = 16;

		EdgeDetectionData() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);
			shaderProgram.setFloat("_Strength", strength);
		}

		void ImGuiDisplay() override
		{

			ImGui::SliderFloat("Strength", &strength, 0.0, 100);
		}
	};

	struct ChromaticAborationData : PPShaderData
	{
		glm::vec3 offset = glm::vec3(-0.006, 0.006, -0.006);
		glm::vec2 direction = glm::vec2(1.0, 0.0);

		ChromaticAborationData() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);
			shaderProgram.setVec3("_Offset", offset);
			shaderProgram.setVec2("_Direction", direction);
		}

		void ImGuiDisplay() override
		{
			ImGui::SliderFloat("X Offset", &offset.x, -1.0, 1.0);
			ImGui::SliderFloat("Y Offset", &offset.y, -1.0, 1.0);
			ImGui::SliderFloat("Z Offset", &offset.z, -1.0, 1.0);
			ImGui::SliderFloat("X Direction", &direction.x, -1.0, 1.0);
			ImGui::SliderFloat("Y Direction", &direction.y, -1.0, 1.0);
		}
	};

	struct BlurrData : PPShaderData
	{
		float strength = 300;

		BlurrData() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);
			shaderProgram.setFloat("_InvStrength", strength);
		}

		void ImGuiDisplay() override
		{

			ImGui::SliderFloat("Inverse Blurr Strength", &strength, 0.0, 500);
		}
	};

	struct GaussenBlurrData : PPShaderData
	{
		float strength = 300;

		GaussenBlurrData() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);
			shaderProgram.setFloat("_InvStrength", strength);
		}

		void ImGuiDisplay() override
		{
			ImGui::SliderFloat("Inverse Blurr Strength", &strength, 0.0, 500);
		}
	};

	struct GrayScaleData : PPShaderData
	{
		bool realistic = true;

		GrayScaleData() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);

			if (realistic) 
			{
				shaderProgram.setVec3("_RealisticScale", glm::vec3(0.2126f, 0.7152f, 0.0722f));
				shaderProgram.setFloat("_AvarageScale", 1.0f);
			}
			else 
			{
				shaderProgram.setVec3("_RealisticScale", glm::vec3(1.0f, 1.0f, 1.0f));
				shaderProgram.setFloat("_AvarageScale", 3.0f);
			}

		}

		void ImGuiDisplay() override
		{

			ImGui::Checkbox("Use Realistic Grayscale", &realistic);
		}
	};

	struct InverseData : PPShaderData
	{
		float invAmount = 1.0;

		InverseData() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);

			shaderProgram.setFloat("_InverseSlider", invAmount);

		}

		void ImGuiDisplay() override
		{

			ImGui::SliderFloat("Inverse Amount", &invAmount, 0.0f, 1.0f);
		}
	};

	struct VignetteData : PPShaderData
	{
		float radius = 0.3;
		float smooth = 0.2;

		VignetteData() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);

			shaderProgram.setFloat("_Radius", radius);
			shaderProgram.setFloat("_Smooth", smooth);

		}

		void ImGuiDisplay() override
		{

			ImGui::SliderFloat("Radius", &radius, 0.0f, 1.0f);
			ImGui::SliderFloat("Cuttoff Smoothing", &smooth, 0.0f, 1.0f);
		}
	};

	struct CRTShader : PPShaderData
	{
		float time = 0.0;
		float scrollSpeed = 0.2;
		float scanLineHeight = 0.015;
		float scanlineIntensity = 0.01;
		float grainIntensity = 0.2;
		float affectIntensity = 0.2f;

		CRTShader() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);

			time = glfwGetTime();
			shaderProgram.setFloat("_Time", time);
			shaderProgram.setFloat("_ScrollSpeed", scrollSpeed);
			shaderProgram.setFloat("_ScanlineHeight", scanLineHeight);
			shaderProgram.setFloat("_ScanlineIntensity", scanlineIntensity);
			shaderProgram.setFloat("_GrainIntensity", grainIntensity);
			shaderProgram.setFloat("_AffectIntensity", affectIntensity);

		}

		void ImGuiDisplay() override
		{

			ImGui::SliderFloat("Scroll Speed", &scrollSpeed, 0.0f, 10.0f);
			ImGui::SliderFloat("Scanline Height", &scanLineHeight, 0.0f, 1.0f);
			ImGui::SliderFloat("Scanline Intensity", &scanlineIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Grain Intensity", &grainIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Affect Intensity", &affectIntensity, 0.0f, 1.0f);
		}
	};

	struct PSXData : PPShaderData
	{
		glm::vec2 psxRes = glm::vec2(640, 480);
		glm::vec2 newRes = glm::vec2(1080, 720);
		float scrollSpeed = 0.2;

		PSXData() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);

			shaderProgram.setVec2("_PSXResolution", psxRes);
			shaderProgram.setVec2("_NewResolution", newRes);
		}
	};

	struct HDRData : PPShaderData
	{
		float gammma = 2.2;
		float exposure = 1.0;

		HDRData() : PPShaderData() {};

		void display(Framebuffer framebuffer) override
		{
			PPShaderData::display(framebuffer);

			shaderProgram.setFloat("_Gamma", gammma);
			shaderProgram.setFloat("_Exposure", exposure);

		}

		void ImGuiDisplay() override
		{

			ImGui::SliderFloat("Gamma", &gammma, 0.0f, 10.0f);
			ImGui::SliderFloat("EXposure", &exposure, 0.0f, 20.0f);
		}
	};

	struct BloomData
	{
		float gammma = 2.2;
		float exposure = 4.0;
		int blurrCycles = 10;
		int blurrInvStrength = 300;

		ew::Shader shaderProgram;

		void loadAdditionalImGui()
		{
			shaderProgram.setFloat("_Gamma", gammma);
			shaderProgram.setFloat("_Exposure", exposure);
		}

		void ImGuiDisplay()
		{
			ImGui::SliderFloat("Gamma", &gammma, 0.0f, 10.0f);
			ImGui::SliderFloat("Exposure", &exposure, 0.0f, 20.0f);
			ImGui::SliderInt("Blurr Cycles", &blurrCycles, 1, 100);
			ImGui::SliderInt("Blurr Inverse Strength", &blurrInvStrength, 1, 1000);
		}
	};
}