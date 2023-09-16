#pragma once
#include "resources_impl.h"

namespace Wiwa {
	//--SPECIALIZATION FOR MODEL
	template<>
	inline void Resources::LoadMeta<Animation>(const char* file, ModelSettings* settings)
	{
		std::filesystem::path filePath = file;
		filePath += ".meta";
		if (!std::filesystem::exists(filePath))
			return;
	}
	template<>
	inline void Resources::CreateMeta<Animation>(const char* file, ModelSettings* settings)
	{
		std::filesystem::path filePath = file;
		filePath += ".meta";
		JSONDocument doc;
		doc.AddMember("fileFormatVersion", 1);
		doc.AddMember("file", file);
		doc.AddMember("folderAsset", false);
		std::time_t time = to_time_t(std::filesystem::last_write_time(file));
		doc.AddMember("timeCreated", time);
		doc.save_file(filePath.string().c_str());
	}
	template<>
	inline ResourceId Resources::LoadNative<Animation>(const char* file)
	{
		ResourceId position = getResourcePosition(WRT_ANIMATION, file);
		size_t size = m_Resources[WRT_ANIMATION].size();

		ResourceId resourceId;

		if (position == size) {
			Animation* animation = new Animation(file);

			PushResource(WRT_ANIMATION, file, animation, true);

			resourceId = size;
		}
		else {
			resourceId = position;
		}

		return resourceId;
	}
	template<>
	inline ResourceId Resources::Load<Animation>(const char* file)
	{
		std::filesystem::path file_path = _assetToLibPath(file);
		file_path.replace_extension(".wianim");
		ResourceId position = getResourcePosition(WRT_ANIMATION, file_path.string().c_str());
		size_t size = m_Resources[WRT_ANIMATION].size();

		ResourceId resourceId;

		if (position == size) {
			Animation* animation = new Animation();

			animation->LoadWiAnimation(file_path.string().c_str());

			PushResource(WRT_ANIMATION, file_path.string().c_str(), animation);

			resourceId = size;
		}
		else {
			resourceId = position;
		}

		return resourceId;
	}
	template<>
	inline Animation* Resources::GetResourceById<Animation>(ResourceId id)
	{
		Animation* animation= NULL;

		if (id >= 0 && id < m_Resources[WRT_MODEL].size()) {
			animation = static_cast<Animation*>(m_Resources[WRT_ANIMATION][id]->resource);
		}

		return animation;
	}
	template<>
	inline bool Resources::Import<Animation>(const char* file, ModelSettings* settings)
	{
		if (!_file_exists(file)) return false;

		std::filesystem::path import_path = file;
		import_path = _import_path_impl(import_path, ".wianim");


		_import_model_impl(file, import_path.string().c_str(), settings);

		WI_CORE_INFO("Model at {} imported succesfully!", import_path.string().c_str());

		return true;
	}

	template<>
	inline bool Resources::CheckImport<Animation>(const char* file)
	{
		return _check_import_impl(file, ".wianim");
	}

	template<>
	inline const char* Resources::getResourcePathById<Animation>(size_t id)
	{
		return getPathById(WRT_ANIMATION, id);
	}

	template<>
	inline void Resources::UnloadResourcesOf<Animation>() {
		std::vector<Resource*>& rvec = m_Resources[WRT_ANIMATION];
		size_t count = rvec.size();

		for (size_t i = 0; i < count; i++) {
			if (!rvec[i]->isNative) {
				Animation* mat = (Animation*)rvec[i]->resource;

				delete mat;

				rvec.erase(rvec.begin() + i);
				i--;
				count--;
			}
		}
	}
}