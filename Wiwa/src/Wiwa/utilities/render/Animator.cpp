#include <wipch.h>

#include "Animator.h"
#include "Wiwa/utilities/AnimatorManager.h"
#include <Wiwa/utilities/json/JSONDocument.h>
#include <Wiwa/utilities/json/JSONValue.h>
#include <glm/gtx/matrix_interpolation.hpp >
#include <stack>
#include <cmath>
namespace Wiwa
{
	Animator::Animator()
	{
		m_Name = "New Animator";
		m_CurrentTime = 0.0;
		m_CurrentAnimation = nullptr;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}
	Animator::Animator(Animation *animation)
	{
		m_Name = "New Animator";
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	Animator::~Animator()
	{
		m_CurrentAnimation = nullptr;
		m_TargetAnimation = nullptr;

		//for (std::vector<Animation*>::iterator item = m_Animations.begin(); item != m_Animations.end(); item++)
		//{
		//	delete* item;
		//	*item = nullptr;
		//}

		m_Animations.clear();
		m_FinalBoneMatrices.clear();
	}

	void Animator::Update(float dt)
	{
		if (m_CurrentAnimation == nullptr) 
			return;

		//convert miliseconds to seconds
		m_DeltaTime = dt * 0.001f;

		switch (m_AnimationState)
		{
		case AnimationState::Blending:

			if (m_TargetAnimation == nullptr)
				return;

				UpdateBlendingAnimation(m_DeltaTime);

			break;
		case AnimationState::PausedBlending:

			CalculateBlendedBoneTransform(m_CurrentAnimation, m_CurrentAnimation->GetRootNode(),
				m_TargetAnimation, m_TargetAnimation->GetRootNode(),
				m_CurrentAnimation->m_CurrentTime, m_TargetAnimation->m_CurrentTime,
				glm::mat4(1), m_BlendWeight);

			break;
		case AnimationState::Paused:
			if (m_PrevAnimationState == AnimationState::Playing) // if animation was playing before it was paused
			{
				m_PausedTime = m_CurrentAnimation->m_CurrentTime;
				m_CurrentTime = 0;
				m_AnimationState = AnimationState::Paused;
			}
			CalculateBoneTransform(m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
			break;
		case AnimationState::Playing:
			if (m_PrevAnimationState == AnimationState::Paused) // if animation was paused before it was played
			{
				m_CurrentAnimation->m_CurrentTime = m_PausedTime;
				m_CurrentTime = 0;
			}
			UpdateAnimation(m_DeltaTime);
			break;
		default:
			break;
		}
		m_PrevAnimationState = m_AnimationState;
	}

	void Animator::UpdateAnimation(float dt)
	{
		// update animation time
		float TicksPerSecond = (float)(m_CurrentAnimation->m_TicksPerSecond != 0 ? m_CurrentAnimation->m_TicksPerSecond : 25.0f);
		
		float framesPerDeltaTime = dt * TicksPerSecond;

		m_CurrentTime += framesPerDeltaTime;

		m_CurrentAnimation->m_CurrentTime = fmod(m_CurrentTime, (float)m_CurrentAnimation->m_Duration);

		if (m_CurrentTime >= m_CurrentAnimation->GetDuration())
		{

			//give a frame to check if the animation has finished
			if (!m_CurrentAnimation->m_HasFinished)
			{
				m_CurrentAnimation->m_HasFinished = true;
				return;
			}

			if (m_CurrentAnimation->m_Loop)
			{
				m_CurrentAnimation->m_HasFinished = false;
				m_CurrentTime = 0;
			}
			else {
				m_AnimationState = AnimationState::Paused;
				m_CurrentAnimation->m_CurrentTime = m_CurrentAnimation->GetDuration();
			}

			return;
		}
		CalculateBoneTransform(m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}

	void Animator::UpdateBlendingAnimation(float dt)
	{
		//update blending parameters
		if (m_BlendTime < 0)
		{
			if (m_LoopBlend)
			{
				m_BlendTime = m_BlendDuration;
			}
			else {
				m_AnimationState = AnimationState::Playing;
				m_CurrentAnimation = m_TargetAnimation;
				return;
			}			
		}

		m_BlendTime -= dt;
		m_BlendWeight = 0 + m_BlendTime * (1 - 0);
		
		BlendTwoAnimations(m_CurrentAnimation, m_TargetAnimation, m_BlendWeight, dt);
	}

	void Animator::Blend(std::string name, float blendTime)
	{
		m_TargetAnimation = GetAnimation(name);
		m_BlendDuration = blendTime;
		m_BlendTime = blendTime;
		m_AnimationState = AnimationState::Blending;
	}

	void Animator::SetBlendDuration(float blendDuration)
	{
		m_BlendDuration = blendDuration;
	}

	void Animator::SetBlendTime(float blendTime)
	{
		m_BlendTime = blendTime;
	}

	void Animator::PlayAnimation(Animation *pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0;
		m_CurrentAnimation->m_CurrentTime = 0;
	}

	void Animator::PlayAnimation(std::string name, bool loop, bool transition, float transitionTime)
	{
		m_CurrentTime = 0;
		//its the same anim just reset the animation with the new parameters
		if ( m_CurrentAnimation != nullptr && strcmp(m_CurrentAnimation->m_Name.c_str(), name.c_str()) == 0)
		{
			m_CurrentAnimation->m_CurrentTime = 0;
			m_AnimationState = AnimationState::Playing;
			m_CurrentAnimation->m_Loop = loop;
			return;
		}
		//find the animation and given the parameters blend or not and or loop
		for (auto &animation : m_Animations)
		{
			if (strcmp(animation->m_Name.c_str(), name.c_str()) == 0)
			{
				if (m_CurrentAnimation == nullptr)
				{
					m_CurrentAnimation = animation.get();
					m_CurrentAnimation->m_CurrentTime = 0;
					m_BlendTime = 0;
					m_CurrentAnimation->m_Loop = loop;
					m_CurrentAnimation->m_HasFinished = false;
					m_AnimationState = AnimationState::Playing;
					return;
				}
				if (transition)
				{
					if(m_CurrentAnimation == nullptr)
						m_CurrentAnimation = animation.get();

					m_TargetAnimation = animation.get();
					m_BlendDuration = transitionTime;
					m_BlendTime = 0;
					m_TargetAnimation->m_CurrentTime = 0;
					m_TargetAnimation->m_Loop = loop;
					m_TargetAnimation->m_HasFinished = false;
					m_LoopBlend = false;
					m_AnimationState = AnimationState::Blending;
				}
				else
				{
					m_CurrentAnimation = animation.get();
					m_CurrentAnimation->m_CurrentTime = 0;
					m_BlendTime = 0;
					m_CurrentAnimation->m_Loop = loop;
					m_CurrentAnimation->m_HasFinished = false;
					m_AnimationState = AnimationState::Playing;
				}

				return;
			}
		}
	}


	void Animator::SaveWiAnimator(Animator& animator, const char *path)
	{
		std::filesystem::path filePath = path;
		std::string name = filePath.filename().string();
		filePath.replace_filename(name);
		filePath.replace_extension();
		filePath += ".wianimator";
		JSONDocument doc;
		doc.AddMember("fileFormatVersion", 1);
		doc.AddMember("file", path);
		doc.AddMember("folderAsset", false);
		JSONValue animations = doc.AddMemberArray("animations");

		for (auto &anim : animator.m_Animations)
		{
			animations.PushBack(anim->m_SavePath.c_str());
		}

		doc.save_file(filePath.string().c_str());
	}

	Animator *Animator::LoadWiAnimator(const char *path)
	{
		std::filesystem::path filePath = path;
		if (!std::filesystem::exists(filePath))
			return nullptr;
		Animator *animator = new Animator();
		JSONDocument doc(filePath.string().c_str());
		if (doc.HasMember("animations"))
		{
			JSONValue animations = doc["animations"];
			if (animations.IsArray())
			{
				for (uint32_t i = 0; i < animations.Size(); i++)
				{
					std::string path = animations[i].as_string();
					if (!FileSystem::Exists(path.c_str()))
						continue;
					//animator->m_Animations.push_back(Animation::LoadWiAnimation(path.c_str()));
					animator->m_Animations.push_back(Wiwa::AnimatorManager::GetAnimation(path.c_str()));
				}
			}
		}
		return animator;
	}

	void Animator::PlayAnimationName(std::string name, bool loop)
	{
		if (m_CurrentAnimation)
		{
			if (strcmp(m_CurrentAnimation->m_Name.c_str(), name.c_str()) == 0)
			{
				m_CurrentAnimation->m_Loop = loop;
				m_AnimationState = AnimationState::Playing;
				return;
			}
		}

		for (auto &animation : m_Animations)
		{
			if (strcmp(animation->m_Name.c_str(), name.c_str()) == 0)
			{
				m_CurrentAnimation = animation.get();
				m_CurrentAnimation->m_Loop = loop;
				m_AnimationState = AnimationState::Playing;
				return;
			}
		}
	}

	void Animator::PlayAnimation()
	{
		m_AnimationState = AnimationState::Playing;
		m_CurrentTime = 0;
		if (m_CurrentAnimation != nullptr)
			m_CurrentAnimation->m_CurrentTime = 0;
	}

	void Animator::PauseAnimation()
	{
		m_AnimationState = AnimationState::Paused;
	}

	void Animator::BlendTwoAnimations(Animation *baseAnim, Animation *layerAnim, float blendFactor, float deltaTime)
	{
		// Speed multipliers to correctly transition from one animation to another
		float a = 1.0f;
		float b = baseAnim->GetDuration() / layerAnim->GetDuration();
		const float animSpeedMultiplierUp = (1.0f - blendFactor) * a + b * blendFactor; // Lerp

		a = layerAnim->GetDuration() / baseAnim->GetDuration();
		b = 1.0f;
		const float animSpeedMultiplierDown = (1.0f - blendFactor) * a + b * blendFactor; // Lerp

		// Current time of each animation, "scaled" by the above speed multiplier variables
		static float currentTimeBase = 0.0f;
		float TicksPerSecond = (float)(m_CurrentAnimation->m_TicksPerSecond != 0 ? m_CurrentAnimation->m_TicksPerSecond : 25.0f);
		float TimeInTicks = deltaTime * TicksPerSecond * animSpeedMultiplierUp;
		float AnimationTimeTicks = fmod(TimeInTicks, (float)m_CurrentAnimation->m_Duration);
		m_CurrentAnimation->m_CurrentTime = AnimationTimeTicks;

		static float currentTimeLayered = 0.0f;
		TicksPerSecond = (float)(m_TargetAnimation->m_TicksPerSecond != 0 ? m_TargetAnimation->m_TicksPerSecond : 25.0f);
		TimeInTicks = deltaTime * TicksPerSecond * animSpeedMultiplierDown;
		AnimationTimeTicks = fmod(TimeInTicks, (float)m_TargetAnimation->m_Duration);
		m_TargetAnimation->m_CurrentTime = AnimationTimeTicks;

		CalculateBlendedBoneTransform(baseAnim, baseAnim->GetRootNode(), layerAnim, layerAnim->GetRootNode(), currentTimeBase, currentTimeLayered, glm::mat4(1.0f), blendFactor);
	}

	void Animator::CalculateBlendedBoneTransform(Animation *animationBase, const NodeData *node, Animation *animationLayer, const NodeData *nodeLayered, const float currentTimeBase, const float currentTimeLayered, const glm::mat4 &parentTransform, const float blendFactor)
	{
		const std::string &nodeName = node->name;

		glm::mat4 nodeTransform = node->transformation;
		Bone *pBone = animationBase->FindBone(nodeName);
		if (pBone)
		{
			pBone->Update(m_CurrentAnimation->m_CurrentTime);
			nodeTransform = pBone->GetLocalTransform();
		}

		glm::mat4 layeredNodeTransform = nodeLayered->transformation;
		pBone = animationLayer->FindBone(nodeName);
		if (pBone)
		{
			pBone->Update(m_TargetAnimation->m_CurrentTime);
			layeredNodeTransform = pBone->GetLocalTransform();
		}

		// Blend two matrices
		const glm::quat rot0 = glm::quat_cast(nodeTransform);
		const glm::quat rot1 = glm::quat_cast(layeredNodeTransform);
		const glm::quat finalRot = glm::slerp(rot0, rot1, blendFactor);
		glm::mat4 blendedMat = glm::mat4_cast(finalRot);
		blendedMat[3] = (1.0f - blendFactor) * nodeTransform[3] + layeredNodeTransform[3] * blendFactor;

		glm::mat4 globalTransformation = parentTransform * blendedMat;

		const auto &boneInfoMap = animationBase->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			const int index = boneInfoMap.at(nodeName).id;
			const glm::mat4 &offset = boneInfoMap.at(nodeName).offsetMatrix;

			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (size_t i = 0; i < node->children.size(); ++i)
			CalculateBlendedBoneTransform(animationBase, node->children[i], animationLayer, nodeLayered->children[i], currentTimeBase, currentTimeLayered, globalTransformation, blendFactor);
	}

	void Animator::CalculateBoneFinalTransform()
	{
		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		for (auto &bone : m_CurrentAnimation->GetBones())
		{
			if (boneInfoMap.find(bone->m_Name) != boneInfoMap.end())
			{
				// interpolate local bone matrix
				bone->Update(m_CurrentAnimation->m_CurrentTime);

				int index = boneInfoMap[bone->m_Name].id;

				glm::mat4 globalTransform = boneInfoMap[bone->m_Name].globalTransformation;
				glm::mat4 localTransform = bone->m_LocalTransform;
				glm::mat4 offsetMatrix = boneInfoMap[bone->m_Name].offsetMatrix;
				m_FinalBoneMatrices[index] = globalTransform * localTransform * offsetMatrix;
			}
		}
	}

	void Animator::CalculateBoneTransform(const NodeData *rootNode, const glm::mat4 parentTransform)
	{
		std::string nodeName = rootNode->name;
		glm::mat4 nodeTransform = rootNode->transformation;

		Bone *Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentAnimation->m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			m_FinalBoneMatrices[index] = globalTransformation * boneInfoMap[nodeName].offsetMatrix;
		}

		for (int i = 0; i < rootNode->childrenCount; i++)
			CalculateBoneTransform(rootNode->children[i], globalTransformation);
	}
	Animation *Animator::GetAnimation(std::string name)
	{
		for (auto &animation : m_Animations)
		{
			if (strcmp(animation->m_Name.c_str(), name.c_str()) == 0)
				return animation.get();
		}
	}
}
