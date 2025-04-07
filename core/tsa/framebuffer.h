#pragma once
#include "../ew/external/glad.h"
#include <GLFW/glfw3.h>

namespace tsa 
{
	enum DepthType
	{
		RENDER_BUFFER, TEXTURE
	};
	struct FrameBuffer
	{
		unsigned int fbo;
		unsigned int colorBuffer[8];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int hight;
	};

	FrameBuffer createFrameBuffer(unsigned int width, unsigned int hight, unsigned int colorFormat, DepthType type = RENDER_BUFFER);
	FrameBuffer createHDR_FramBuffer(unsigned int width, unsigned int hight);
	FrameBuffer createShadowBuffer(unsigned int shdw_width, unsigned int shdw_hight);
}