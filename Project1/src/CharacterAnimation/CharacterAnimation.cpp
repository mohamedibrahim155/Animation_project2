#include "CharacterAnimation.h"
#include "../GraphicsRender.h"
CharacterAnimation::CharacterAnimation()
{


    LoadModel("Models/Character/X Bot.fbx");
    transform.SetScale(glm::vec3(0.01f));
    //characterMesh = new SkinnedMeshRenderer("Models/Character/X Bot.fbx");
   // characterMesh->transform.SetScale(glm::vec3(0.01f));

    //characterMesh->LoadAnimation("Models/Character/Rumba Dancing.fbx");
    LoadAnimation("Models/Character/Rumba Dancing.fbx");
    LoadAnimation("Models/Character/Punching Bag.fbx");
    LoadAnimation("Models/Character/Mma Kick.fbx");
    LoadAnimation("Models/Character/Snake Hip Hop Dance.fbx");
    LoadAnimation("Models/Character/Waving.fbx");

    GraphicsRender::GetInstance().AddModelAndShader(this, GraphicsRender::GetInstance().animationShader);

    InputManager::GetInstance().AddObserver(this);
}

CharacterAnimation::~CharacterAnimation()
{
}

void CharacterAnimation::OnKeyPressed(const int& key)
{
    if (key == GLFW_KEY_SPACE)
    {
        isPlaying = !isPlaying;
    }
    if (key == GLFW_KEY_1)
    {
        ChangeAnimation(0);
    }
    if (key == GLFW_KEY_2)
    {
        ChangeAnimation(1);
    }

    if (key == GLFW_KEY_3)
    {
        ChangeAnimation(2);
    }
    if (key == GLFW_KEY_4)
    {
        ChangeAnimation(3);
    }
    if (key == GLFW_KEY_5)
    {
        ChangeAnimation(4);
    }
}

void CharacterAnimation::OnKeyReleased(const int& key)
{
}

void CharacterAnimation::OnKeyHold(const int& key)
{
}

void CharacterAnimation::SceneDraw()
{
    SkinnedMeshRenderer::SceneDraw();
}

void CharacterAnimation::DrawProperties()
{
    SkinnedMeshRenderer::DrawProperties();
}

void CharacterAnimation::Start()
{
}

void CharacterAnimation::Update(float deltaTime)
{
    SkinnedMeshRenderer::Update(deltaTime);
}

void CharacterAnimation::Render()
{
}

void CharacterAnimation::OnDestroy()
{
}
