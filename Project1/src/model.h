#pragma once
#ifndef MODEL_H
#define MODEL_H



#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "mesh.h"
#include "Shader.h"
#include "Transform.h"
#include "BaseMaterial.h"
#include "UnLitMaterial.h"
#include "EntityManager/Entity.h"




class Model : public Entity
{
public:


    std::vector<Texture*> textures_loaded;
    std::vector<std::shared_ptr<Mesh>> meshes;

    std::string directory;
    std::string modelPath;
    std::string id; //if needed 

    Texture* alphaMask;

    int offset;
    float size;

    bool gammaCorrection;
    bool isTransparant = false;
    bool isCutOut = false;
    bool isWireFrame;
    bool isVisible = true;
    bool isLoadTexture;

    Model();
    Model(const Model& copyModel);
    Model( std::string const& path, bool isLoadTexture =true, bool isTextureFlip=true, bool isTransparancy= false, bool isCutOut = false);
    ~Model();
    void LoadModel(std::string const& path , bool isLoadTexture =true);
    void Draw(Shader& shader);
    void Draw(Shader* shader);
    
    virtual void OnPropertyDraw();
    virtual void OnSceneDraw();
private:
    
    void ProcessNode(aiNode* node, const aiScene* scene);   
   
    std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
    
    Texture* LoadDefaultTexture(aiTextureType type, std::string typeName);
    Texture* LoadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName);

    std::string TextureType(aiTextureType type);
   
    bool isTextureFlipped = true;

    const std::string alphaTextureDefaultPath = "Textures/DefaultTextures/Opacity_Default.png";

    // Inherited via Entity
    void Start() override;
    void Update(float deltaTime) override;
    virtual void OnDestroy();
};




#endif