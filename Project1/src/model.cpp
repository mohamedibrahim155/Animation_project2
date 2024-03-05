#include "model.h"
#include "ImGui/EditorLayout.h"
#include "GraphicsRender.h"

aiMesh* ai_Mesh;

Model::Model()
{

}

Model::Model(const Model& copyModel, bool isDebugModel)
{
    meshes = copyModel.meshes;
    directory = copyModel.directory;
    isVisible = copyModel.isVisible;
    isWireFrame = copyModel.isWireFrame;
    modelPath = copyModel.modelPath;
    isLoadTexture = copyModel.isLoadTexture;

    SetModelName();
    if (isDebugModel) return;

    InitializeEntity(this);
}

Model::Model(std::string const& path, bool isLoadTexture, bool isDebugModel)
{
    this->isLoadTexture = isLoadTexture;
    LoadModel(path, this->isLoadTexture, isDebugModel);
}

Model::~Model()
{
    delete alphaMask;

    for (Texture* texture : textures_loaded)
    {
        delete texture;

    }
    textures_loaded.clear();

    meshes.clear();
}





void Model::Draw(Shader& shader)
{
    shader.Bind();
   
    if (shader.modelUniform)
    {
        shader.setMat4("model", transform.GetModelMatrix());
    }
    
    if (!isVisible)
    {
        return;
    }
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
       
        meshes[i]->Draw(&shader);
    }
}

void Model::Draw(Shader* shader)
{

    if (!isVisible)
    {
        return;
    }
    shader->Bind();
    if (shader->modelUniform)
    {
        shader->setMat4("model", transform.GetModelMatrix());
    }

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->Draw(shader);
    }
}

void Model::DrawSolidColor(const glm::vec4& color, bool isWireframe)
{
    if (!isVisible)
    {
        return;
    }

    Shader* solidShader = GraphicsRender::GetInstance().solidColorShader;

    solidShader->Bind();
    if (solidShader->modelUniform)
    {
        solidShader->setMat4("model", transform.GetModelMatrix());
    }
    
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->isWireFrame = isWireframe;
        meshes[i]->DrawSolidColorMesh(solidShader, color);
    }
}



void Model::LoadModel(const Model& copyModel, bool isDebugModel)
{
    meshes = copyModel.meshes;
    directory = copyModel.directory;
    isVisible = copyModel.isVisible;
    isWireFrame = copyModel.isWireFrame;
    modelPath = copyModel.modelPath;
    isLoadTexture = copyModel.isLoadTexture;

    if (name.empty())
    {
        SetModelName();
    }

    if (isDebugModel) return;
    InitializeEntity(this);


}
void Model::LoadModel(std::string const& path, bool isLoadTexture, bool isDebugModel)
{
    this->isLoadTexture = isLoadTexture;
    this->modelPath = path;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
   
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene);

    std::cout << " Loaded  Model file  : " << directory << " Mesh count : " << scene->mNumMeshes << std::endl;

    std::cout << " Loaded  Model file  : " << directory << " Mesh count : " << scene->mNumMeshes << std::endl;
    SetModelName();

    if (isDebugModel) return;
    InitializeEntity(this);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
       
        ai_Mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(ai_Mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
   
}

std::shared_ptr<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
     std::vector<Vertex> vertices;
     std::vector<unsigned int> indices;
     std::vector<Texture*> textures;

     std::vector<BoneWeightInfo> boneWeightInfos;


    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; 
        // positions
        vector.x = mesh->mVertices[i].x ;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        SetDefaultVertexBoneData(vertex);

        // normals
        if (mesh->HasNormals())
        {
          
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) 
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        }

        if (mesh->HasVertexColors(0))
        {
            aiColor4D color = mesh->mColors[0][i];
            vertex.vRgb = glm::vec4(color.r, color.g, color.b,color.a);
        }
        else
        {
            vertex.vRgb = glm::vec4(1.0f, 1.0f, 1.0f,1.0f);
        }

        //vertex.BoneID = glm::vec4(0);
        //vertex.BoneWeight = glm::vec4(0.0f);

  
   
        vertices.push_back(vertex);
    }

    
    //if (mesh->HasBones())
    //{
    //    boneWeightInfos.resize(mesh->mNumVertices);
    //    for (unsigned int i = 0; i < mesh->mNumBones; ++i)
    //    {
    //        aiBone* bone = mesh->mBones[i];
    //        std::string name(bone->mName.C_Str(), bone->mName.length); //	'\0'
    //        boneIDMap.insert(std::pair<std::string, int>(name, listOfBoneInfo.size()));
    //        BoneInfo boneInfo;
    //        AssimpToGLM(bone->mOffsetMatrix, boneInfo.boneOffset);
    //        listOfBoneInfo.emplace_back(boneInfo);
    //        for (int weightIdx = 0; weightIdx < bone->mNumWeights; ++weightIdx)
    //        {
    //            aiVertexWeight& vertexWeight = bone->mWeights[weightIdx];
    //           
    //            BoneWeightInfo& boneInfo = boneWeightInfos[vertexWeight.mVertexId];
    //            for (int infoIdx = 0; infoIdx < 4; ++infoIdx)
    //            {
    //                if (boneInfo.boneWeight[infoIdx] == 0.f)  /// xyzw of boneIDs
    //                {
    //                    boneInfo.boneID[infoIdx] = i; // boneIndex
    //                    boneInfo.boneWeight[infoIdx] = vertexWeight.mWeight;
    //                    break;
    //                }
    //            }
    //        }
    //    }
    //}

    unsigned int vertArrayIndex = 0;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process materials

    /*if (mesh->HasBones())
    {
        for (unsigned int i = 0; i < vertices.size(); i++)
        {
            std::cout << "Bone ID : " << boneWeightInfos[i].boneID[0]
                << " " << boneWeightInfos[i].boneID[1]
                << " " << boneWeightInfos[i].boneID[2]
                << " " << boneWeightInfos[i].boneID[3] << std::endl;
            vertices[i].BoneID = boneWeightInfos[i].boneID;
            vertices[i].BoneWeight = boneWeightInfos[i].boneWeight;
        }
    }*/
   
    
    if (scene->mNumAnimations > 0)
    {

        aiAnimation* animation = scene->mAnimations[0];

          std ::string animation_name = animation->mName.C_Str();
          double animation_Duration = animation->mDuration;
          double tickesPerSecond = animation->mTicksPerSecond;

        for (int i = 0; i < animation->mNumChannels; i++)
        {
            aiNodeAnim* assimpNodeAnim = animation->mChannels[i];

            for (int i = 0; i < assimpNodeAnim->mNumPositionKeys; i++)
            {
                aiVectorKey& p = assimpNodeAnim->mPositionKeys[i];
            }

            for (int i = 0; i < assimpNodeAnim->mNumScalingKeys; ++i)
            {
                aiVectorKey& s = assimpNodeAnim->mScalingKeys[i];
            }

            for (int i = 0; i < assimpNodeAnim->mNumRotationKeys; ++i)
            {
                aiQuatKey& q = assimpNodeAnim->mRotationKeys[i];
            }
        }

    }



    aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);

    BaseMaterial* baseMeshMaterial = new Material();

    if (mesh->mMaterialIndex >= 0)
    {
        if (isLoadTexture)
        {
           // baseMeshMaterial = new Material();
            aiMaterial* m_aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

            baseMeshMaterial->material()->diffuseTexture = LoadMaterialTexture(m_aiMaterial, aiTextureType_DIFFUSE, "diffuse_Texture");
            baseMeshMaterial->material()->specularTexture = LoadMaterialTexture(m_aiMaterial, aiTextureType_SPECULAR, "specular_Texture");
            baseMeshMaterial->material()->alphaTexture = LoadMaterialTexture(m_aiMaterial, aiTextureType_OPACITY, "opacity_Texture");

            Texture* materialTexture = (Texture*)baseMeshMaterial->material()->alphaTexture;

            if (materialTexture->path != alphaTextureDefaultPath)
            {
                baseMeshMaterial->material()->useMaskTexture = true;
            }

            baseMeshMaterial->material()->SetBaseColor(glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a));
        }
        else
        {

            baseMeshMaterial = new UnLitMaterial();
            baseMeshMaterial->unLitMaterial()->SetBaseColor(glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a));
        }
    }
    else
    {
        baseMeshMaterial = new UnLitMaterial();
        baseMeshMaterial->unLitMaterial()->SetBaseColor(glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a));
    }

   

    return std::make_shared<Mesh>(vertices, indices, baseMeshMaterial);
 }




 Texture* Model::LoadDefaultTexture(aiTextureType type, std::string typeName)
 {
     std::string path = "";
     switch (type)
     {
     case aiTextureType_DIFFUSE:
         path = "Textures/DefaultTextures/Default_Diffuse.png";
         break;
     case aiTextureType_SPECULAR:
         path = "Textures/DefaultTextures/Default_Specular.jpg";
         break;
     case aiTextureType_OPACITY:
         path = "Textures/DefaultTextures/Default_Opacity.png";
         break;
    
     }

     for (unsigned int i = 0; i < textures_loaded.size(); i++)
     {
         if (std::strcmp(textures_loaded[i]->path.data(), path.c_str()) == 0)
         {
             return textures_loaded[i];
         }
     }

     Texture* defaultTexture = new Texture(path);
     defaultTexture->type = typeName.c_str();
     textures_loaded.push_back(defaultTexture);
     return defaultTexture;
 }

 Texture* Model::LoadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName)
 {
     if (mat->GetTextureCount(type) == 0)
     {
         return LoadDefaultTexture(type, typeName);
     }

     for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
     {
         aiString texString;
         mat->GetTexture(type, i, &texString);

         std::cout << " texutre path : " << texString.C_Str() << "Texture Type : " << TextureType(type) << std::endl;
         std::string filename = std::string(texString.C_Str());
         filename = directory + '/' + filename;
         std::ifstream file(filename);
         for (unsigned int i = 0; i < textures_loaded.size(); i++)
         {
             if (std::strcmp(textures_loaded[i]->path.data(), filename.c_str()) == 0)
             {
                 return textures_loaded[i];
             }
         }


         if (file.good())
         {
             Texture* texture = new Texture(filename);

             texture->type = typeName;
             textures_loaded.push_back(texture);

             return texture;
         }
         else
         {
             return LoadDefaultTexture(type, typeName);
         }

     }
 }

 std::string Model::TextureType(aiTextureType type)
 {
     switch (type)
     {
     case aiTextureType_NONE:
         return  "NO TEXTURE";
         break;
     case aiTextureType_DIFFUSE:
         return  "DIFFUSE";
         break;
     case aiTextureType_SPECULAR:
         return  "SPECULAR";
         break;
     case aiTextureType_AMBIENT:
         break;
     case aiTextureType_EMISSIVE:
         break;
     case aiTextureType_HEIGHT:
         break;
     case aiTextureType_NORMALS:
         break;
     case aiTextureType_SHININESS:
         break;
     case aiTextureType_OPACITY:
         return "OPACITY";
         break;
     case aiTextureType_DISPLACEMENT:
         break;
     case aiTextureType_LIGHTMAP:
         break;
     case aiTextureType_REFLECTION:
         break;
     case aiTextureType_BASE_COLOR:
         break;
     case aiTextureType_NORMAL_CAMERA:
         break;
     case aiTextureType_EMISSION_COLOR:
         break;
     case aiTextureType_METALNESS:
         break;
     case aiTextureType_DIFFUSE_ROUGHNESS:
         break;
     case aiTextureType_AMBIENT_OCCLUSION:
         break;
     case aiTextureType_SHEEN:
         break;
     case aiTextureType_CLEARCOAT:
         break;
     case aiTextureType_TRANSMISSION:
         break;
     case aiTextureType_UNKNOWN:
         break;
     case _aiTextureType_Force32Bit:
         break;
     default:
         return "NO TEXTURE";
         break;
     }
 }

 void Model::SetModelName()
 {
     int lastSlastPosition = modelPath.find_last_of('/');

     if (lastSlastPosition != std::string::npos)
     {
         name = modelPath.substr(lastSlastPosition + 1);
     }
     else
     {
         name = modelPath;
     }

     for (int i = 0; i < meshes.size(); i++)
     {
         if (meshes[i]->name.empty())
         {
             meshes[i]->name = "mesh " + std::to_string(i + 1);
         }
     }
 }

 void Model::SetDefaultVertexBoneData(Vertex& vertex)
 {
     for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
     {
         vertex.BoneID[i] = -1;
         vertex.BoneWeight[i] = 0.0f;
     }
 }

 void Model::Render()
 {
 }

 void Model::Start()
 {
    
 }

 void Model::Update(float deltaTime)
 {
 }

 void Model::OnDestroy()
 {
     GraphicsRender::GetInstance().RemoveModel(this);
 }



 void Model::DrawProperties()
 {
     Entity::DrawProperties();
 }

 void Model::SceneDraw()
 {
     ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
     node_flags |= ImGuiTreeNodeFlags_SpanFullWidth;

     if (isSelected)
     {
         node_flags |= ImGuiTreeNodeFlags_Selected;
     }

     bool node_open = ImGui::TreeNodeEx(name.c_str(), node_flags);

     if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
     {
         GraphicsRender::GetInstance().SetSelectedModel(this);

         EditorLayout::GetInstance().SetSelectedObjects({ this });

     }


     if (node_open)
     {
         for (std::shared_ptr<Mesh> mesh : meshes)
         {
             ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf;
             leaf_flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
             leaf_flags |= ImGuiTreeNodeFlags_SpanFullWidth;

             if (mesh->isSelected)
             {
                 leaf_flags |= ImGuiTreeNodeFlags_Selected;
             }

             ImGui::TreeNodeEx(mesh->name.c_str(), leaf_flags);
             if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
             {
                 GraphicsRender::GetInstance().SetSelectedModel(this);
             
                 EditorLayout::GetInstance().SetSelectedObjects({ mesh.get() });
             }
         }

         ImGui::TreePop();
     }

 }




