#include "wipch.h"
#include "AnimatorManager.h"
#include "Wiwa/utilities/render/Animation.h"


namespace Wiwa {
	std::unordered_map<std::string, std::shared_ptr<Animation>> AnimatorManager::m_Animations{};
	void AnimatorManager::CleanUp()
	{
		AnimatorManager::m_Animations.clear();
	}
	std::shared_ptr<Animation> AnimatorManager::GetAnimation(const char* file_path)
	{
		auto animation = m_Animations.find(file_path);

		if (animation != m_Animations.end())
			return animation->second;
		else
		{
			std::shared_ptr<Animation> loadedAnim(Wiwa::Animation::LoadWiAnimation(file_path));
			StoreAnimation(loadedAnim);
			return loadedAnim;
		}

		return nullptr;
	}
	void AnimatorManager::StoreAnimation(std::shared_ptr<Animation> animation)
	{
		m_Animations[animation->m_SavePath] = animation;
	}
}