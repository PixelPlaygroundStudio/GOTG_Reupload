#pragma once
#include <queue>
#include <memory>
#include <vector>
#include <string>

namespace Wiwa
{
	class Animation;
	class WI_API AnimatorManager
	{
	public:
		static void CleanUp();
		static std::shared_ptr<Animation> GetAnimation(const char* file_path);
		static void StoreAnimation(std::shared_ptr<Animation> animation);
		
	private:
		static std::unordered_map<std::string, std::shared_ptr<Animation>> m_Animations;
	};
}