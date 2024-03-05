#pragma once
#include "model.h"
class SkinnedMeshRenderer : public Model
{
public:


    SkinnedMeshRenderer(std::string const& path, bool isLoadTexture = true, bool isDebugModel = false);
    virtual ~SkinnedMeshRenderer() {}

    int& GetBoneCount() { return boneCount; }
    std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene) override;
    std::map<std::string, BoneInfo>& GetBoneMap() { return boneInfoMap; }

    BoneNode* GenerateBoneHierarchy(aiNode* ainode, const int depth = 0);
    BoneNode* CreateNode(aiNode* node);

     void LoadModel(std::string const& path, bool isLoadTexture = true, bool isDebugModel = false) override;
     void DrawProperties() override;
     void SceneDraw() override;

     void Start() override;
     void Update(float deltaTime) override;
     void Render() override;
     void OnDestroy() override;
     void Draw(Shader* shader) override;

   private:

       std::map<std::string, BoneInfo> boneInfoMap;
       std::vector<BoneInfo> listOfBoneInfo;
       int boneCount = 0;
       BoneNode* rootBoneNode;

       void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
       void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
       void UpdateMeshRendererBones();
       void CalculateMatrices(Model* model, BoneNode* boneNode, const glm::mat4& parentTransformationMatrix);


};

