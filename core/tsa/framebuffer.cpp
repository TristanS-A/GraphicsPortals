#include "framebuffer.h"
#include <stdio.h>
namespace tsa
{
	FrameBuffer createFrameBuffer(unsigned int width, unsigned int hight, unsigned int colorFormat, DepthType type)
	{
		FrameBuffer buffer;
		buffer.width = width;
		buffer.hight = hight;

		//buffer code
		glGenFramebuffers(1, &buffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);

		//create color
		glGenTextures(1, &buffer.colorBuffer[0]);

		glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, hight, 0, colorFormat, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



		if (type == RENDER_BUFFER)
		{
			//create depth
			glGenRenderbuffers(1, &buffer.depthBuffer);

			glBindRenderbuffer(GL_RENDERBUFFER, buffer.depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, hight);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);


			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer.depthBuffer);
		}
		else if (type == TEXTURE)
		{
			/*glGenTextures(1, &buffer.depthBuffer);

			glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, hight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);*/

			glGenTextures(1, &buffer.depthBuffer);

			glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, hight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);

			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.colorBuffer[0], 0);


		}

		//attach buffers	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.colorBuffer[0], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("frame buffer is not complete!");
			return FrameBuffer();

		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return buffer;
	}

	FrameBuffer createHDR_FramBuffer(unsigned int width, unsigned int hight)
	{

		FrameBuffer buffer;
		buffer.width = width;
		buffer.hight = hight;

		//buffer code
		glGenFramebuffers(1, &buffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);

		//create color
		glGenTextures(1, &buffer.colorBuffer[0]);

		glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, hight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.colorBuffer[0], 0);

		//create brightness
		glGenTextures(1, &buffer.colorBuffer[1]);

		glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, hight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//attach buffers	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, buffer.colorBuffer[1], 0);

		//create brightness
		glGenTextures(1, &buffer.colorBuffer[2]);

		glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, hight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//attach buffers	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, buffer.colorBuffer[2], 0);

		//create brightness
		glGenTextures(1, &buffer.colorBuffer[3]);

		glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[3]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, hight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//attach buffers	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, buffer.colorBuffer[3], 0);

		//create brightness
		glGenTextures(1, &buffer.colorBuffer[4]);

		glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[4]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, hight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//attach buffers	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, buffer.colorBuffer[4], 0);

		GLuint att[5] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(5, att);

		glGenTextures(1, &buffer.depthBuffer);

		glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, hight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("frame buffer is not complete!");
			return FrameBuffer();

		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return buffer;
	}

	FrameBuffer createShadowBuffer(unsigned int shdw_width, unsigned int shdw_hight)
	{
		FrameBuffer buffer;

		glGenFramebuffers(1, &buffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);

		//create and bind depth
		glGenTextures(1, &buffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			shdw_width, shdw_hight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//fixing the boarder color
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("frame buffer is not complete!");
			return FrameBuffer();

		}
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return buffer;

	}

}
