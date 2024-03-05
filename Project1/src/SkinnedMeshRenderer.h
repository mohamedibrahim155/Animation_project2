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

    // void AddAnimation();

     void LoadAnimation(const std::string& animationPath);
     void UpdateSkeletonAnimation(float deltaTime);
     void UpdateAnimationFrame(NodeAnim* anim , glm::mat4& nodeTransform, double time);
    glm::mat4 UpdateTranslation(std::vector<PositionKeyFrame>& listOfKeyFrames, float time);
    glm::mat4 UpdateRotation(std::vector<RotationKeyFrame>& listOfKeyFrames, float time);
    glm::mat4 UpdateScale(std::vector<ScaleKeyFrame>& listOfKeyFrames, float time);

     const SkeletonAnim* GetCurrentSkeletonAnimation();
   private:

       std::map<std::string, BoneInfo> boneInfoMap;
       std::map<std::string, BoneNode*> boneNodeMap;
       std::vector<BoneInfo> listOfBoneInfo;
       int boneCount = 0;
       BoneNode* rootBoneNode;

       void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
       void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
       void UpdateMeshRendererBones();
       void CalculateMatrices(BoneNode* boneNode, const glm::mat4& parentTransformationMatrix);

       std::vector<SkeletonAnim*> listOfAnimations;

       SkeletonAnim* currentAnimation;

       int currentAnimationIndex = 0;   

       float timeStep;
};

