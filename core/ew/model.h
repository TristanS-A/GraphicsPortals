/*
*	Author: Eric Winebrenner
*/

#pragma once
#include "mesh.h"
#include "shader.h"
#include <vector>
#include "external/glad.h"

namespace ew {
	class Model {
	public:
		Model(const std::string& filePath);
		void draw();
		void draw(std::vector<GLuint>& path, ew::Shader& shdr);
	private:
		std::vector<ew::Mesh> m_meshes;
	};
}