#pragma once
#include "../ImGui/BasePanel.h"
#include <iostream>

class EntityManager;

class Heirachy : public BasePanel
{
public:

	Heirachy();
	void OnRender(float windowWidth, float windowHeight) override;

	EntityManager* entityManager = nullptr;

private :

	void ShowAllEntities();
};

