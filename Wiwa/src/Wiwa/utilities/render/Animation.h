#pragma once
#include <Wiwa/core/Core.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <glm/glm.hpp>
#include <Wiwa/utilities/math/Math.h>
#include <Wiwa/utilities/filesystem/FileSystem.h>
#include <Wiwa/utilities/render/Bone.h>
#include <Wiwa/utilities/render/Model.h>

struct aiAnimation;
struct BoneInfo;
class ModelHierarchy;

namespace Wiwa {
	struct NodeData
	{
		NodeData() {};
		~NodeData() {
			for (auto child : children) {
				delete child; 
			}
		}
		glm::mat4 transformation;
		glm::mat4 globalTransformation;
		std::string name;
		int childrenCount;
		std::vector<NodeData*> children;
	};

	class WI_API Animation
	{
	public:
		Animation();
		Animation(const aiAnimation* animation, Model* model);
		Animation(const aiAnimation* animation);
		Animation(const char* filePath, Model* model);
		Animation(const char* filePath);
		~Animation();

		Bone* FindBone(const std::string& name);

		inline int GetTicksPerSecond() { return m_TicksPerSecond; }

		inline float GetDuration() { return m_Duration; }

		inline bool HasFinished() { return m_HasFinished; }

		inline std::vector<Bone*> GetBones() { return m_Bones; }

		inline const NodeData* GetRootNode() { return m_RootNode; }

		inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
		{
			return m_BoneInfoMap;
		}

		glm::mat4 GetBoneTransform(int index) {return m_Bones[index]->GetLocalTransform(); };

		void LoadAnimation(const aiAnimation* animation);

		static Animation* LoadWiAnimation(const char* filepath);

		static void SaveWiAnimation(Animation* animation, const char* path);

		void SaveNodeData(File& file, NodeData* node);

		NodeData* LoadNodeData(File& file);

		static Animation* GetAnimationFromFile(const char* filepath);

		void PrintGlmMatrix(const glm::mat4& mat);

		std::string m_Name;

		std::string m_SavePath;

		bool m_Loop;
		
		bool m_HasFinished;

		float m_CurrentTime = 0;

		float m_Duration = 0;

		int m_TicksPerSecond = 0;

		int m_NumChannels = 0;

	private:
		void ReadMissingBones(const aiAnimation* animation, Model& model);

		void ReadHeirarchyData(NodeData* dest, const ModelHierarchy* root,glm::mat4& parentTransform);

		glm::mat4 CalculateGlobalTransform(const ModelHierarchy* bone, glm::mat4 parentTransform);

		std::vector<Bone*> m_Bones;
		NodeData* m_RootNode;
		std::map<std::string, BoneInfo> m_BoneInfoMap;
		std::map<std::string, std::vector<glm::mat4>> m_CalculatedBoneMatrices;
		unsigned int m_BoneCount;
	};
}

