#pragma once
#include "../ew/external/glad.h"
#include <GLFW/glfw3.h>

namespace tsa 
{
	struct Framebuffer
	{
		GLuint fbo;
		GLuint rbo;
		GLuint color0;
		GLuint brightness;
		GLuint depthColor;
	};

	Framebuffer createFrameBuffer(float width, float height, GLuint colorFiltering);
	Framebuffer createHDRFrameBuffer(float width, float height);
	Framebuffer createBloomHDRFrameBuffer(float width, float height);
}