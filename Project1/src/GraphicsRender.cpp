#include "GraphicsRender.h"

GraphicsRender::GraphicsRender()
{
}

GraphicsRender::~GraphicsRender()
{
	ClearData();	
}





void GraphicsRender::AddModelsAndShader(Model& model, Shader& Shader)
{
	m_Models.push_back(&model);
	m_Shaders.push_back(&Shader);
}

void GraphicsRender::AddModelAndShader(Model* model, Shader* shader)
{
	if (shader->blendMode ==OPAQUE || shader->blendMode == ALPHA_CUTOUT || shader->blendMode==SOLID)
	{
		modelAndShaderList.push_back(new ModelAndShader(model, shader));
	}
	else if (shader->blendMode == ALPHA_BLEND)
	{
		transparentmodelAndShaderList.push_back(new ModelAndShader(model, shader));
	}
	
}

void GraphicsRender::AddTransparentModels(Model* model, Shader* Shader)
{
	m_transparentModels.push_back(model);
	m_transparentShaders.push_back(Shader);

}

void GraphicsRender::AssignStencilShader(Shader* Shader)
{
	this->m_StencilShader = Shader;


}

void GraphicsRender::AssignCamera(Camera* cam)
{

	this->cam = cam;
}

void GraphicsRender::SortObject()
{
	CompareDistances compareDistance(cam->transform.position);
	std::sort(transparentmodelAndShaderList.begin(), transparentmodelAndShaderList.end(), compareDistance);


}

std::vector<Model*> GraphicsRender::GetModelList()
{

	return m_Models;
}




void GraphicsRender::Draw()
{


	glStencilMask(0x00);

	for (ModelAndShader* modelAndShader : modelAndShaderList)
	{
		if (modelAndShader->model == selectedModel)  continue;

		modelAndShader->model->Draw(modelAndShader->shader);
	}

	if (selectedModel != nullptr)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glm::vec3 tempScale;


		tempScale = selectedModel->transform.scale;
		selectedModel->Draw(*m_Shaders[0]);
		
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
	
		
		glm::vec3 scale = selectedModel->transform.scale;
		
		selectedModel->transform.SetScale(scale + 0.1f);
		selectedModel->Draw(*m_StencilShader);
		selectedModel->transform.SetScale(tempScale);
		
	}
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);


	SortObject();

	for (ModelAndShader* modelAndShader :  transparentmodelAndShaderList)
	{
		modelAndShader->model->Draw(modelAndShader->shader);
	}

}

void GraphicsRender::ClearData()
{

	for (ModelAndShader* modelAndShader : modelAndShaderList)
	{
		delete modelAndShader->model;
		delete modelAndShader->shader;
		delete modelAndShader;
	}

	modelAndShaderList.clear();

	for (ModelAndShader* modelAndShader : transparentmodelAndShaderList)
	{
		delete modelAndShader->model;
		delete modelAndShader->shader;
		delete modelAndShader;
	}

	transparentmodelAndShaderList.clear();

}
