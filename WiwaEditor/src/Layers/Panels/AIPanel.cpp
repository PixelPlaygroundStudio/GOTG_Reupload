#include "AIPanel.h"

#include "imgui.h"

#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/scene/SceneManager.h>
#include "../../Utils/EditorUtils.h"
#include "Wiwa/ecs/components/Mesh.h"
#include "Wiwa/AI/AI_RecastManager.h"
#include "Wiwa/ecs/components/ai/NavMesh.h"

AIPanel::AIPanel(EditorLayer* instance)
	: Panel("AI Panel", ICON_FK_MAGIC, instance)
{
	m_Id = -1;
	m_DebugDraw = true;
}

AIPanel::~AIPanel()
{
	//if (m_RecastMesh)
	//	delete m_RecastMesh;

	//if (m_Geom)
	//	delete m_Geom;
}

void AIPanel::Draw()
{
	//Wiwa::Model* mod = Wiwa::Resources::GetResourceById<Wiwa::Model>(mesh->meshId);
	//mod = mod->getModelAt(mesh->modelIndex);
	ImGui::Begin(iconName.c_str(), &active);
	HandleDragAndDrop();

	ImGui::Text("Need obj as input geometry");
	ImGui::SameLine();
	if (ImGui::Button("Create OBJ file"))
		Wiwa::RecastManager::CreateObj();

	ImGui::Text("Need recast to build, save or load");
	ImGui::SameLine();
	if (ImGui::Button("Setup Recast"))
		Wiwa::RecastManager::CreateRecast();

	HandleSettings();

	ImGui::Text("Build the navmesh through recast");
	ImGui::SameLine();
	if (ImGui::Button("Build Map"))
		Wiwa::RecastManager::Build();

	ImGui::Text("Save navmesh");
	ImGui::SameLine();
	if (ImGui::Button("Save Map"))
		HandleSave();

	ImGui::Text("Load navmesh");
	ImGui::SameLine();
	if (ImGui::Button("Load Map"))
		HandleLoad();

	if (ImGui::Button("Clean up"))
		HandleCleanUp();

	if (ImGui::Button("Deffault settings"))
		ResetSettings();

	if (ImGui::CollapsingHeader("Debug draw (Open to debug draw)", &m_DebugDraw, ImGuiTreeNodeFlags_DefaultOpen))
	{
		HandleRender();
		HandleDebugSettings();
		if (Wiwa::RecastManager::m_RecastMesh)
		{
			Wiwa::RecastManager::m_RecastMesh->handleTools();
		}
	}

	ImGui::End();
}

void AIPanel::HandleSettings()
{
	if (Wiwa::RecastManager::m_RecastMesh)
		Wiwa::RecastManager::m_RecastMesh->handleCommonSettings();
}

void AIPanel::ResetSettings()
{
	if (Wiwa::RecastManager::m_RecastMesh)
		Wiwa::RecastManager::m_RecastMesh->resetCommonSettings();
}

void AIPanel::HandleDragAndDrop()
{
	Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
	Wiwa::Mesh* mesh = nullptr;

	ImGui::Text("Drag and drop from hierarchy");

	if (m_Id != -1)
	{
		mesh = (Wiwa::Mesh*)em.GetComponentByIterator(em.GetComponentIterator<Wiwa::Mesh>(m_Id));
	}

	Wiwa::Model* mod = nullptr;
	if (mesh == nullptr){
		AssetContainer("Drag an entity from the hierarchy");
	}
	else
	{
		mod = Wiwa::Resources::GetResourceById<Wiwa::Model>(mesh->meshId);
		if (mod != NULL){
			//mod = mod->getModelAt(m_Mesh->modelIndex);
			AssetContainer(std::filesystem::path(mod->getModelPath()).stem().string().c_str());
		}
	}

	if (ImGui::BeginDragDropTarget()){
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY"))
		{
			const EntityId* id = (EntityId*)payload->Data;
			m_Id = *id;
		}
		ImGui::EndDragDropTarget();
	}

	RefreshManager();
}

void AIPanel::HandleDebugSettings()
{
	if (m_DebugDraw)
		Wiwa::RecastManager::DebugSettings();
}

void AIPanel::HandleRender()
{
	if (m_DebugDraw)
		Wiwa::RecastManager::Render();
}

void AIPanel::HandleCleanUp()
{
	Wiwa::RecastManager::Cleanup();
	m_Id = -1;
}

void AIPanel::HandleSave()
{
	Wiwa::RecastManager::Save();
}

void AIPanel::HandleLoad()
{
	Wiwa::RecastManager::Load(Wiwa::SceneManager::getActiveScene());
}

void AIPanel::RefreshManager()
{
	if (m_Id == -1)
		return;

	Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
	Wiwa::RecastManager::m_Id = m_Id;
	Wiwa::RecastManager::m_MeshIt = em.GetComponentIterator<Wiwa::Mesh>(m_Id);
}

void AIPanel::OnEvent(Wiwa::Event& e)
{
	Wiwa::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Wiwa::SceneChangeEvent>({ &AIPanel::OnSceneChange, this });
}

bool AIPanel::OnSceneChange(Wiwa::SceneChangeEvent& e)
{
	m_Id = -1;
	return false;
}
