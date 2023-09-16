#pragma once
#include "resources_impl.h"

namespace Wiwa {
	template<>
	inline void Resources::LoadMeta<Video>(const char* file)
	{
		std::filesystem::path filePath = file;
		if (!std::filesystem::exists(filePath))
			return;
		filePath += ".meta";
		JSONDocument doc(filePath.string().c_str());
		doc.save_file(filePath.string().c_str());
	}
	template<>
	inline void Resources::CreateMeta<Video>(const char* file)
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
	inline ResourceId Resources::Load<Video>(const char* file)
	{
		std::filesystem::path file_path = _assetToLibPath(file);
		ResourceId position = getResourcePosition(WRT_VIDEO, file_path.string().c_str());
		size_t size = m_Resources[WRT_VIDEO].size();

		ResourceId resourceId;

		if (position == size) {
			Video* video = new Video();

			video->Init(file_path.string().c_str(),nullptr);

			PushResource(WRT_VIDEO, file_path.string().c_str(), video);

			resourceId = size;
		}
		else {
			resourceId = position;
		}

		return resourceId;
	}
	template<>
	inline ResourceId Resources::LoadNative<Video>(const char* file)
	{
		std::string path = file;
		standarizePath(path);
		ResourceId position = getResourcePosition(WRT_VIDEO, path.c_str());
		size_t size = m_Resources[WRT_VIDEO].size();
		ResourceId resourceId;

		if (position == size) {
			Video* video = new Video();
			video->Init(path.c_str(),nullptr);

			PushResource(WRT_VIDEO, path.c_str(), video, true);

			resourceId = size;
		}
		else {
			resourceId = position;
		}

		return resourceId;
	}
	template<>
	inline Video* Resources::GetResourceById<Video>(ResourceId id)
	{
		Video* video = NULL;

		if (id >= 0 && id < m_Resources[WRT_VIDEO].size()) {
			video = static_cast<Video*>(m_Resources[WRT_VIDEO][id]->resource);
		}

		return video;
	}

	template<>
	inline bool Resources::Import<Video>(const char* file)
	{
		if (std::filesystem::is_directory(file)) return false;
		if (!_file_exists(file)) return false;

		std::filesystem::path import_path = _import_path_impl(file, ".mp4");
		_import_video_impl(file, import_path.string().c_str());

		WI_CORE_INFO("Video at {} imported succesfully!", import_path.string().c_str());
		return true;
	}
	template<>
	inline bool Resources::CheckImport<Video>(const char* file)
	{
		return _check_import_impl(file, ".mp4");
	}
	template<>
	inline const char* Resources::getResourcePathById<Video>(size_t id)
	{
		return getPathById(WRT_VIDEO, id);
	}

	template<>
	inline void Resources::UnloadResourcesOf<Video>() {
		std::vector<Resource*>& rvec = m_Resources[WRT_VIDEO];
		size_t count = rvec.size();

		for (size_t i = 0; i < count; i++) {
			if (!rvec[i]->isNative) {
				Video* video = (Video*)rvec[i]->resource;

				delete video;

				rvec.erase(rvec.begin() + i);
				i--;
				count--;
			}
		}
	}
}