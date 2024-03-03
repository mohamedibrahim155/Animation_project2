#pragma once

#include "Renderer.h"
#include"model.h"
#include "Shader.h"
#include "Camera.h"


struct ModelAndShader
{
	ModelAndShader(Model* _model, Shader* _shader)
	{
		this->model = _model;
		this->shader = _shader;
	}
	Model* model;
	Shader* shader;
};

struct CompareDistances
{
	const glm::vec3& cameraPos;

	explicit CompareDistances(const glm::vec3& cameraPosition) : cameraPos(cameraPosition) {}

	bool operator()(const ModelAndShader* lhs, const ModelAndShader* rhs) const
	{
		glm::vec3 diff1 = cameraPos - lhs->model->transform.position;
		glm::vec3 diff2 = cameraPos - rhs->  model->transform.position;

		return glm::dot(diff2, diff2) < glm::dot(diff1, diff1);
	}
};




class GraphicsRender
{
public:
	GraphicsRender();
	~GraphicsRender();

	void AddModelsAndShader(Model* model, Shader* Shader);
	void AddAlphaModelsAndShader(Model* model, Shader* Shader);
	void AddModelsAndShader(Model& model, Shader& Shader);
	void AddModelAndShader(Model* model, Shader* Shader);

	void AddTransparentModels(Model* model, Shader* Shader);
	
	void AssignStencilShader( Shader* Shader);

	void AssignCamera(Camera* cam);

	void SortObject();
	
	std::vector<Model*> GetModelList();

	
	void Draw();

	Model* selectedModel;
	Model* SkyBoxModel;
	

private:
	Shader* m_StencilShader;

	void ClearData();

	std::vector<Model*> m_Models;
	std::vector<Shader*> m_Shaders;

	std::vector<Model*> m_transparentModels;
	std::vector<Shader*> m_transparentShaders;


	std::vector<ModelAndShader*> modelAndShaderList;
	std::vector<ModelAndShader*> transparentmodelAndShaderList;

	Camera* cam;

};

