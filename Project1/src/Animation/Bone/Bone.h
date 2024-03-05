#pragma once
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <map>
#include <string>		
#include <vector>



struct BoneInfo
{
	int id;
	glm::mat4 boneOffset;				// Offset from the parent bone/node
	glm::mat4 finalTransformation;		// Calculated transformation used for rendering
	glm::mat4 globalTransformation;		// used for the bone hierarchy transformation calculations when animating
};

struct BoneNode
{
	BoneNode(const std::string& name) : name(name) { }
	std::string name;
	int index;               // bone UID
	glm::mat4 transformation;
	std::vector<BoneNode*> children;
};

struct Armature
{
	std::vector<BoneNode> bones;  // Root bones of the armature
};

struct BoneWeightInfo 
{
	BoneWeightInfo() {
		boneID = glm::vec4(0);
		boneWeight = glm::vec4(0);
	}
	glm::vec4 boneID;
	glm::vec4 boneWeight;
};

void AssimpToGLM(const aiMatrix4x4& a, glm::mat4& g);

