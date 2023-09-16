#include "InspectorPanel.h"

#include <Wiwa/scene/SceneManager.h>
#include <Wiwa/scene/Scene.h>

#include <imgui_internal.h>
#include <Wiwa/core/Application.h>
#include <Wiwa/core/Resources.h>
#include <Wiwa/utilities/render/Material.h>
#include <Wiwa/utilities/math/Vector2i.h>
#include "../../Utils/EditorUtils.h"

#include <Wiwa/ecs/components/Mesh.h>
#include <Wiwa/ecs/components/AudioSource.h>
#include <Wiwa/ecs/components/PointLight.h>
#include <Wiwa/ecs/components/SpotLight.h>
#include <Wiwa/ecs/components/DirectionalLight.h>
#include <Wiwa/ecs/components/AnimatorComponent.h>
#include <Wiwa/ecs/components/CollisionBody.h>
#include <Wiwa/ecs/systems/ParticleSystem.h>
#include <Wiwa/ecs/components/game/items/Item.h>
#include <Wiwa/ecs/components/ai/NavMesh.h>
#include <Wiwa/ecs/components/OzzAnimatorCmp.h>
#include <Wiwa/ecs/components/game/player/PlayerSpawnerData.h>

bool InspectorPanel::s_EntitySet = false;

bool InspectorPanel::DrawComponent(size_t componentId)
{
	bool ret = true;

	Wiwa::EntityManager &em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();

	const Type *type = em.GetComponentType(componentId);

	std::string name = type->name;

	if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::string del_label = "Delete##comp";
		del_label += name.c_str();
		if (!(type->hash == (size_t)TypeHash::Transform3D))
		{
			if (ImGui::Button(del_label.c_str()))
			{
				ret = false;
			}
		}

		byte *data = em.GetComponent(m_CurrentID, componentId, type->size);

		// Custom component interface
		if (type->hash == (size_t)TypeHash::Mesh) {	DrawMeshComponent(data); } else
		if (type->hash == (size_t)TypeHash::Transform3D) { DrawTransform3dComponent(data); } else
		if (type->hash == (size_t)TypeHash::AudioSource) { DrawAudioSourceComponent(data); } else
		if (type->hash == (size_t)TypeHash::PointLight) { DrawPointLightComponent(data); } else
		if (type->hash == (size_t)TypeHash::DirectionalLight) { DrawDirectionalLightComponent(data); } else
		if (type->hash == (size_t)TypeHash::SpotLight) { DrawSpotLightComponent(data); } else
		if (type->hash == (size_t)TypeHash::CollisionBody) { DrawCollisionBodyComponent(data); } else
		if (type->hash == (size_t)TypeHash::ColliderCube) { DrawColliderCubeComponent(data); } else
		if (type->hash == (size_t)TypeHash::ColliderSphere) { DrawColliderSpehereComponent(data); } else
		if (type->hash == (size_t)TypeHash::ColliderCylinder) { DrawColliderCylinderComponent(data); } else
		if (type->hash == (size_t)TypeHash::RayCast) { DrawRayCastComponent(data); } else
		if (type->hash == (size_t)TypeHash::AnimatorComponent) { DrawAnimatorComponent(data); } else
		if (type->hash == (size_t)TypeHash::ParticleEmitter) { DrawParticleSystemComponent(data); } else
		if (type->hash == (size_t)TypeHash::Item) { DrawItemComponent(data); }else
		if (type->hash == (size_t)TypeHash::NavMesh) { DrawNavMeshComponent(data); }else
		if (type->hash == FNV1A_HASH("OzzAnimatorCmp")) { DrawOzzAnimatorCmp(data); } else
		if (type->hash == (size_t)TypeHash::PlayerSpawner) { DrawPlayerSpawnerComponent(data); } else
		// Basic component interface
		if (type->is_class) {
			const Class* cl = (const Class*)type;

			for (size_t i = 0; i < cl->fields.size(); i++)
			{
				DrawField(data, cl->fields[i]);
			}
		}
		
		ImGui::TreePop();
	}

	return ret;
}

void InspectorPanel::DrawField(unsigned char *data, const Field &field)
{
	// Draw class field
	if (field.type->is_class)
	{
		const Class *cl = (const Class *)field.type;

		std::string name = field.name;

		if (ImGui::TreeNodeEx(name.c_str()))
		{
			for (size_t i = 0; i < cl->fields.size(); i++)
			{
				DrawField(data + field.offset, cl->fields[i]);
			}
			ImGui::TreePop();
		}
		return;
	}

	// Draw enum field
	if (field.type->is_enum)
	{
		const Enum *en = (const Enum *)field.type;

		ImGui::Text(field.name.c_str());
		ImGui::PushID(field.name.c_str());

		// TODO: DRAW LIST OF ENUMS TO CHOOSE FROM

		ImGui::PopID();
		return;
	}
	if (field.type->hash == (size_t)TypeHash::Bool)
	{
		ImGui::PushID(field.name.c_str());
		ImGui::Checkbox(field.name.c_str(), (bool*)(data + field.offset));
		ImGui::PopID();
	}
	// Draw basic fields
	if (field.type->hash == (size_t)TypeHash::Vector2i)
	{
		ImGui::PushID(field.name.c_str());
		DrawInt2Control(field.name.c_str(), data, field);
		ImGui::PopID();
	}
	else if (field.type->hash == (size_t)TypeHash::Float)
	{
		ImGui::Text(field.name.c_str());
		ImGui::PushID(field.name.c_str());
		ImGui::InputFloat("", (float *)(data + field.offset));
		ImGui::PopID();
	}
	else if (field.type->hash == (size_t)TypeHash::Vector2f)
	{
		ImGui::PushID(field.name.c_str());
		DrawVec2Control(field.name.c_str(), data, field);
		ImGui::PopID();
	}
	else if (field.type->hash == (size_t)TypeHash::Vector3f)
	{
		ImGui::PushID(field.name.c_str());
		DrawVec3Control(field.name.c_str(), data, field, 0.0f);
		ImGui::PopID();
	}
	else if (field.type->hash == (size_t)TypeHash::UInt64)
	{
		ImGui::Text(field.name.c_str());
		ImGui::PushID(field.name.c_str());
		ImGui::InputInt("", (int *)(data + field.offset));
		ImGui::PopID();
	}
	else if (field.type->hash == (size_t)TypeHash::Int32)
	{
		ImGui::Text(field.name.c_str());
		ImGui::PushID(field.name.c_str());
		ImGui::InputInt("", (int *)(data + field.offset));
		ImGui::PopID();
	}
	else if (field.type->hash == (size_t)TypeHash::Rect2i)
	{
		ImGui::PushID(field.name.c_str());
		DrawRect2Control(field.name.c_str(), data, field);
		ImGui::PopID();
	}
	else if (field.type->hash == (size_t)TypeHash::Pivot)
	{
		ImGui::Text(field.name.c_str());
		ImGui::PushID(field.name.c_str());
		ImGui::InputInt("", (int*)(data + field.offset));
		ImGui::PopID();
	}
}

void InspectorPanel::DrawCollisionTags()
{
	Wiwa::PhysicsManager &py = Wiwa::SceneManager::getActiveScene()->GetPhysicsManager();

	if (ImGui::Button("Edit Tags"))
		ImGui::OpenPopup("Edit Tags");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Edit Tags", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static int selected = -1;
		for (const auto& [key, value] : py.filterMapStringKey)
		{
			/*std::string tempStr = py.GetFilterTag(n);
			int bits = 1 << n;
			tempStr += "_" + std::to_string(bits);*/
			std::string tagNameWitBits = key;
			tagNameWitBits += "_" + std::to_string(value);
			if (ImGui::Selectable(tagNameWitBits.c_str(), selected == value, ImGuiSelectableFlags_DontClosePopups))
				selected = value;
		}
		/*for (int n = 0; n < py.filterMapStringKey.size(); n++)
		{
			std::string tempStr = py.GetFilterTag(n);
			int bits = 1 << n;
			tempStr += "_" + std::to_string(bits);
			if (ImGui::Selectable(tempStr.c_str(), selected == n, ImGuiSelectableFlags_DontClosePopups))
				selected = n;
		}*/

		static char strBuf[64] = "";
		ImGui::InputText("Write Tag", strBuf, 64, ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank);

		if (ImGui::Button("Add", ImVec2(120, 0)))
		{
			if (!(py.filterMapStringKey.find(strBuf) != py.filterMapStringKey.end()))
			{
				py.AddFilterTag(strBuf);
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Remove", ImVec2(120, 0)))
		{
			// If only index > 0, because we don't want to erase the basic tag
			if (selected > 0)
				py.RemoveFilterTag(selected);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Save Tags", ImVec2(120, 0)))
		{
			// Save Physics Manager json Data
			py.OnSave();
		}
		ImGui::EndPopup();
	}

	// Using the generic BeginCombo() API, you have full control over how to display the combo contents.
	// (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
	// stored in the object itself, etc.)
	// const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
	// static int itemCurrentIdx = 0; // Here we store our selection data as an index.
	// const char* comboPreviewValue = py.filterStrings[itemCurrentIdx].c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
	// if (ImGui::BeginCombo("Collision Tags", comboPreviewValue))
	//{
	//	for (int n = 0; n < py.filterStrings.size(); n++)
	//	{
	//		const bool is_selected = (itemCurrentIdx == n);
	//		if (ImGui::Selectable(py.filterStrings[n].c_str(), is_selected))
	//			itemCurrentIdx = n;

	//		// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
	//		if (is_selected)
	//			ImGui::SetItemDefaultFocus();
	//	}

	//

	//	ImGui::EndCombo();
	//}
}

void InspectorPanel::DrawMeshComponent(byte *data)
{
	Wiwa::Mesh *mesh = (Wiwa::Mesh *)data;

	// Draw meshId field
	ImGui::Text("Model");
	ImGui::PushID("meshId");

	Wiwa::Model *mod = Wiwa::Resources::GetResourceById<Wiwa::Model>(mesh->meshId);
	mod = mod->getModelAt(mesh->modelIndex);

	AssetContainer(std::filesystem::path(mod->getModelPath()).stem().string().c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t *path = (const wchar_t *)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".fbx" || p.extension() == ".FBX")
			{
				WI_INFO("Trying to load payload at path {0}", pathS.c_str());
				p.replace_extension();
				std::string src = Wiwa::FileSystem::RemoveFolderFromPath("assets", p.string());
				mesh->meshId = Wiwa::Resources::Load<Wiwa::Model>(src.c_str());
				mesh->modelIndex = 0;
				mesh->drawChildren = true;

				Wiwa::Model *m = Wiwa::Resources::GetResourceById<Wiwa::Model>(mesh->meshId);
			}
		}

		ImGui::EndDragDropTarget();
	}
	ImGui::Text("Model path: ");
	ImGui::SameLine();
	ImGui::Text(mod->getModelPath());
	ImGui::Text("Bounding box");
	ImGui::InputFloat3("Max", (float *)glm::value_ptr(mod->boundingBox.getMax()));
	ImGui::InputFloat3("Min", (float *)glm::value_ptr(mod->boundingBox.getMin()));
	static bool showNormals = false;
	if (ImGui::Checkbox("Show normals", &showNormals))
		mod->showNormals = showNormals;
	ImGui::PopID();

	// Draw materialId field
	ImGui::Text("Material");
	Wiwa::Material *mat = Wiwa::Resources::GetResourceById<Wiwa::Material>(mesh->materialId);
	AssetContainer(std::filesystem::path(mat->getMaterialPath()).stem().string().c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t *path = (const wchar_t *)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".wimaterial")
			{
				WI_INFO("Trying to load payload at path {0}", pathS.c_str());
				mesh->materialId = Wiwa::Resources::Load<Wiwa::Material>(pathS.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}
	ImGui::PushID("materialId");
	ImGui::Text("Material at: ");
	ImGui::SameLine();
	ImGui::Text(mat->getMaterialPath());

	ImGui::Checkbox("Cast shadows", &mesh->castShadows);
	ImGui::Checkbox("Recieve shadows", &mesh->shadowRecieve);
	ImGui::PopID();
}

void InspectorPanel::DrawTransform3dComponent(byte *data)
{
	Wiwa::Transform3D *transform = (Wiwa::Transform3D *)data;
	DrawVec3Control("Position", &transform->localPosition, 0.0f, 100.0f);
	DrawVec3Control("Rotation", &transform->localRotation, 0.0f, 100.0f);
	DrawVec3Control("Scale", &transform->localScale, 1.0f, 100.0f);
}

void InspectorPanel::DrawAudioSourceComponent(byte *data)
{
	Wiwa::AudioSource *asrc = (Wiwa::AudioSource *)data;

	ImGui::Checkbox("Is default listener", &asrc->isDefaultListener);
	ImGui::InputText("Event", asrc->eventName, sizeof(asrc->eventName));
	ImGui::Checkbox("Play on awake", &asrc->playOnAwake);
}

void InspectorPanel::DrawPointLightComponent(byte *data)
{
	Wiwa::PointLight *lsrc = (Wiwa::PointLight *)data;

	ImGui::ColorEdit3("Color", glm::value_ptr(lsrc->Color));
	ImGui::InputFloat("Ambient Intensity", &lsrc->AmbientIntensity, 0.05f, 0.5f, "%.2f");
	ImGui::InputFloat("Diffuse Intensity", &lsrc->DiffuseIntensity, 0.05f, 0.5f, "%.2f");
	ImGui::SliderFloat("Constant", &lsrc->Constant, 0.001f, 1.0f);
	ImGui::SliderFloat("Linear", &lsrc->Linear, 0.001f, 1.0f);
	ImGui::SliderFloat("Exponential", &lsrc->Exp, 0.001f, 1.0f);
}

void InspectorPanel::DrawDirectionalLightComponent(byte *data)
{
	Wiwa::DirectionalLight *lsrc = (Wiwa::DirectionalLight *)data;

	ImGui::ColorEdit3("Color", glm::value_ptr(lsrc->Color));
	ImGui::InputFloat("Ambient intensity", &lsrc->AmbientIntensity, 0.5f, 1.0f, "%.2f");
	ImGui::InputFloat("Diffuse intensity", &lsrc->DiffuseIntensity, 0.5f, 1.0f, "%.2f");
}
// TODO: Implement when ready
void InspectorPanel::DrawSpotLightComponent(byte *data)
{
	Wiwa::SpotLight *lsrc = (Wiwa::SpotLight *)data;

	ImGui::ColorEdit3("Color", glm::value_ptr(lsrc->Color));
	ImGui::InputFloat("Ambient Intensity", &lsrc->AmbientIntensity, 0.5f, 1.0f, "%.2f");
	ImGui::InputFloat("Diffuse Intensity", &lsrc->DiffuseIntensity, 0.5f, 1.0f, "%.2f");
	ImGui::SliderFloat("Constant", &lsrc->Constant, 0.001f, 1.0f);
	ImGui::SliderFloat("Linear", &lsrc->Linear, 0.001f, 1.0f);
	ImGui::SliderFloat("Exponential", &lsrc->Exp, 0.001f, 1.0f);
	DrawVec3Control("Direction", &lsrc->Direction);
	ImGui::InputFloat("Cutoff", &lsrc->Cutoff);
}

void InspectorPanel::DrawAnimatorComponent(byte *data)
{
	Wiwa::AnimatorComponent *animator = (Wiwa::AnimatorComponent *)data;

	AssetContainer(animator->filePath);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".wianimator")
			{
				WI_INFO("Trying to load payload at path {0}", pathS.c_str());
				pathS =	Wiwa::Resources::_assetToLibPath(pathS);
				strcpy(animator->filePath, pathS.c_str());
				animator->animator = Wiwa::Animator::LoadWiAnimator(pathS.c_str());
				return;
			}
		}

		ImGui::EndDragDropTarget();
	}
	if (animator->animator == nullptr)
		return;

	// get animaitons
	ImGui::Separator();
	const char* animationBaseItems[25];

	for (unsigned int i = 0; i < animator->animator->m_Animations.size(); i++)
	{
		animationBaseItems[i] = animator->animator->m_Animations[i]->m_Name.c_str();
	}
	animationBaseItems[animator->animator->m_Animations.size()] = "none";

	const char* current_item = NULL;
	if (animator->animator->GetCurrentAnimation() != nullptr)
		current_item = animator->animator->GetCurrentAnimation()->m_Name.c_str();
	ImGui::Text("Base animation");
	if (ImGui::BeginCombo("base", current_item))
	{
		for (int n = 0; n < animator->animator->m_Animations.size(); n++)
		{
			bool is_selected = (current_item == animationBaseItems[n]);
			if (ImGui::Selectable(animationBaseItems[n], is_selected))
			{
				current_item = animationBaseItems[n];
				ImGui::SetItemDefaultFocus();
				if (current_item == "none")
				{
					animator->animator->SetCurrentAnimation(nullptr);
				}
				else {

					animator->animator->SetCurrentAnimation(animator->animator->m_Animations[n].get());
					animator->animator->GetCurrentAnimation()->m_Loop = animator->Loop;
				}
				
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Separator();
	const char* animationLayerItems[25];

	for (unsigned int i = 0; i < animator->animator->m_Animations.size(); i++)
	{
		animationLayerItems[i] = animator->animator->m_Animations[i]->m_Name.c_str();
	}

	animationLayerItems[animator->animator->m_Animations.size()] = "none";
	
	const char* current_item2 = NULL;

	if (animator->animator->GetTargetAnimation() != nullptr)
		current_item2 = animator->animator->GetTargetAnimation()->m_Name.c_str();
	ImGui::Text("Layer animation");
	if (ImGui::BeginCombo("layer", current_item2))
	{
		for (int n = 0; n < animator->animator->m_Animations.size(); n++)
		{
			bool is_selected = (current_item2 == animationLayerItems[n]);
			if (ImGui::Selectable(animationLayerItems[n], is_selected))
			{
				current_item2 = animationLayerItems[n];
				ImGui::SetItemDefaultFocus();
				if (current_item2 == "none")
				{
					animator->animator->SetTargetAnimation(nullptr);
				}
				else {

					animator->animator->SetTargetAnimation(animator->animator->m_Animations[n].get());
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Separator();

	if (!animator->animator->GetCurrentAnimation())
		return;

	if (ImGui::Checkbox("Play", &animator->Play))
	{
		if (animator->Play)
			animator->animator->PlayAnimation();
		else
			animator->animator->PauseAnimation();
	}

	if (ImGui::Checkbox("Loop", &animator->Loop))
	{
		animator->animator->Loop(animator->Loop);
	}
	ImGui::Separator();

	ImGui::Text("frames: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(trunc(animator->animator->GetCurrentAnimation()->GetDuration())).c_str());

	float maxTime = animator->animator->GetCurrentAnimation()->GetDuration()/24;
	ImGui::Text("Base animation time");

	if (ImGui::SliderFloat("base time", &animator->baseAnimTime, 0, maxTime))
	{
		animator->animator->GetCurrentAnimation()->m_CurrentTime = animator->baseAnimTime * animator->animator->GetCurrentAnimation()->m_TicksPerSecond;
	}
	ImGui::Separator();

	if (animator->animator->GetTargetAnimation())
	{
		ImGui::Text("frames: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(trunc(animator->animator->GetTargetAnimation()->GetDuration())).c_str());

		float maxTime = animator->animator->GetTargetAnimation()->GetDuration() / 24;
		ImGui::Text("Layer animation time");

		if (ImGui::SliderFloat("layer time", &animator->layerAnimTime, 0, maxTime))
		{
			animator->animator->GetTargetAnimation()->m_CurrentTime = animator->layerAnimTime * animator->animator->GetTargetAnimation()->m_TicksPerSecond;
		}

	}
	
	ImGui::Separator();
	if (ImGui::Checkbox("Blend", &animator->Blend))
	{
		if(animator->Blend)
			animator->animator->SetAnimationState(Wiwa::AnimationState::Blending);
		else
			animator->animator->SetAnimationState(Wiwa::AnimationState::PausedBlending);
	}
	if (ImGui::Checkbox("Loop Blending", &animator->LoopBlend))
	{
		animator->animator->m_LoopBlend = animator->LoopBlend;
	}
	if(ImGui::SliderFloat("Blend Duration", &animator->blendDuration,0,1))
	{
		animator->animator->SetBlendDuration(animator->blendDuration);
	}
	if (ImGui::SliderFloat("Weight", &animator->weight, 0, 1))
	{
		animator->animator->m_BlendWeight = animator->weight;
	}
}

void InspectorPanel::DrawCollisionBodyComponent(byte* data)
{
	Wiwa::PhysicsManager& py = Wiwa::SceneManager::getActiveScene()->GetPhysicsManager();
	Wiwa::CollisionBody* collisionBody = (Wiwa::CollisionBody*)data;
	DrawVec3Control("Position offset", &collisionBody->positionOffset, 0.0f, 100.0f);
	DrawVec3Control("Scaling offset", &collisionBody->scalingOffset, 0.0f, 100.0f);
	ImGui::Checkbox("Is static?", &collisionBody->isStatic);
	ImGui::Checkbox("Is trigger?", &collisionBody->isTrigger);
	ImGui::Checkbox("Do continuous?", &collisionBody->doContinuousCollision);

	const char* comboPreviewValue = py.GetFilterTag(collisionBody->selfTag);  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("Self Tag", comboPreviewValue))
	{
		for (const auto& [key, value] : py.filterMapStringKey)
		{
			const bool is_selected = (collisionBody->selfTag == value);
			std::string tagNameWitBits = key;
			tagNameWitBits += "_" + std::to_string(value);
			if (ImGui::Selectable(tagNameWitBits.c_str(), is_selected))
				collisionBody->selfTag = value;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		//for (int n = 0; n < py.filterMapStringKey.size(); n++)
		//{
		//	const bool is_selected = (collisionBody->selfTag == n);
		//	if (ImGui::Selectable(py.GetFilterTag(n), is_selected))
		//		collisionBody->selfTag = n;

		//	// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
		//	if (is_selected)
		//		ImGui::SetItemDefaultFocus();
		//}

		ImGui::EndCombo();
	}

	ImGui::Text("Collide with:");
	for (const auto& [key, value] : py.filterMapStringKey)
	{
		bool local = (collisionBody->filterBits >> value) & 1; //Checking a bit
		ImGui::Checkbox(key.c_str(), &local);
		if (local)
			collisionBody->filterBits |= 1 << value;
		else
			collisionBody->filterBits &= ~(1 << value);
		///*std::string tempStr = py.GetFilterTag(n);
		//int bits = 1 << n;
		//tempStr += "_" + std::to_string(bits);*/
		//std::string tagNameWitBits = key;
		//tagNameWitBits += "_" + std::to_string(value);
		//if (ImGui::Selectable(tagNameWitBits.c_str(), selected == value, ImGuiSelectableFlags_DontClosePopups))
		//	selected = value;
	}

	//ImGui::Text("Collide with:");
	//for (int i = 0; i < py.filterMapStringKey.size(); i++)
	//{
	//	bool local = (collisionBody->filterBits >> i) & 1; //Checking a bit
	//	ImGui::Checkbox(py.GetFilterTag(i), &local);
	//	if (local)
	//		collisionBody->filterBits |= 1 << i;
	//	else
	//		collisionBody->filterBits &= ~(1 << i);
	//}

	// ImGui::Text("Do not collide with:");
	// for (int i = 0; i < py.filterStrings.size(); i++)
	//{
	//	bool local = (collisionBody->filterBits >> i) & 1U; //Checking a bit
	//	ImGui::Checkbox(py.filterStrings[i].c_str(), &local);
	//	if (local)
	//		collisionBody->filterBits |= 1UL << i;
	//	else
	//		collisionBody->filterBits &= ~(1UL << i);
	// }
}

void InspectorPanel::DrawColliderCubeComponent(byte *data)
{
	Wiwa::ColliderCube *colliderCube = (Wiwa::ColliderCube *)data;
	DrawVec3Control("HalfExtents", &colliderCube->halfExtents, 0.0f, 100.0f);
}

void InspectorPanel::DrawColliderSpehereComponent(byte *data)
{
	Wiwa::ColliderSphere *colliderSphere = (Wiwa::ColliderSphere *)data;
	ImGui::InputFloat("Radius", &colliderSphere->radius);
}

void InspectorPanel::DrawColliderCylinderComponent(byte *data)
{
	Wiwa::ColliderCylinder *colliderCylinder = (Wiwa::ColliderCylinder *)data;
	ImGui::InputFloat("Radius", &colliderCylinder->radius);
	ImGui::InputFloat("Height", &colliderCylinder->height);
}

void InspectorPanel::DrawRayCastComponent(byte *data)
{
	Wiwa::RayCast *rayCast = (Wiwa::RayCast *)data;
	DrawVec3Control("Ray From", &rayCast->rayFromWorld, 0.0f, 100.0f);
	DrawVec3Control("Ray To", &rayCast->rayToWorld, 0.0f, 100.0f);
	ImGui::Checkbox("Enabled", &rayCast->doRayCasting);
}

void ParticleTab()
{
	ImGui::Dummy(ImVec2(38, 0));
	ImGui::SameLine();
}

void InspectorPanel::DrawParticleSystemComponent(byte* data)
{
	Wiwa::ParticleEmitterComponent* emitter = (Wiwa::ParticleEmitterComponent*)data;
	Wiwa::EntityManager& eman = Wiwa::SceneManager::getActiveScene()->GetEntityManager();

	ImGui::Separator();
	ImGui::Text("Emitter Parameters");
	ImGui::Separator();

	int max_int_val = std::numeric_limits<int>::max();
	float max_float_val = std::numeric_limits<float>::max();

	//set mesh
	ImGui::Text("Particle Mesh Type");
	AssetContainer(emitter->m_meshPath);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".fbx")
			{
				WI_INFO("Trying to load payload at path {0}", pathS.c_str());
				pathS = Wiwa::Resources::_assetToLibPath(pathS);
				strcpy(emitter->m_meshPath, pathS.c_str());
				emitter->m_meshChanged = true;
			}
		}

		ImGui::EndDragDropTarget();
	}


	ImGui::Dummy(ImVec2(0, 4));

	//set material
	ImGui::Text("Particle Material");
	AssetContainer(emitter->m_materialPath);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".wimaterial")
			{
				WI_INFO("Trying to load payload at path {0}", pathS.c_str());
				pathS = Wiwa::Resources::_assetToLibPath(pathS);
				strcpy(emitter->m_materialPath, pathS.c_str());
				emitter->m_materialChanged = true;
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::Separator();

	ImGui::Dummy(ImVec2(0, 4));

	std::string activeParticles = "Active Particles: " + std::to_string(emitter->m_activeParticles);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0.5f, 1));
	ImGui::Text(activeParticles.c_str());
	ImGui::PopStyleColor();

	ImGui::Dummy(ImVec2(0, 4));


	ImGui::Checkbox("##m_active", &emitter->m_active);
	ImGui::SameLine();
	ImGui::Text("Active");

	ImGui::Checkbox("##m_activeOverTime", &emitter->m_activeOverTime);
	ImGui::SameLine();
	ImGui::Text("Active Over Time");

	if (emitter->m_activeOverTime)
	{

		std::string activeTime = "Time Active: " + std::to_string(emitter->m_ActiveTimer);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 1, 1));
		ImGui::Text(activeTime.c_str());
		ImGui::PopStyleColor();

		ImGui::Dummy(ImVec2(0, 4));

		ParticleTab();
		ImGui::Checkbox("##m_startActive", &emitter->m_startActive);
		ImGui::SameLine();
		ImGui::Text("Start Active");


		if ((int)emitter->m_destroyOnFinishActive > 1 || (int)emitter->m_destroyOnFinishActive < 0)
			emitter->m_destroyOnFinishActive = false;

		ImGui::Dummy(ImVec2(0, 4));

		ParticleTab();
		ImGui::Checkbox("##m_destroyOnFinishActive", &emitter->m_destroyOnFinishActive);
		ImGui::SameLine();
		ImGui::Text("Destroy When Time Ends");

		ImGui::Dummy(ImVec2(0, 4));

		if (emitter->m_destroyOnFinishActive)
		{
			ParticleTab();
			ImGui::Checkbox("##m_destroyActiveParticles", &emitter->m_destroyActiveParticles);
			ImGui::SameLine();
			ImGui::Text("Destroy Active Particles");
		}

		ImGui::Dummy(ImVec2(0, 4));

		ParticleTab();
		ImGui::Checkbox("##m_rangedTimeActive", &emitter->m_rangedTimeActive);
		ImGui::SameLine();
		ImGui::Text("Time Acive");

		if (emitter->m_rangedTimeActive)
		{
			ParticleTab();
			ImGui::PushItemWidth(46.f);

			
			if (ImGui::DragFloat("##m_minInitialTimeActive", &emitter->m_minInitialTimeActive, 0.05f, 0.0f, max_float_val, "%.2f"))
				emitter->m_activeTimeChanged = true;
			ImGui::SameLine();
			if (ImGui::DragFloat("##m_maxInitialTimeActive", &emitter->m_maxInitialTimeActive, 0.05f, 0.0f, max_float_val, "%.2f"))
				emitter->m_activeTimeChanged = true;
			ImGui::PopItemWidth();

		}
		else
		{

			ImGui::Dummy(ImVec2(38, 0));
			ImGui::SameLine();
			ImGui::PushItemWidth(100.0f);
			if (ImGui::DragFloat("##m_initialTimeActive", &emitter->m_initialTimeActive, 0.05f, 0.0f, max_float_val, "%.2f")) emitter->m_activeTimeChanged = true;
			ImGui::PopItemWidth();
		}

	}

	ImGui::Dummy(ImVec2(0, 4));

	ImGui::Text("Max Particles");
	ImGui::PushItemWidth(100.0f);

	ImGui::DragInt("##m_maxParticles", &emitter->m_maxParticles, 0.05f, 0.0f, max_int_val, "%.2f");

	ImGui::PopItemWidth();


	std::string spawnTimer = "Spawn Timer: " + std::to_string(emitter->m_spawnTimer);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 1, 1));
	ImGui::Text(spawnTimer.c_str());
	ImGui::PopStyleColor();

	ImGui::Dummy(ImVec2(0, 4));

	ImGui::Dummy(ImVec2(0, 4));

	ImGui::Checkbox("##m_loopSpawning", &emitter->m_loopSpawning);
	ImGui::SameLine();
	ImGui::Text("Loop Spawning");

	if (emitter->m_loopSpawning)
	{
		ParticleTab();
		ImGui::Checkbox("##m_p_rangedSpawnRate", &emitter->m_p_rangedSpawnRate);
		ImGui::SameLine();
		ImGui::Text("Spawn Rate");

		if (emitter->m_p_rangedSpawnRate)
		{
			ParticleTab();
			ImGui::PushItemWidth(46.f);

			ImGui::DragFloat("##m_p_minSpawnRate", &emitter->m_p_minSpawnRate, 0.05f, 0.0f, max_float_val, "%.2f");
			ImGui::SameLine();
			ImGui::DragFloat("##m_p_maxSpawnRate", &emitter->m_p_maxSpawnRate, 0.05f, 0.0f, max_float_val, "%.2f");
			ImGui::PopItemWidth();
		}
		else
		{
			ImGui::Dummy(ImVec2(38, 0));
			ImGui::SameLine();
			ImGui::PushItemWidth(100.0f);

			ImGui::DragFloat("##m_spawnRate", &emitter->m_spawnRate, 0.05f, 0.0f, max_float_val, "%.2f");
			ImGui::PopItemWidth();
		}
	}

	//start spawn delay
	ImGui::Checkbox("##m_p_rangedSpawnDelay", &emitter->m_p_rangedSpawnDelay);
	ImGui::SameLine();
	ImGui::Text("Spawn Delay");

	if (emitter->m_p_rangedSpawnDelay)
	{
		ImGui::PushItemWidth(46.f);

		ImGui::DragFloat("##m_p_minSpawnDelay", &emitter->m_p_minSpawnDelay, 0.05f, 0.0f, max_float_val, "%.2f");
		ImGui::SameLine();
		ImGui::DragFloat("##m_p_maxSpawnDelay", &emitter->m_p_maxSpawnDelay, 0.05f, 0.0f, max_float_val, "%.2f");
		ImGui::PopItemWidth();
	}
	else
	{
		ImGui::PushItemWidth(100.0f);
		ImGui::DragFloat("##m_p_spawnDelay", &emitter->m_p_spawnDelay, 0.05f, 0.0f, max_float_val, "%.2f");
		ImGui::PopItemWidth();
	}
	ImGui::Dummy(ImVec2(0, 4));

	ImGui::Checkbox("##m_rangedSpawnAmount", &emitter->m_rangedSpawnAmount);
	ImGui::SameLine();
	ImGui::Text("Spawn Amount");

	if (emitter->m_rangedSpawnAmount)
	{

		ImGui::PushItemWidth(46.f);

		ImGui::DragInt("##m_minSpawnAmount", &emitter->m_minSpawnAmount, 0.05f, 0.0f, max_int_val, "%.2f");
		ImGui::SameLine();
		ImGui::DragInt("##m_maxSpawnAmount", &emitter->m_maxSpawnAmount, 0.05f, 0.0f, max_int_val, "%.2f");
		ImGui::PopItemWidth();
	}
	else
	{
		ImGui::Dummy(ImVec2(38, 0));
		ImGui::SameLine();
		ImGui::PushItemWidth(100.0f);

		ImGui::DragInt("##m_spawnAmount", &emitter->m_spawnAmount, 0.05f, 0.0f, max_int_val, "%.2f");
		ImGui::PopItemWidth();
	}
	
	ImGui::Dummy(ImVec2(0, 4));

	ImGui::Separator();
	ImGui::Text("Particle attributes");
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 4));

	ImGui::Checkbox("##m_billboardActive", &emitter->m_billboardActive);
	ImGui::SameLine();
	ImGui::Text("Use Billboarding");


	ImGui::Dummy(ImVec2(0, 4));

	if (emitter->m_billboardActive)
	{
		ParticleTab();
		ImGui::Text("Lock Axis");
		ImGui::SameLine();
		HelpMarker("Locks selected Axis from any other rotation more than the billboarding");
		ParticleTab();
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::Checkbox("##m_billboardLockAxisX", &emitter->m_billboardLockAxisX);
		ImGui::SameLine();
		ImGui::Text(" Y:");
		ImGui::SameLine();
		ImGui::Checkbox("##m_billboardLockAxisY", &emitter->m_billboardLockAxisY);
		ImGui::SameLine();
		ImGui::Text(" Z:");
		ImGui::SameLine();
		ImGui::Checkbox("##m_billboardLockAxisZ", &emitter->m_billboardLockAxisZ);

	}
	ImGui::Dummy(ImVec2(0, 8));

	//lifetime
	
	if (!emitter->m_permanentParticles)
	{
		ImGui::Checkbox("##m_p_rangedLifeTime", &emitter->m_p_rangedLifeTime);
		ImGui::SameLine();
	}
	ImGui::Text("Life Time");

	if (!emitter->m_permanentParticles)
	{
		if (emitter->m_p_rangedLifeTime)
		{
			ImGui::PushItemWidth(46.f);

			ImGui::DragFloat("##m_p_minLifeTime", &emitter->m_p_minLifeTime, 0.05f, 0.0f, max_float_val, "%.2f");
			ImGui::SameLine();
			ImGui::DragFloat("##m_p_maxLifeTime", &emitter->m_p_maxLifeTime, 0.05f, 0.0f, max_float_val, "%.2f");
			ImGui::PopItemWidth();
		}
		else
		{
			ImGui::PushItemWidth(100.0f);
			ImGui::DragFloat("##m_p_lifeTime", &emitter->m_p_lifeTime, 0.05f, 0.0f, max_float_val, "%.2f");
			ImGui::PopItemWidth();
		}
		ImGui::SameLine();
		ImGui::Checkbox("##m_permanentParticles", &emitter->m_permanentParticles);
		ImGui::SameLine();
		ImGui::Text("Permanent");
	}
	
	if (emitter->m_permanentParticles)
	{
		ImGui::Checkbox("##m_permanentParticles", &emitter->m_permanentParticles);
		ImGui::SameLine();
		ImGui::Text("Permanent");
	
		ImGui::Checkbox("##m_cycleLifeTime", &emitter->m_cycleLifeTime);
		ImGui::SameLine();
		ImGui::Text("Cycle LifeTime");
		ImGui::SameLine();
		HelpMarker("The Life Time keep counting, but it will restart once ended. This will affect variables dependent on the Life Time.");

		if (emitter->m_cycleLifeTime)
		{
			if (emitter->m_p_rangedLifeTime)
			{
				ImGui::PushItemWidth(46.f);

				ImGui::DragFloat("##m_p_minLifeTime", &emitter->m_p_minLifeTime, 0.05f, 0.0f, max_float_val, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##m_p_maxLifeTime", &emitter->m_p_maxLifeTime, 0.05f, 0.0f, max_float_val, "%.2f");
				ImGui::PopItemWidth();
			}
			else
			{
				ImGui::PushItemWidth(100.0f);
				ImGui::DragFloat("##m_p_lifeTime", &emitter->m_p_lifeTime, 0.05f, 0.0f, max_float_val, "%.2f");
				ImGui::PopItemWidth();
			}

		}
		ImGui::Dummy(ImVec2(0, 4));

	}

	


	

	ImGui::Dummy(ImVec2(0, 4));

	if (ImGui::TreeNode("Position & Translation"))
	{
		if (!emitter->m_p_followEmitterPositionSpawn)
		{
			ImGui::Checkbox("##m_p_followEmitterPosition", &emitter->m_p_followEmitterPosition);
			ImGui::SameLine();
			ImGui::Text("Follow Emitter Position");
		}

		if (!emitter->m_p_followEmitterPosition)
		{
			ImGui::Checkbox("##m_p_followEmitterPositionSpawn", &emitter->m_p_followEmitterPositionSpawn);
			ImGui::SameLine();
			ImGui::Text("Follow Emitter Position Only on Spawn");
		}


		if ((int)emitter->m_p_positionFollowsRotation > 1 || (int)emitter->m_p_positionFollowsRotation < 0)
			emitter->m_p_positionFollowsRotation = false;

		if ((int)emitter->m_stopSizeAtZeroX > 1 || (int)emitter->m_stopSizeAtZeroX < 0)
			emitter->m_stopSizeAtZeroX = false;

		if ((int)emitter->m_stopSizeAtZeroY > 1 || (int)emitter->m_stopSizeAtZeroY < 0)
			emitter->m_stopSizeAtZeroY = false;

		if ((int)emitter->m_stopSizeAtZeroZ > 1 || (int)emitter->m_stopSizeAtZeroZ < 0)
			emitter->m_stopSizeAtZeroZ = false;

		ImGui::Checkbox("##m_p_positionFollowsRotation", &emitter->m_p_positionFollowsRotation);
		ImGui::SameLine();
		ImGui::Text("Position Depends on Rotation");

		

		ImGui::Text("Use Volume:");
		ImGui::SameLine();
		const char* volumeItems[] = { "None", "Cube", "Sphere" };
		static const char* volumeCurrentItem = NULL;

		switch (emitter->m_spawnVolume)
		{
		case Wiwa::ParticleSpawnVolume::NONE:
		{
			volumeCurrentItem = "None";
		}
		break;
		case Wiwa::ParticleSpawnVolume::CUBE:
		{
			volumeCurrentItem = "Cube";

		}
		break;
		case Wiwa::ParticleSpawnVolume::SPHERE:
		{
			volumeCurrentItem = "Sphere";

		}
		break;
		default:
		{
			volumeCurrentItem = "None";

		}
		break;
		}

		

		ImGui::PushItemWidth(100);
		if (ImGui::BeginCombo("##combo", volumeCurrentItem)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < IM_ARRAYSIZE(volumeItems); n++)
			{
				bool is_selected = (volumeCurrentItem == volumeItems[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(volumeItems[n], is_selected))
				{
					volumeCurrentItem = volumeItems[n];

					switch (n)
					{
					case Wiwa::ParticleSpawnVolume::NONE:
					{
						emitter->m_spawnVolume = Wiwa::ParticleSpawnVolume::NONE;
					}
					break;
					case Wiwa::ParticleSpawnVolume::CUBE:
					{
						emitter->m_spawnVolume = Wiwa::ParticleSpawnVolume::CUBE;

					}
					break;
					case Wiwa::ParticleSpawnVolume::SPHERE:
					{
						emitter->m_spawnVolume = Wiwa::ParticleSpawnVolume::SPHERE;
					}
					break;

					default:
					{
						emitter->m_spawnVolume = Wiwa::ParticleSpawnVolume::NONE;
					}
					break;
					}
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		switch (emitter->m_spawnVolume)
		{
		case Wiwa::ParticleSpawnVolume::NONE:
		{
			
			ImGui::Text("Initial Position");
			ImGui::DragFloat3("##m_p_initialPosition", &(emitter->m_p_initialPosition)[0], 0.05f, 0.0f, 0.0f, "%.2f");
						
		}
		break;
		case Wiwa::ParticleSpawnVolume::CUBE:
		{
			ImGui::Text("Spawn Area:");

			ImGui::Text("Corner A");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_initialPositionBoxA", &(emitter->m_p_initialPositionBoxA)[0], 0.05f, 0.0f, 0.0f, "%.2f");
			ImGui::Text("Corner B");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_initialPositionBoxB", &(emitter->m_p_initialPositionBoxB)[0], 0.05f, 0.0f, 0.0f, "%.2f");
		}
		break;
		case Wiwa::ParticleSpawnVolume::SPHERE:
		{
			ImGui::Text("Center Position");
			ImGui::DragFloat3("##m_p_initialPositionSphCenter", &(emitter->m_p_initialPositionSphCenter)[0], 0.05f, 0.0f, 0.0f, "%.2f");

			ImGui::Text("Spawn Radius");
			ImGui::DragFloat("##m_p_initialPositionSphRadius", &emitter->m_p_initialPositionSphRadius, 0.05f, 0.0f, 0.0f, "%.2f");
		}
		break;
		default:
		{
			ImGui::Text("Initial Position");
			ImGui::DragFloat3("##m_p_initialPosition", &(emitter->m_p_initialPosition)[0], 0.05f, 0.0f, 0.0f, "%.2f");
		}
		break;
		}
		ImGui::Dummy(ImVec2(0, 4));

		ImGui::Checkbox("##m_p_positionTowardsPoint", &emitter->m_p_positionTowardsPoint);
		ImGui::SameLine();
		ImGui::Text("Move Towards Point");

		if (emitter->m_p_positionTowardsPoint)
		{
			ImGui::Text("Point ");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_positionTowardsPointPos", &(emitter->m_p_positionTowardsPointPos)[0], 0.05f, 0.0f, 0.0f, "%.2f");
		}

		ImGui::Dummy(ImVec2(0, 8));

		ImGui::Checkbox("##m_p_rangedVelocity", &emitter->m_p_rangedVelocity);
		ImGui::SameLine();
		ImGui::Text("Initial Velocity");

		if (emitter->m_p_rangedVelocity)
		{
			ImGui::Text("Min");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_minVelocity", &(emitter->m_p_minVelocity)[0], 0.05f, 0.0f, 0.0f, "%.2f");
			ImGui::Text("Max");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_maxVelocity", &(emitter->m_p_maxVelocity)[0], 0.05f, 0.0f, 0.0f, "%.2f");
		}
		else
		{
			ImGui::DragFloat3("##m_p_initialVelocity", &(emitter->m_p_initialVelocity)[0], 0.05f, 0.0f, 0.0f, "%.2f");
		}

		ImGui::Dummy(ImVec2(0, 8));

		ImGui::Checkbox("##m_p_useGravity", &emitter->m_p_useGravity);
		ImGui::SameLine();
		ImGui::Text("Use Gravity");

		if (emitter->m_p_useGravity)
		{
			ImGui::PushItemWidth(100.0f);
			ImGui::DragFloat("##m_p_lifeTime", &emitter->m_p_gravity, 0.05f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
		}

		ImGui::TreePop();
		ImGui::Dummy(ImVec2(0, 8));
	}

	if (ImGui::TreeNode("Rotation & Angular Velocity"))
	{
		if ((int)emitter->m_p_followEmitterRotation > 1 || (int)emitter->m_p_followEmitterRotation < 0)
			emitter->m_p_followEmitterRotation = false;

		if (!emitter->m_p_followEmitterRotationSpawn)
		{
			ImGui::Checkbox("##m_p_followEmitterRotation", &emitter->m_p_followEmitterRotation);
			ImGui::SameLine();
			ImGui::Text("Follow Emitter Rotation");
		}

		if (!emitter->m_p_followEmitterRotation)
		{
			ImGui::Checkbox("##m_p_followEmitterRotationSpawn", &emitter->m_p_followEmitterRotationSpawn);
			ImGui::SameLine();
			ImGui::Text("Follow Emitter Rotation Only on Spawn");
		}
		
		ImGui::Dummy(ImVec2(0, 8));

		ImGui::Checkbox("##m_p_initialRotation", &emitter->m_p_rangedInitialRotation);
		ImGui::SameLine();
		ImGui::Text("Initial Rotation");

		if (emitter->m_p_rangedInitialRotation)
		{
			ImGui::Text("Min");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_minInitialRotation", &(emitter->m_p_minInitialRotation)[0], 0.05f, 0.0f, 0.0f, "%.2f");
			ImGui::Text("Max");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_maxInitialRotation", &(emitter->m_p_maxInitialRotation)[0], 0.05f, 0.0f, 0.0f, "%.2f");
		}
		else
		{
			ImGui::DragFloat3("##m_p_initialRotation", &(emitter->m_p_initialRotation)[0], 0.05f, 0.0f, 0.0f, "%.2f");
		}

		ImGui::Dummy(ImVec2(0, 8));

		ImGui::Checkbox("##m_p_rangedAngularVelocity", &emitter->m_p_rangedAngularVelocity);
		ImGui::SameLine();
		ImGui::Text("Initial Angular Velocity");

		ImGui::Checkbox("##m_p_rotationOverTime", &emitter->m_p_rotationOverTime);
		ImGui::SameLine();
		ImGui::Text("Use Rotation over time");

		if (emitter->m_p_rotationOverTime)
		{
			ImGui::Text("From");
			ImGui::SameLine();

			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("##m_p_rotationOverTimePerStart", &emitter->m_p_rotationOverTimePerStart, 0.05f, 0.0f, 100.f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::SameLine();
			ImGui::Text("%% to");
			ImGui::SameLine();

			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("##m_p_rotationOverTimePerEnd", &emitter->m_p_rotationOverTimePerEnd, 0.05f, 0.0f, 100.f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text("%%");


			ImGui::Text("Start");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_rotationOverTimeStart", &(emitter->m_p_rotationOverTimeStart)[0], 0.05f, 0.0f, 0.0f, "%.2f");
			ImGui::Text("End  ");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_rotationOverTimeEnd", &(emitter->m_p_rotationOverTimeEnd)[0], 0.05f, 0.0f, 0.0f, "%.2f");
		}
		else
		{
			if (emitter->m_p_rangedAngularVelocity)
			{
				ImGui::Text("Min");
				ImGui::SameLine();
				ImGui::DragFloat3("##m_p_minInitialAngularVelocity", &(emitter->m_p_minInitialAngularVelocity)[0], 0.05f, 0.0f, 0.0f, "%.2f");
				ImGui::Text("Max");
				ImGui::SameLine();
				ImGui::DragFloat3("##m_p_maxInitialAngularVelocity", &(emitter->m_p_maxInitialAngularVelocity)[0], 0.05f, 0.0f, 0.0f, "%.2f");
			}
			else
			{
				ImGui::DragFloat3("##m_p_initialAngularVelocity", &(emitter->m_p_initialAngularVelocity)[0], 0.05f, 0.0f, 0.0f, "%.2f");
			}
		}

		ImGui::TreePop();
		ImGui::Dummy(ImVec2(0, 8));
	}

	if (ImGui::TreeNode("Scaling & Growth"))
	{
		ImGui::Checkbox("##m_p_rangedInitialScale", &emitter->m_p_rangedInitialScale);
		ImGui::SameLine();
		ImGui::Text("Initial Scale");

		if (emitter->m_p_uniformStartSize)
		{
			if (emitter->m_p_rangedInitialScale)
			{
				ParticleTab();
				ImGui::PushItemWidth(100.f);

				ImGui::Text("Min");
				ImGui::SameLine();
				ImGui::DragFloat("##m_p_minUniformStartSizeVal", &emitter->m_p_minUniformStartSizeVal, 0.05f, 0.0f, 0.0f, "%.2f");

				ParticleTab();
				ImGui::Text("Max");
				ImGui::SameLine();
				ImGui::DragFloat("##m_p_maxUniformStartSizeVal", &emitter->m_p_maxUniformStartSizeVal, 0.05f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();
			}
			else
			{
				ImGui::Dummy(ImVec2(38, 0));
				ImGui::SameLine();
				ImGui::PushItemWidth(100.0f);

				ImGui::DragFloat("##m_p_uniformStartSizeVal", &emitter->m_p_uniformStartSizeVal, 0.05f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();
			}
		}
		else
		{
			if (emitter->m_p_rangedInitialScale)
			{
				ImGui::Text("Min");
				ImGui::SameLine();
				ImGui::DragFloat3("##m_p_minInitialScale", &(emitter->m_p_minInitialScale)[0], 0.05f, 0.0f, 0.0f, "%.2f");
				ImGui::Text("Max");
				ImGui::SameLine();
				ImGui::DragFloat3("##m_p_maxInitialScale", &(emitter->m_p_maxInitialScale)[0], 0.05f, 0.0f, 0.0f, "%.2f");
			}
			else
			{
				ImGui::DragFloat3("##m_p_initialScale", &(emitter->m_p_initialScale)[0], 0.05f, 0.0f, 0.0f, "%.2f");
			}
		}

		ImGui::Dummy(ImVec2(0, 4));

		ParticleTab();
		ImGui::Checkbox("##m_p_uniformStartSize", &emitter->m_p_uniformStartSize);
		ImGui::SameLine();
		ImGui::Text("Uniform Start Scale");

		ImGui::Dummy(ImVec2(0, 8));

		if ((int)emitter->m_p_growUniformly > 1 || (int)emitter->m_p_growUniformly < 0)
			emitter->m_p_growUniformly = false;

		ImGui::Checkbox("##m_p_scaleOverTime", &emitter->m_p_scaleOverTime);
		ImGui::SameLine();
		ImGui::Text("Use Growth over life time");
		

		if (emitter->m_p_scaleOverTime)
		{
			ImGui::Text("From");
			ImGui::SameLine();

			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("##m_p_scaleOverTimePerStart", &emitter->m_p_scaleOverTimePerStart, 0.05f, 0.0f, 100.f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::SameLine();
			ImGui::Text("%% to");
			ImGui::SameLine();

			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("##m_p_scaleOverTimePerEnd", &emitter->m_p_scaleOverTimePerEnd, 0.05f, 0.0f, 100.f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text("%%");


			ImGui::Text("Start");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_scaleOverTimeStart", &(emitter->m_p_scaleOverTimeStart)[0], 0.05f, 0.0f, 0.0f, "%.2f");
			ImGui::Text("End  ");
			ImGui::SameLine();
			ImGui::DragFloat3("##m_p_scaleOverTimeEnd", &(emitter->m_p_scaleOverTimeEnd)[0], 0.05f, 0.0f, 0.0f, "%.2f");

		}
		else
		{
			ImGui::Checkbox("##m_p_rangedGrowthVelocity", &emitter->m_p_rangedGrowthVelocity);
			ImGui::SameLine();
			ImGui::Text("Initial Growth Velocity");

			ImGui::Dummy(ImVec2(0, 4));

			

			if (emitter->m_p_growUniformly)
			{
				if (emitter->m_p_rangedGrowthVelocity)
				{
					ParticleTab();
					ImGui::PushItemWidth(100.f);

					ImGui::Text("Min");
					ImGui::SameLine();
					ImGui::DragFloat("##m_p_minUniformGrowthVal", &emitter->m_p_minUniformGrowthVal, 0.05f, 0.0f, 0.0f, "%.2f");
					
					ParticleTab();
					ImGui::Text("Max");
					ImGui::SameLine();
					ImGui::DragFloat("##m_p_maxUniformGrowthVal", &emitter->m_p_maxUniformGrowthVal, 0.05f, 0.0f, 0.0f, "%.2f");
					ImGui::PopItemWidth();
				}
				else
				{
					ImGui::Dummy(ImVec2(38, 0));
					ImGui::SameLine();
					ImGui::PushItemWidth(100.0f);

					ImGui::DragFloat("##m_p_uniformGrowthVal", &emitter->m_p_uniformGrowthVal, 0.05f, 0.0f, 0.0f, "%.2f");
					ImGui::PopItemWidth();
				}
			}
			else
			{
				if (emitter->m_p_rangedGrowthVelocity)
				{
					ImGui::Text("Min");
					ImGui::SameLine();
					ImGui::DragFloat3("##m_p_minInitialGrowthVelocity", &(emitter->m_p_minInitialGrowthVelocity)[0], 0.05f, 0.0f, 0.0f, "%.2f");
					ImGui::Text("Max");
					ImGui::SameLine();
					ImGui::DragFloat3("##m_p_maxInitialGrowthVelocity", &(emitter->m_p_maxInitialGrowthVelocity)[0], 0.05f, 0.0f, 0.0f, "%.2f");
				}
				else
				{
					ImGui::DragFloat3("##m_p_initialGrowthVelocity", &(emitter->m_p_initialGrowthVelocity)[0], 0.05f, 0.0f, 0.0f, "%.2f");
				}
			}

			ImGui::Dummy(ImVec2(0, 4));

			ParticleTab();
			ImGui::Checkbox("##m_p_growUniformly", &emitter->m_p_growUniformly);
			ImGui::SameLine();
			ImGui::Text("Scale Uniformly");

			ParticleTab();
			ImGui::Checkbox("##m_stopSizeAtZero", &emitter->m_stopSizeAtZero);
			ImGui::SameLine();
			ImGui::Text("Stop Growth At Zero");


			if (emitter->m_stopSizeAtZero)
			{
				ParticleTab();
				ImGui::Text("X:");
				ImGui::SameLine();
				ImGui::Checkbox("##m_stopSizeAtZeroX", &emitter->m_stopSizeAtZeroX);
				ImGui::SameLine();
				ImGui::Text(" Y:");
				ImGui::SameLine();
				ImGui::Checkbox("##m_stopSizeAtZeroY", &emitter->m_stopSizeAtZeroY);
				ImGui::SameLine();
				ImGui::Text(" Z:");
				ImGui::SameLine();
				ImGui::Checkbox("##m_stopSizeAtZeroZ", &emitter->m_stopSizeAtZeroZ);

			}
			ImGui::Dummy(ImVec2(0, 8));
			
		}

		
		


		ImGui::TreePop();
		ImGui::Dummy(ImVec2(0, 8));
	}

	if (ImGui::TreeNode("Color & Blending"))
	{
		if ((int)emitter->m_useAdditiveBlending > 1 || (int)emitter->m_useAdditiveBlending < 0)
			emitter->m_useAdditiveBlending = false;

		if ((int)emitter->m_useMultiplyBlending > 1 || (int)emitter->m_useMultiplyBlending < 0)
			emitter->m_useMultiplyBlending = false;


		ImGui::Text("Blending Mode:");

		ImGui::Dummy(ImVec2(0, 4));

		if (!emitter->m_useMultiplyBlending)
		{
			ParticleTab();
			ImGui::Checkbox("##m_useAdditiveBlending", &emitter->m_useAdditiveBlending);
			ImGui::SameLine();
			ImGui::Text("Additive");

			ImGui::Dummy(ImVec2(0, 4));
		}
		else
			ImGui::Dummy(ImVec2(0, 8));


		if (!emitter->m_useAdditiveBlending)
		{
			ParticleTab();
			ImGui::Checkbox("##m_useMultiplyBlending", &emitter->m_useMultiplyBlending);
			ImGui::SameLine();
			ImGui::Text("Multiply");

			ImGui::Dummy(ImVec2(0, 4));
		}
		else
			ImGui::Dummy(ImVec2(0, 8));

		if ((int)emitter->m_deactivateFaceCulling > 1 || (int)emitter->m_deactivateFaceCulling < 0)
			emitter->m_deactivateFaceCulling = false;

		ImGui::Checkbox("##m_deactivateFaceCulling", &emitter->m_deactivateFaceCulling);
		ImGui::SameLine();
		ImGui::Text("Deactivate Face Culling");


		
		ImGui::Dummy(ImVec2(0, 12));

		ImGui::Checkbox("##m_useNewColorNodes", &emitter->m_useNewColorNodes);
		ImGui::SameLine();
		ImGui::Text("Use New Color System");

		

		if (emitter->m_useNewColorNodes)
		{
			ImGui::Text("Max. colors available: 20");
			ImGui::Dummy(ImVec2(0, 8));
			if (emitter->m_newColorsUsed < 1)
			{
				emitter->m_newColorsUsed = 1;
			}
			if (emitter->m_newColorsUsed >= 20)
			{
				emitter->m_newColorsUsed = 20;
			}

			ImGui::Text("Colors:");

			//todo: add delete color and duplicate color
			ImGui::Dummy(ImVec2(0, 8));
			ImGui::Text("Colors over lifetime:");
			ImGui::SameLine();
			ImGui::PushItemWidth(100.0f);
			ImGui::DragInt("##m_colorNodes", &emitter->m_newColorsUsed, 0.1f, 1.0f, 20, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button("+") && emitter->m_newColorsUsed < 20) emitter->m_newColorsUsed++;
			ImGui::SameLine();
			if (ImGui::Button("-") && emitter->m_newColorsUsed > 1) emitter->m_newColorsUsed--;
			ImGui::Dummy(ImVec2(0, 4));

			if (emitter->m_permanentParticles && emitter->m_cycleLifeTime)
			{
				ImGui::Checkbox("##m_cycleColors", &emitter->m_cycleColors);
				ImGui::SameLine();
				ImGui::Text("Cycle Colors");
				ImGui::SameLine();
				HelpMarker("Available due to 'Cycle Life Time'. Colors will cycle along with the Life Time");
				ImGui::Dummy(ImVec2(0, 4));

			}

			if (ImGui::Button("Sort Colors by Percentage"))
			{
				for (int i = 0; i < emitter->m_newColorsUsed; i++) {
					int minIndex = i;
					for (int j = i + 1; j < emitter->m_newColorsUsed; j++) {
						if (emitter->m_newColorsNodes[j].m_percentage < emitter->m_newColorsNodes[minIndex].m_percentage) {
							minIndex = j;
						}
					}
					auto temp = emitter->m_newColorsNodes[i];
					emitter->m_newColorsNodes[i] = emitter->m_newColorsNodes[minIndex];
					emitter->m_newColorsNodes[minIndex] = temp;
				}
			}

			if (ImGui::Button("Reset unused Colors"))
			{
				for (size_t i = emitter->m_newColorsUsed; i < 20; i++)
				{
					emitter->m_newColorsNodes[i].m_percentage = 0;
					emitter->m_newColorsNodes[i].color = glm::vec4(0, 0, 0, 1);
				}
			}

			ImGui::Dummy(ImVec2(0, 4));


			for (size_t i = 0; i < emitter->m_newColorsUsed; i++)
			{
				std::string colorCodePercentage = "##colorCodePer_" + std::to_string(i);
				std::string colorCode = "##colorCode_" + std::to_string(i);

				ImGui::Text("At");
				ImGui::SameLine();
				ImGui::PushItemWidth(75);
				ImGui::DragFloat(colorCodePercentage.c_str(), &emitter->m_newColorsNodes[i].m_percentage, 0.1f, 0.0f, 100.0f, "%.2f");
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::Text("%% use");
				ImGui::SameLine();
				//ImGui::ColorEdit4(colorCode.c_str(), &emitter->m_newColorsNodes[i].color.x);


				ImVec4 imCol = {
					emitter->m_newColorsNodes[i].color.x,
					emitter->m_newColorsNodes[i].color.y,
					emitter->m_newColorsNodes[i].color.z,
					emitter->m_newColorsNodes[i].color.w };

				ImVec4 temp = ImVec4(0.2, 0.8, 0.4, 1);

				ImGui::PushStyleColor(ImGuiCol_Button, imCol);

				float bordergrey = 0.75f;
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(bordergrey, bordergrey, bordergrey, 1));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.25));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2);

				std::string colorCodePickerPopup = "##colorCodePickerPopup_" + std::to_string(i);

				if (ImGui::Button(colorCode.c_str(), ImVec2(75, 20)))
				{
					ImGui::OpenPopup(colorCodePickerPopup.c_str());
				}

				// Display a color picker inside a popup
				if (ImGui::BeginPopup(colorCodePickerPopup.c_str()))
				{
					std::string colorCodePicker = "##colorCodePicker_" + std::to_string(i);
					ImGui::ColorPicker4(colorCodePicker.c_str(), (float*)&emitter->m_newColorsNodes[i].color.x);
					ImGui::EndPopup();
				}
				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar();
				ImGui::SameLine();
				std::string colorCodeAlpha = "##colorCodeAlpha_" + std::to_string(i);


				ImGui::Text("A:", i);
				ImGui::SameLine();
				ImGui::PushItemWidth(50);
				ImGui::DragFloat(colorCodeAlpha.c_str(), &emitter->m_newColorsNodes[i].color.w, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::PopItemWidth();

				ImGui::SameLine();
				std::string colorCodeDuplicate = "++##colorCodeDuplicate_" + std::to_string(i);
				if (ImGui::Button(colorCodeDuplicate.c_str()) && emitter->m_newColorsUsed < 20)
				{
					emitter->m_newColorsUsed++;
					for (size_t j = emitter->m_newColorsUsed; j > i; j--)
					{
						emitter->m_newColorsNodes[j + 1].m_percentage = emitter->m_newColorsNodes[j].m_percentage;
						emitter->m_newColorsNodes[j + 1].color = emitter->m_newColorsNodes[j].color;
					}
					emitter->m_newColorsNodes[i + 1].m_percentage = emitter->m_newColorsNodes[i].m_percentage;
					emitter->m_newColorsNodes[i + 1].color = emitter->m_newColorsNodes[i].color;
				}

				ImGui::SameLine();

				std::string colorCodeDelete = "-##colorCodeDelete_" + std::to_string(i);
				if (ImGui::Button(colorCodeDelete.c_str()) && emitter->m_newColorsUsed > 1)
				{
					for (size_t j = i + 1; j < emitter->m_newColorsUsed; j++)
					{
						emitter->m_newColorsNodes[j - 1].m_percentage = emitter->m_newColorsNodes[j].m_percentage;
						emitter->m_newColorsNodes[j - 1].color = emitter->m_newColorsNodes[j].color;
					}
					emitter->m_newColorsNodes[emitter->m_newColorsUsed - 1].m_percentage = 0;
					emitter->m_newColorsNodes[emitter->m_newColorsUsed - 1].color = glm::vec4(0, 0, 0, 1);
					emitter->m_newColorsUsed--;
				}
			}
		}
		else
		{
			ImGui::Text("Max. colors available: 4");
			ImGui::Dummy(ImVec2(0, 8));

			if (emitter->m_colorsUsed < 1)
			{
				emitter->m_colorsUsed = 1;
			}
			if (emitter->m_colorsUsed >= 4)
			{
				emitter->m_colorsUsed = 4;
			}

			ImGui::Text("Colors:");

			//todo: add delete color and duplicate color
			ImGui::Dummy(ImVec2(0, 8));
			ImGui::Text("Colors over lifetime:");
			ImGui::SameLine();
			ImGui::PushItemWidth(100.0f);
			ImGui::DragInt("##m_colorNodes", &emitter->m_colorsUsed, 0.1f, 1.0f, 4, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button("+") && emitter->m_colorsUsed < 4) emitter->m_colorsUsed++;
			ImGui::SameLine();
			if (ImGui::Button("-") && emitter->m_colorsUsed > 1) emitter->m_colorsUsed--;
			ImGui::Dummy(ImVec2(0, 4));

			if (emitter->m_permanentParticles && emitter->m_cycleLifeTime)
			{
				ImGui::Checkbox("##m_cycleColors", &emitter->m_cycleColors);
				ImGui::SameLine();
				ImGui::Text("Cycle Colors");
				ImGui::SameLine();
				HelpMarker("Available due to 'Cycle Life Time'. Colors will cycle along with the Life Time");
				ImGui::Dummy(ImVec2(0, 4));

			}

			if (ImGui::Button("Sort Colors by Percentage"))
			{
				for (int i = 0; i < emitter->m_colorsUsed; i++) {
					int minIndex = i;
					for (int j = i + 1; j < emitter->m_colorsUsed; j++) {
						if (emitter->m_p_colorsOverLifetime[j].m_percentage < emitter->m_p_colorsOverLifetime[minIndex].m_percentage) {
							minIndex = j;
						}
					}
					auto temp = emitter->m_p_colorsOverLifetime[i];
					emitter->m_p_colorsOverLifetime[i] = emitter->m_p_colorsOverLifetime[minIndex];
					emitter->m_p_colorsOverLifetime[minIndex] = temp;
				}
			}

			if (ImGui::Button("Reset unused Colors"))
			{
				for (size_t i = emitter->m_colorsUsed; i < 4; i++)
				{
					emitter->m_p_colorsOverLifetime[i].m_percentage = 0;
					emitter->m_p_colorsOverLifetime[i].color = glm::vec4(0, 0, 0, 1);
				}
			}

			ImGui::Dummy(ImVec2(0, 4));


			for (size_t i = 0; i < emitter->m_colorsUsed; i++)
			{
				std::string colorCodePercentage = "##colorCodePer_" + std::to_string(i);
				std::string colorCode = "##colorCode_" + std::to_string(i);

				ImGui::Text("At");
				ImGui::SameLine();
				ImGui::PushItemWidth(75);
				ImGui::DragFloat(colorCodePercentage.c_str(), &emitter->m_p_colorsOverLifetime[i].m_percentage, 0.1f, 0.0f, 100.0f, "%.2f");
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::Text("%% use");
				ImGui::SameLine();
				//ImGui::ColorEdit4(colorCode.c_str(), &emitter->m_p_colorsOverLifetime[i].color.x);


				ImVec4 imCol = {
					emitter->m_p_colorsOverLifetime[i].color.x,
					emitter->m_p_colorsOverLifetime[i].color.y,
					emitter->m_p_colorsOverLifetime[i].color.z,
					emitter->m_p_colorsOverLifetime[i].color.w };

				ImVec4 temp = ImVec4(0.2, 0.8, 0.4, 1);

				ImGui::PushStyleColor(ImGuiCol_Button, imCol);

				float bordergrey = 0.75f;
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(bordergrey, bordergrey, bordergrey, 1));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.25));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2);

				std::string colorCodePickerPopup = "##colorCodePickerPopup_" + std::to_string(i);

				if (ImGui::Button(colorCode.c_str(), ImVec2(75, 20)))
				{
					ImGui::OpenPopup(colorCodePickerPopup.c_str());
				}

				// Display a color picker inside a popup
				if (ImGui::BeginPopup(colorCodePickerPopup.c_str()))
				{
					std::string colorCodePicker = "##colorCodePicker_" + std::to_string(i);
					ImGui::ColorPicker4(colorCodePicker.c_str(), (float*)&emitter->m_p_colorsOverLifetime[i].color.x);
					ImGui::EndPopup();
				}
				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar();
				ImGui::SameLine();
				std::string colorCodeAlpha = "##colorCodeAlpha_" + std::to_string(i);


				ImGui::Text("A:", i);
				ImGui::SameLine();
				ImGui::PushItemWidth(50);
				ImGui::DragFloat(colorCodeAlpha.c_str(), &emitter->m_p_colorsOverLifetime[i].color.w, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::PopItemWidth();

				ImGui::SameLine();
				std::string colorCodeDuplicate = "++##colorCodeDuplicate_" + std::to_string(i);
				if (ImGui::Button(colorCodeDuplicate.c_str()) && emitter->m_colorsUsed < 4)
				{
					emitter->m_colorsUsed++;
					for (size_t j = emitter->m_colorsUsed; j > i; j--)
					{
						emitter->m_p_colorsOverLifetime[j + 1].m_percentage = emitter->m_p_colorsOverLifetime[j].m_percentage;
						emitter->m_p_colorsOverLifetime[j + 1].color = emitter->m_p_colorsOverLifetime[j].color;
					}
					emitter->m_p_colorsOverLifetime[i + 1].m_percentage = emitter->m_p_colorsOverLifetime[i].m_percentage;
					emitter->m_p_colorsOverLifetime[i + 1].color = emitter->m_p_colorsOverLifetime[i].color;
				}

				ImGui::SameLine();

				std::string colorCodeDelete = "-##colorCodeDelete_" + std::to_string(i);
				if (ImGui::Button(colorCodeDelete.c_str()) && emitter->m_colorsUsed > 1)
				{
					for (size_t j = i + 1; j < emitter->m_colorsUsed; j++)
					{
						emitter->m_p_colorsOverLifetime[j - 1].m_percentage = emitter->m_p_colorsOverLifetime[j].m_percentage;
						emitter->m_p_colorsOverLifetime[j - 1].color = emitter->m_p_colorsOverLifetime[j].color;
					}
					emitter->m_p_colorsOverLifetime[emitter->m_colorsUsed - 1].m_percentage = 0;
					emitter->m_p_colorsOverLifetime[emitter->m_colorsUsed - 1].color = glm::vec4(0, 0, 0, 1);
					emitter->m_colorsUsed--;
				}
			}
		}

		
		ImGui::TreePop();

		

	}
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 8));
}

void InspectorPanel::DrawItemComponent(byte* data)
{
	Wiwa::Item* item = (Wiwa::Item*)data;
	ImGui::InputText("Name",item->Name, 128);
	ImGui::InputScalar("ItemType", ImGuiDataType_U8,&item->item_type);

}

void InspectorPanel::DrawNavMeshComponent(byte* data)
{
	Wiwa::NavMesh* navMesh = (Wiwa::NavMesh*)data;

	AssetContainer(navMesh->filePath);
}

void InspectorPanel::DrawOzzAnimatorCmp(byte* data)
{
	Wiwa::OzzAnimatorCmp* a_cmp = (Wiwa::OzzAnimatorCmp*)data;

	ImGui::PushID("a_cmp");

	AssetContainer(a_cmp->animator_path);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS.c_str();
			if (p.extension() == ".wiozzanimator")
			{
				std::string libpath = Wiwa::Resources::_assetToLibPath(p.string());
				strcpy_s(a_cmp->animator_path, libpath.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::PopID();
}
void InspectorPanel::DrawPlayerSpawnerComponent(byte* data)
{
	Wiwa::PlayerSpawnerData* spawn = (Wiwa::PlayerSpawnerData*)data;
	DrawVec3Control("Position", &spawn->Position, 0.f);
}

InspectorPanel::InspectorPanel(EditorLayer *instance)
	: Panel("Inspector", ICON_FK_INFO_CIRCLE, instance)
{
}

InspectorPanel::~InspectorPanel()
{
}

void InspectorPanel::Draw()
{
	Wiwa::EntityManager &em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();

	ImGui::Begin(iconName.c_str(), &active);

	if (s_EntitySet && m_CurrentID >= 0)
	{
		ImGui::InputText("Name", &em.GetEntityString(m_CurrentID));
		ImGui::SameLine();
		ImGui::Text("(%i)", m_CurrentID);
		

		DrawCollisionTags();
		ImGui::SameLine();
		bool active = em.IsActive(m_CurrentID);
		if (ImGui::Checkbox("Active", &active))
			em.SetActive(m_CurrentID, active);

		if (ImGui::Button("Delete##entity"))
		{
			s_EntitySet = false;
			em.DestroyEntity(m_CurrentID);
		}

		if (ImGui::CollapsingHeader("Components"))
			DrawComponents(em);

		if (ImGui::CollapsingHeader("Systems"))
			DrawSystems(em);

	}
	else
	{
		TextCentered("Select an entity to inspect");
	}
	ImGui::End();
}

void InspectorPanel::DrawComponents(Wiwa::EntityManager &em)
{
	std::map<ComponentId, size_t> &map = em.GetEntityComponents(m_CurrentID);
	bool removed = false;
	size_t idToRemove;

	for (std::map<ComponentId, size_t>::iterator comp = map.begin(); comp != map.end(); comp++)
	{
		if (!DrawComponent(comp->first))
		{
			idToRemove = comp->first;
			removed = true;
		}
	}

	if (removed)
	{
		em.RemoveComponentById(m_CurrentID, idToRemove);
	}

	ImGui::Separator();

	if (ButtonCenteredOnLine("Add component"))
		ImGui::OpenPopup("Components");

	size_t c_count = Wiwa::Application::Get().GetComponentTypeCount();

	if (ImGui::BeginPopup("Components"))
	{
		static ImGuiTextFilter filter;
		ImGui::Text("Search:");
		filter.Draw("##searchbar", 340.f);
		ImGui::BeginChild("listbox child", ImVec2(300, 200));
		for (size_t i = 0; i < c_count; i++)
		{
			const Type *type = Wiwa::Application::Get().GetComponentType(i);
			const char *paintkit = type->name.c_str();
			if (filter.PassFilter(paintkit))
			{
				std::string label = paintkit;

				label += "##" + std::to_string(i);
				if (ImGui::MenuItem(label.c_str()))
				{
					em.AddComponent(m_CurrentID, type);
					ImGui::CloseCurrentPopup();
				}
			}
		}
		ImGui::EndChild();
		ImGui::EndPopup();
	}
}

void InspectorPanel::DrawSystems(Wiwa::EntityManager &em)
{
	std::vector<SystemHash> &systems = em.GetEntitySystemHashes(m_CurrentID);

	for (size_t i = 0; i < systems.size(); i++)
	{
		ImGui::PushID(i);
		const Type *system = Wiwa::Application::Get().GetSystemTypeH(systems[i]);
		ImGui::Text(system->name.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			em.RemoveSystem(m_CurrentID, system->hash);
		}
		ImGui::PopID();
	}

	if (ButtonCenteredOnLine("Add System"))
		ImGui::OpenPopup("System");

	size_t c_count = Wiwa::Application::Get().GetSystemTypeCount();

	if (ImGui::BeginPopup("System"))
	{
		static ImGuiTextFilter filter;
		ImGui::Text("Search:");
		filter.Draw("##searchbar", 340.f);
		ImGui::BeginChild("listbox child", ImVec2(300, 200));
		for (size_t i = 0; i < c_count; i++)
		{
			const Type *type = Wiwa::Application::Get().GetSystemType(i);
			const char *paintkit = type->name.c_str();
			if (filter.PassFilter(paintkit))
			{
				std::string label = paintkit;

				label += "##" + std::to_string(i);
				if (ImGui::MenuItem(label.c_str()))
				{
					em.ApplySystem(m_CurrentID, type);
					ImGui::CloseCurrentPopup();
				}
			}
		}
		ImGui::EndChild();
		ImGui::EndPopup();
	}
}

void InspectorPanel::Update()
{
}

void InspectorPanel::OnEvent(Wiwa::Event &e)
{
	Wiwa::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<EntityChangeEvent>( {&InspectorPanel::OnEntityChangeEvent, this});
	dispatcher.Dispatch<Wiwa::SceneChangeEvent>( {&InspectorPanel::OnSceneChangeEvent, this});
}

bool InspectorPanel::OnEntityChangeEvent(EntityChangeEvent &e)
{
	m_CurrentID = e.GetResourceId();
	s_EntitySet = true;
	return false;
}

bool InspectorPanel::OnSceneChangeEvent(Wiwa::SceneChangeEvent &e)
{
	m_CurrentID = -1;
	s_EntitySet = false;
	return false;
}
