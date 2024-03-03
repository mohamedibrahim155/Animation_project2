#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class BasePanel
{
public:

	bool isPanelOpen = true;
	bool isEnable = true;


	BasePanel();
	virtual void OnRender(float windowWidth, float windowHeight) =0;


};


