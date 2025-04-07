#include "framebuffer.h"

namespace tsa
{
	Framebuffer createFrameBuffer(float width, float height, GLuint colorFiltering)
	{
		Framebuffer frameBuffer;
		
		//Bind framebuffer
		glGenFramebuffers(1, &frameBuffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);

		//Create color texture attachment
		glGenTextures(1, &frameBuffer.color0);
		glBindTexture(GL_TEXTURE_2D, frameBuffer.color0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, colorFiltering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, colorFiltering);

		//Bind color attachment
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.color0, 0);

		//Create depth texture attachment
		glGenTextures(1, &frameBuffer.depthColor);
		glBindTexture(GL_TEXTURE_2D, frameBuffer.depthColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//Bind depth texture attachment
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, frameBuffer.depthColor, 0);

		//Generate and bind rbo
		/*glGenRenderbuffers(1, &framebuffer.depthColor);
		glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depthColor);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depthColor);*/

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return frameBuffer;
	}
	
	Framebuffer createHDRFrameBuffer(float width, float height)
	{
		Framebuffer frameBuffer;

		//Bind framebuffer
		glGenFramebuffers(1, &frameBuffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);

		//Create color texture attachment
		glGenTextures(1, &frameBuffer.color0);
		glBindTexture(GL_TEXTURE_2D, frameBuffer.color0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Bind color attachment
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.color0, 0);

		//Create depth texture attachment
		glGenTextures(1, &frameBuffer.depthColor);
		glBindTexture(GL_TEXTURE_2D, frameBuffer.depthColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//Bind depth texture attachment
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, frameBuffer.depthColor, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return frameBuffer;
	}

	Framebuffer createBloomHDRFrameBuffer(float width, float height)
	{
		Framebuffer frameBuffer;

		//Bind framebuffer
		glGenFramebuffers(1, &frameBuffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);

		//Create color texture attachment
		glGenTextures(1, &frameBuffer.color0);
		glBindTexture(GL_TEXTURE_2D, frameBuffer.color0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Create brightness texture attachment
		glGenTextures(1, &frameBuffer.brightness);
		glBindTexture(GL_TEXTURE_2D, frameBuffer.brightness);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Bind color attachment
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.color0, 0);

		//Bind brightness attachment
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, frameBuffer.brightness, 0);

		GLuint arr[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, arr);

		//Create depth texture attachment
		glGenTextures(1, &frameBuffer.depthColor);
		glBindTexture(GL_TEXTURE_2D, frameBuffer.depthColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//Bind depth texture attachment
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, frameBuffer.depthColor, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return frameBuffer;
	}
}
