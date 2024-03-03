#pragma once

#include "Renderer.h"
#include <GLFW/stb_image.h>

class CubeMapTexture
{
public : 

	

	CubeMapTexture();
	~CubeMapTexture();

	unsigned int textureId;

	void Bind();
	void UnBind();
	void SetActiveTexture(int id);
	void LoadTexture(std::vector<std::string> faces);

	bool IsFileExists(const std::string& path);
};

