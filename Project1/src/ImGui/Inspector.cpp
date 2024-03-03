#include "Inspector.h"

void Inspector::OnRender(float windowWidth, float windowHeight)
{

    windowWidth = 400;
    windowHeight = 400;

    ImGui::Begin("Inspector");
    ImGui::SetWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::Text("Add Something");
    ImGui::End();

}
