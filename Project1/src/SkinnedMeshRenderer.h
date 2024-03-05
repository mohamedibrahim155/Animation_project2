#pragma once
#include "model.h"
class SkinnedMeshRenderer : public Model
{
public:

    SkinnedMeshRenderer();
    SkinnedMeshRenderer(std::string const& path, bool isLoadTexture = true, bool isDebugModel = false);
    virtual ~SkinnedMeshRenderer() {}

    int& GetBoneCount() { return boneCount; }
    virtual std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene) override;
    std::map<std::string, BoneInfo>& GetBoneMap() { return boneInfoMap; }

    BoneNode* GenerateBoneHierarchy(aiNode* ainode, const int depth = 0);
    BoneNode* CreateNode(aiNode* node);

    void LoadModel(std::string const& path, bool isLoadTexture = true, bool isDebugModel = false) override;
    virtual void DrawProperties() override;
    virtual void SceneDraw() override;

    virtual void Start() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;
    virtual void OnDestroy() override;
     void Draw(Shader* shader) override;

    // void AddAnimation();

     void LoadAnimation(const std::string& animationPath);
     void UpdateSkeletonAnimation(float deltaTime);
     void UpdateAnimationFrame(NodeAnim* anim , glm::mat4& nodeTransform, double time);

    glm::vec3 UpdateTranslation(std::vector<PositionKeyFrame>& listOfKeyFrames, float time);
    glm::quat UpdateRotation(std::vector<RotationKeyFrame>& listOfKeyFrames, float time);
    glm::vec3 UpdateScale(std::vector<ScaleKeyFrame>& listOfKeyFrames, float time);

     const SkeletonAnim* GetCurrentSkeletonAnimation();
     SkeletonAnim* currentAnimation;
     int currentAnimationIndex = 0;
     bool isPlaying = false;

     void ChangeAnimation(int animationIndex);

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
       float timeStep;
};

