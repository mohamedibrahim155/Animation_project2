#pragma once
#include "../Object.h"
#include"../Transform.h"


class EntityManager;
class EditorLayout;
class Animation;
class AnimationSystem;

class Entity : public Object
{
public:

	std::string entityID;
	std::string tag;
	std::string name = "EmptyGameObject";

	Transform transform;


	bool isStartInvoked = false;

	virtual ~Entity() {};

	virtual void InitializeEntity(Entity* entity);
	virtual void Start() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void OnDestroy() = 0;

	 virtual void Destroy();

	 //Inherited from Object
	 virtual void OnPropertyDraw();
	 virtual void OnSceneDraw();



};

