#pragma once
#include <string>
#include <imgui.h>
#include <imgui_internal.h>
class Object
{
public :
	Object();
  //  ~Object();

	static unsigned int ID;
	


	bool isSelected = false;
	bool isEnabled = true;

	virtual void OnPropertyDraw() = 0;
	virtual void OnSceneDraw() = 0;
};

