#include "Entity.h"
#include "EntityManager.h"
#include "../ImGui/EditorLayout.h"
#include "../Animation/Animation.h"
#include "../Animation/AnimationSystem.h"


void Entity::InitializeEntity(Entity* entity)
{
	EntityManager::GetInstance().AddEntity(entity);
}

void Entity::Destroy()
{
	EntityManager::GetInstance().RemoveEntity(entityID);
}

void Entity::OnPropertyDraw()
{
	ImGui::InputText("##ObjectName", &name[0], 516);
	ImGui::SameLine();
	ImGui::InputText("##Tag", &tag[0], 516);
	transform.OnPropertyDraw();
}

void Entity::OnSceneDraw()
{
	if (ImGui::Selectable(name.c_str(), &isSelected))
	{
		EditorLayout::GetInstance().SetSelectedObject(this);
	}
}
