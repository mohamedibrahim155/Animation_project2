#include "EditorLayout.h"

EditorLayout& EditorLayout::GetInstance()
{
    static EditorLayout editorInstance;

    return editorInstance;
}

Object* EditorLayout::GetCurrentSelectedObject()
{
    return currentSelectedObject;
}

void EditorLayout::SetSelectedObject(Object* object)
{
    currentSelectedObject = object;
}

void EditorLayout::InitializeEditors()
{
    hierarchyPanel = new Heirachy();
    inspectorPanel = new Inspector();

}
