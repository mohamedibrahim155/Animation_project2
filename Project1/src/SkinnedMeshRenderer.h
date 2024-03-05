#pragma once
#include "model.h"
class SkinnedMeshRenderer : public Model
{
public:
    SkinnedMeshRenderer(std::string const& path, bool isLoadTexture = true, bool isDebugModel = false);
    virtual ~SkinnedMeshRenderer() {}
     void DrawProperties() override;
     void SceneDraw() override;

     void Start() override;
     void Update(float deltaTime) override;
     void Render() override;
     void OnDestroy() override;
     void Draw(Shader* shader) override;
   std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene) override;
   void LoadModel(std::string const& path, bool isLoadTexture = true, bool isDebugModel = false) override;


   std::map<std::string, BoneInfo>& GetBoneMap() { return m_BoneInfoMap; }

   int& GetBoneCount() { return boneCount; }


   private:

       void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
       void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
       std::map<std::string, BoneInfo> m_BoneInfoMap;
       int boneCount = 0;

       void UpdateMeshRendererBones();
       void CalculateMatrices(Model* model, BoneNode* boneNode, const glm::mat4& parentTransformationMatrix);
};

