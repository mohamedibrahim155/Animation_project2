#include "SkinnedMeshRenderer.h"
#include "GraphicsRender.h"
std::shared_ptr<Mesh> SkinnedMeshRenderer::ProcessMesh(aiMesh* mesh, const aiScene* scene)

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
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;



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
            vertex.vRgb = glm::vec4(color.r, color.g, color.b, color.a);
        }
        else
        {
            vertex.vRgb = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

       // vertex.BoneID = glm::vec4(0);
       // vertex.BoneWeight = glm::vec4(0.0f);

        SetDefaultVertexBoneData(vertex);


        vertices.push_back(vertex);
    }

    if (mesh->HasBones())
    {
        boneWeightInfos.resize(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumBones; ++i)
        {
            aiBone* bone = mesh->mBones[i];
            std::string name(bone->mName.C_Str(), bone->mName.length); //	'\0'
            boneIDMap.insert(std::pair<std::string, int>(name, listOfBoneInfo.size()));
            BoneInfo boneInfo;
            AssimpToGLM(bone->mOffsetMatrix, boneInfo.boneOffset);
            listOfBoneInfo.emplace_back(boneInfo);
            for (int weightIdx = 0; weightIdx < bone->mNumWeights; ++weightIdx)
            {
                aiVertexWeight& vertexWeight = bone->mWeights[weightIdx];
                BoneWeightInfo& boneInfo = boneWeightInfos[vertexWeight.mVertexId];
                for (int infoIdx = 0; infoIdx < 4; ++infoIdx)
                {
                    if (boneInfo.boneWeight[infoIdx] == 0.f)  /// xyzw of boneIDs
                    {
                        boneInfo.boneID[infoIdx] = i; // boneIndex
                        boneInfo.boneWeight[infoIdx] = vertexWeight.mWeight;
                        break;
                    }
                }
            }
        }
    }

    unsigned int vertArrayIndex = 0;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process materials

    if (mesh->HasBones())
    {
        for (unsigned int i = 0; i < vertices.size(); i++)
        {
            //std::cout << "Bone ID : " << boneWeightInfos[i].boneID[0]
            //    << " " << boneWeightInfos[i].boneID[1]
            //    << " " << boneWeightInfos[i].boneID[2]
            //    << " " << boneWeightInfos[i].boneID[3] << std::endl;
            vertices[i].BoneID = boneWeightInfos[i].boneID;
            vertices[i].BoneWeight = boneWeightInfos[i].boneWeight;
        }
    }


    if (scene->mNumAnimations > 0)
    {

        aiAnimation* animation = scene->mAnimations[0];

        std::string animation_name = animation->mName.C_Str();
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


   // ExtractBoneWeightForVertices(vertices, mesh, scene);


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

void SkinnedMeshRenderer::LoadModel(std::string const& path, bool isLoadTexture, bool isDebugModel)
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

    rootBoneNode = GenerateBoneHierarchy(scene->mRootNode);

    globalInverseTransformedMatrix = glm::inverse(rootBoneNode->transformation);

	std::cout << " Loaded  Model file  : " << directory << " Mesh count : " << scene->mNumMeshes << std::endl;

	SetModelName();

	if (isDebugModel) return;
	InitializeEntity(this);
}

void SkinnedMeshRenderer::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    std::map<std::string, BoneInfo>& boneMap = m_BoneInfoMap;

   // int& boneCount = boneCount;

    for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
    {
        int boneID = -1;

        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
           
            AssimpToGLM(mesh->mBones[boneIndex]->mOffsetMatrix, newBoneInfo.boneOffset);
            listOfBoneInfo.emplace_back(newBoneInfo);
            m_BoneInfoMap[boneName] = newBoneInfo;

            boneID = boneCount;
            boneCount++;
        }
        else
        {
            boneID = m_BoneInfoMap[boneName].id;
        }

        assert(boneID != -1);

        aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }

    }

}

void SkinnedMeshRenderer::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.BoneID[i] < 0)
        {
            vertex.BoneWeight[i] = weight;
            vertex.BoneID[i] = boneID;
            break;
        }
    }
}

void SkinnedMeshRenderer::UpdateMeshRendererBones()
{
    //glm::mat4 rootTransformation = glm::mat4(1.f);
    glm::mat4 rootTransformation = glm::mat4(1.0f);
   // glm::mat4 rootTransformation = transform.GetModelMatrix();

    CalculateMatrices(this, rootBoneNode, rootTransformation);
}

void SkinnedMeshRenderer::CalculateMatrices(Model* model, BoneNode* node, const glm::mat4& parentTransformationMatrix)
{
    std::string nodeName(node->name);

    glm::mat4 transformationMatrix = node->transformation;

    glm::mat4 globalTransformation = parentTransformationMatrix * transformationMatrix;

   // std::map<std::string, int> ::iterator boneMapIt = model->boneIDMap.find(nodeName);

    std::map<std::string, int> ::iterator boneMapIt  = boneIDMap.find(nodeName);


    if (boneMapIt != boneIDMap.end())
    {
        BoneInfo& boneInfo = model->listOfBoneInfo[boneMapIt->second];
        boneInfo.finalTransformation = model->globalInverseTransformedMatrix * globalTransformation * boneInfo.boneOffset;
        boneInfo.globalTransformation = globalTransformation;
    }

    for (int i = 0; i < node->children.size(); i++)
    {
        CalculateMatrices(this, node->children[i], globalTransformation);
    }

}

SkinnedMeshRenderer::SkinnedMeshRenderer(std::string const& path, bool isLoadTexture, bool isDebugModel)
{
    this->isLoadTexture = isLoadTexture;
   this->LoadModel(path, this->isLoadTexture, isDebugModel);
}
void SkinnedMeshRenderer::DrawProperties()
{
    Model::DrawProperties();
}

void SkinnedMeshRenderer::SceneDraw()
{
	Model::SceneDraw();
}

void SkinnedMeshRenderer::Start()
{
}

void SkinnedMeshRenderer::Update(float deltaTime)
{
}

void SkinnedMeshRenderer::Render()
{
}

void SkinnedMeshRenderer::OnDestroy()
{
}

void SkinnedMeshRenderer::Draw(Shader* shader)
{

    UpdateMeshRendererBones();


    if (!isVisible)
    {
        return;
    }
    shader->Bind();
    if (shader->modelUniform)
    {
        shader->setMat4("model", transform.GetModelMatrix());
        shader->Bind();

        shader->setBool("isBones", true);

        std::string boneMatrixUniform;

        for (int i = 0; i < listOfBoneInfo.size(); i++)
        {
            boneMatrixUniform = "BoneMatrices[" + std::to_string(i) + "]";
            shader->setMat4(boneMatrixUniform, listOfBoneInfo[i].finalTransformation);
        }

    }



    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->Draw(shader);
    }
}
