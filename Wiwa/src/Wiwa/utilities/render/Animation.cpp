#include "wipch.h"
#include "Animation.h"
#include <Wiwa/core/Resources.h>
#include <assimp/anim.h>
#include <glm/gtx/quaternion.hpp>
namespace Wiwa {
	Animation::Animation()
	{
		m_Duration = 0;
		m_TicksPerSecond = 0;
		m_Name = "new animation";
		m_Loop = false;
		m_HasFinished = false;
	}
	Animation::Animation(const aiAnimation* animation, Model* model)
	{
		m_Duration = (float)animation->mDuration;
		m_TicksPerSecond = (int)animation->mTicksPerSecond;
		m_NumChannels = animation->mNumChannels;
		m_Name = animation->mName.C_Str();
		m_BoneInfoMap = model->GetBoneInfoMap();
		m_Loop = false;
		m_HasFinished = false;
		m_RootNode = new NodeData();
		ReadMissingBones(animation, *model);		
		ReadHeirarchyData(m_RootNode, model->getModelHierarchy(), glm::mat4(1.f));
	}

	Animation::Animation(const aiAnimation* animation)
	{
		m_Duration = (float)animation->mDuration;
		m_TicksPerSecond = (int)animation->mTicksPerSecond;
		m_NumChannels = animation->mNumChannels;
		m_Name = animation->mName.C_Str();
		m_Loop = false;
		m_HasFinished = false;
	}

	Animation::Animation(const char* filePath, Model* model)
	{
		m_Duration = 0;
		m_TicksPerSecond = 0;
		m_Name = "new animation";
		m_Loop = false;
		m_HasFinished = false;
	}

	Animation::Animation(const char* filePath)
	{
		//Load animation from assimp
	}

	Animation::~Animation()
	{
		for (std::vector<Bone*>::iterator item = m_Bones.begin(); item != m_Bones.end(); item++)
		{
			delete* item;
			*item = nullptr;
		}
		m_Bones.clear();

		delete m_RootNode;

		m_BoneInfoMap.clear();
		m_CalculatedBoneMatrices.clear();
	}

	Bone* Animation::FindBone(const std::string& name)
	{
		for (auto& bone : m_Bones)
		{
			if (bone->m_Name == name)
				return bone;
		}
		return nullptr;
	}

	void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
	{
		int size = animation->mNumChannels;

		auto& boneInfoMap = model.GetBoneInfoMap();	//getting m_BoneInfoMap from Model class
		int& boneCount = model.GetBoneCount();		//getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(new Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
		}
		m_BoneCount = boneCount;
		m_BoneInfoMap = boneInfoMap;
	}

	void Animation::ReadHeirarchyData(NodeData* dest, const ModelHierarchy* root, glm::mat4& parentTransform)
	{
		assert(root);		

		glm::mat4 nodeTransform = root->Transformation;
		glm::mat4 globalTransform = parentTransform * nodeTransform;

		if (m_BoneInfoMap.find(root->name) != m_BoneInfoMap.end()) {
			m_BoneInfoMap[root->name].globalTransformation = globalTransform;
		}

		dest->name = root->name.data();
		dest->transformation = root->Transformation;
		dest->childrenCount = root->children.size();

		for (unsigned int i = 0; i < root->children.size(); i++) {
			NodeData* newData = new NodeData();
			ReadHeirarchyData(newData,root->children[i], globalTransform);
			dest->children.push_back(newData);
		}
	}
	glm::mat4  Animation::CalculateGlobalTransform(const ModelHierarchy* bone, glm::mat4 parentTransform)
	{
		glm::mat4 nodeTransform = bone->Transformation;
		glm::mat4 globalTransform = parentTransform * nodeTransform;

		if (m_BoneInfoMap.find(bone->name) != m_BoneInfoMap.end()) {
			m_BoneInfoMap[bone->name].globalTransformation = globalTransform;
		}

		for (auto child : bone->children) {
			CalculateGlobalTransform(child, globalTransform);
		}

		return globalTransform;
	}
	void Animation::PrintGlmMatrix(const glm::mat4& mat)
	{
		WI_INFO("{0}_{1}_{2}_{3}", mat[0][0], mat[0][1], mat[0][2], mat[0][3]);
		WI_INFO("{0}_{1}_{2}_{3}", mat[1][0], mat[1][1], mat[1][2], mat[1][3]);
		WI_INFO("{0}_{1}_{2}_{3}", mat[2][0], mat[2][1], mat[2][2], mat[2][3]);
		WI_INFO("{0}_{1}_{2}_{3}", mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
		WI_INFO("\n");
	}
	void Animation::LoadAnimation(const aiAnimation* animation)
	{
		m_Duration = (float)animation->mDuration;
		m_TicksPerSecond = (int)animation->mTicksPerSecond;
		m_NumChannels = animation->mNumChannels;
		m_Name = animation->mName.C_Str();

		for (unsigned int i = 0; i < animation->mNumChannels; i++)
		{
			m_Bones.push_back(new Bone(animation->mChannels[i]));
		}
	}
	void Animation::SaveWiAnimation(Animation* animation, const char* path)
	{
		std::filesystem::path name = animation->m_Name;
		std::filesystem::path filepath = path;
		filepath.replace_filename(animation->m_Name.c_str());
		filepath.replace_extension("wianim");
		animation->m_SavePath =  Wiwa::Resources::_assetToLibPath(filepath.string());

		File file = FileSystem::OpenOB(filepath.string().c_str());

		size_t name_len = animation->m_Name.size();
		file.Write(&name_len, sizeof(size_t));
		file.Write(animation->m_Name.c_str(), name_len);

		size_t savep_len = animation->m_SavePath.size();
		file.Write(&savep_len, sizeof(size_t));
		file.Write(animation->m_SavePath.c_str(), savep_len);

		file.Write(&animation->m_Duration, sizeof(double));
		file.Write(&animation->m_TicksPerSecond, sizeof(double));
		file.Write(&animation->m_NumChannels, sizeof(unsigned int));

		//save NodeAnim structure
		animation->SaveNodeData(file, animation->m_RootNode);

		//save bone info map
		size_t bone_index_size = animation->m_BoneInfoMap.size();
		file.Write(&bone_index_size, sizeof(size_t));
		std::map<std::string, BoneInfo>::iterator it;
		for (it = animation->m_BoneInfoMap.begin(); it != animation->m_BoneInfoMap.end(); it++)
		{
			size_t name_size = it->first.size();
			file.Write(&name_size, sizeof(size_t));
			file.Write(it->first.c_str(), name_size);
			file.Write(&it->second.id, sizeof(unsigned int));
			file.Write(&it->second.offsetMatrix, sizeof(glm::mat4));
			file.Write(&it->second.globalTransformation, sizeof(glm::mat4));
		}

		size_t channels_size = animation->m_Bones.size();
		file.Write(&channels_size, sizeof(size_t));

		for (unsigned int i = 0; i < channels_size; i++)
		{
			animation->m_Bones[i]->SaveWiAnimNode(file, animation->m_Bones[i]);
		}
		file.Close();
	}

	void Animation::SaveNodeData(File& file, NodeData* node)
	{
		size_t name_len = node->name.size();
		file.Write(&name_len, sizeof(size_t));
		file.Write(node->name.c_str(), name_len);

		file.Write(&node->childrenCount, sizeof(int));
		file.Write(&node->transformation,sizeof(glm::mat4));

		for (int i = 0; i < node->children.size(); i++)
		{
			SaveNodeData(file, node->children[i]);
		}
	}

	NodeData* Animation::LoadNodeData(File& file)
	{
		NodeData* node = new NodeData();

		size_t name_len;
		file.Read(&name_len, sizeof(size_t));
		node->name.resize(name_len);
		file.Read(&node->name[0], name_len);

		file.Read(&node->childrenCount, sizeof(int));
		file.Read(&node->transformation, sizeof(glm::mat4));

		for (int i = 0; i < node->childrenCount; i++)
		{
			node->children.emplace_back(LoadNodeData(file));
		}

		return node;
	}

	Animation* Animation::GetAnimationFromFile(const char* filepath)
	{
		//Animation* anim = new Animation();

		return nullptr;
	}
	
	Animation* Animation::LoadWiAnimation(const char* filepath)
	{
		File file = Wiwa::FileSystem::OpenIB(filepath);
		Animation* anim = new Animation();
		//load name
		size_t name_len;
		file.Read(&name_len, sizeof(size_t));
		if (name_len > 100)
		{
			WI_ERROR("ANIMATION: string length oversize -- LoadWiAnimation");
			return nullptr;
		}
		anim->m_Name.resize(name_len);
		file.Read(&anim->m_Name[0], name_len);
		//load save path
		size_t savep_len;
		file.Read(&savep_len, sizeof(size_t));
		if (savep_len > 500)
		{
			WI_ERROR("ANIMATION: string length oversize -- LoadWiAnimation");
			return nullptr;
		}
		anim->m_SavePath.resize(savep_len);
		file.Read(&anim->m_SavePath[0], savep_len);

		file.Read(&anim->m_Duration, sizeof(double));
		file.Read(&anim->m_TicksPerSecond, sizeof(double));
		file.Read(&anim->m_NumChannels, sizeof(unsigned int));

		//Load NodeAnim structure
		anim->m_RootNode = anim->LoadNodeData(file);

		//save bone info map
		size_t bone_index_size;
		file.Read(&bone_index_size, sizeof(size_t));
		for (size_t i = 0; i < bone_index_size; i++)
		{
			std::pair<std::string, BoneInfo> item;
			size_t name_size;
			file.Read(&name_size, sizeof(size_t));
			item.first.resize(name_size);
			file.Read(&item.first[0], name_size);
			file.Read(&item.second.id, sizeof(unsigned int));
			file.Read(&item.second.offsetMatrix, sizeof(glm::mat4));
			file.Read(&item.second.globalTransformation, sizeof(glm::mat4));

			anim->m_BoneInfoMap.insert(item);
		}

		size_t channels_size;
		file.Read(&channels_size, sizeof(size_t));

		for (unsigned int i = 0; i < channels_size; i++)
		{
			anim->m_Bones.push_back(Bone::LoadWiAnimNode(file));	
		}		
		return anim;
	}
}