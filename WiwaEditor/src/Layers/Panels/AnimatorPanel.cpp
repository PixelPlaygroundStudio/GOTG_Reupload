#include "AnimatorPanel.h"

#include "imgui.h"

#include <Wiwa/core/Application.h>
#include <Wiwa/scene/SceneManager.h>
#include <Wiwa/ecs/components/Mesh.h>
#include <Wiwa/utilities/render/Animation.h>
#include <ImGuizmo.h>
#include <GraphEditor.h>


#include <Wiwa/core/ProjectManager.h>
#include <Wiwa/utilities/filesystem/FileSystem.h>

AnimatorPanel::AnimatorPanel(EditorLayer* instance)
	: Panel("Animator", ICON_FK_MALE, instance)
{
	delegate.AddTemplate();
}

AnimatorPanel::~AnimatorPanel()
{
}

void AnimatorPanel::Draw()
{
	Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();

	

	ImGui::Begin(iconName.c_str(), &active);

	

	static float w = 200.0f;
	static float h = 300.0f;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginChild("child1", ImVec2(w, h), true);

	

	ImGui::NewLine();

	if (ImGui::Button("Save file"))
	{
		SaveOnFile("try");
	}

	if (s_EntitySet && (m_CurrentID >= 0))
	{
		const char* entName = em.GetEntityName(m_CurrentID);
		std::string edit = entName;

		ImGui::Text("Name: %s", (char*)edit.c_str());
		ImGui::SameLine();
		ImGui::Text("(%i)", m_CurrentID);

		Wiwa::Model* model = Wiwa::Resources::GetResourceById<Wiwa::Model>(m_CurrentID);

		

		

		if (model != NULL) {
			//if (model->GetAnimations().size() != NULL)
			//{
			//	std::vector<Wiwa::Animation*> aux = model->GetAnimations();
			//	if (aux[0] != nullptr)
			//	{
			//		for (int i = 0; i < aux.size(); i++)
			//		{

			//			if (ImGui::Button(aux[i]->name.c_str(), ImVec2(150, 40)))
			//			{
			//				GraphEditorDelegate::Node node = GraphEditorDelegate::Node();
			//				node.name = aux[i]->name.c_str();
			//				node.templateIndex = 0;
			//				node.mRect = { 0, 0, 100, 75 };
			//				node.mSelected = false;
			//			
			//				delegate.mNodes.push_back(node);
			//														NEED TO ADD TEMPLATE WITH NODE AND ADD ID TEMPLATE TO NODE, I FEEL LIKE THIS IS SUPER INEFICIENT
			//			}

			//		}
			//	}
			//}
		}
	}
	else
	{
		ImGui::Text("Select an entity!");
	}

	
	//if (ImGui::Button("add"))
	//{
	//	delegate.test++;
	//	delegate.mTemplates[0].mInputCount++;
	//	WI_INFO("{}", delegate.test);
	//}
	
	ImGui::EndChild();
	ImGui::SameLine();
	
	if (ImGui::InvisibleButton("splitter", ImVec2(8.0f, h))) 
	{
		w += ImGui::GetIO().MouseDelta.x;
	}
	ImGui::SameLine();
	ImGui::BeginChild("child2", ImVec2(0, h), true);
	

	GraphEditor::Show(delegate, options, viewState, true);

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::wstring ws(path);
			std::string pathS(ws.begin(), ws.end());
			std::filesystem::path p = pathS;
			if (p.extension() == ".json" || p.extension() == ".JSON")
			{
				std::filesystem::path src = Wiwa::FileSystem::RemoveFolderFromPath("assets\\", pathS);
				
				
				LoadOnFile(src.string());
				//LoadOnFile(p.string().c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::EndChild();
	
	ImGui::PopStyleVar();
	ImGui::End();


	

}

	
void AnimatorPanel::OnEvent(Wiwa::Event& e)
{
	Wiwa::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<EntityChangeEvent>({ &AnimatorPanel::OnEntityChangeEvent, this });
	dispatcher.Dispatch<Wiwa::SceneChangeEvent>({ &AnimatorPanel::OnSceneChangeEvent, this });
}

bool AnimatorPanel::OnEntityChangeEvent(EntityChangeEvent& e)
{
	m_CurrentID = e.GetResourceId();
	s_EntitySet = true;
	return false;
}

bool AnimatorPanel::OnSceneChangeEvent(Wiwa::SceneChangeEvent& e)
{
	m_CurrentID = -1;
	s_EntitySet = false;
	return false;
}

void AnimatorPanel::SaveOnFile(const char* modelName)
{
	//const char finalName = "library/" + modelName;
	//WI_INFO("{}", finalName);

	// save library and assets

	Wiwa::JSONDocument animFile;

	for (int i = 0; i < delegate.mNodes.size(); i++)
	{
		
		SaveNode(&animFile, i);
	}

	for (int i = 0; i < delegate.mLinks.size(); i++)
	{
		SaveLink(&animFile, i);
	}

	for (int i = 0; i < delegate.mTemplates.size(); i++)
	{
		SaveTemplate(&animFile, i);
	}

	animFile.save_file("library/wry.json");
	animFile.save_file("assets/wry.json");
	
}

void AnimatorPanel::SaveNode(Wiwa::JSONDocument *file, int index)
{
	std::string name = "node" + std::to_string(index);
	file->AddMemberObject(name.c_str()).AddMember("name", delegate.mNodes[index].name).AddMember("template", delegate.mNodes[index].templateIndex)
		.AddMemberObject("Rect").AddMember("min_x", delegate.mNodes[index].mRect.Min.x).AddMember("min_y", delegate.mNodes[index].mRect.Min.y)
		.AddMember("max_x", delegate.mNodes[index].mRect.Max.x).AddMember("max_y", delegate.mNodes[index].mRect.Max.y);
	
}

void AnimatorPanel::SaveTemplate(Wiwa::JSONDocument* file, int index)
{
	std::string name = "template" + std::to_string(index);
	Wiwa::JSONValue value = file->AddMemberObject(name.c_str());

	SaveColor(value, delegate.mTemplates[index].mBackgroundColor, "background_color");
	SaveColor(value, delegate.mTemplates[index].mBackgroundColorOver, "background_color_over");
	SaveColor(value, delegate.mTemplates[index].mHeaderColor, "header_color");

	value.AddMember("input_count", delegate.mTemplates[index].mInputCount).AddMember("output_count", delegate.mTemplates[index].mOutputCount);
}

void AnimatorPanel::LoadOnFile(std::string name)
{
	std::string finalName = "library/" + name;
	Wiwa::JSONDocument file(finalName.c_str());

	if (file.HasMember("node0"))
	{
		Wiwa::JSONValue node(file["node0"]);
		if (node.HasMember("name"))
		{
			WI_INFO("bazinga");
		}
	}
	
	// LOAD NODES
	int index = 0;
	bool keepLoop = true;
	std::string nName = "";
	while (keepLoop)
	{
		nName = "node" + std::to_string(index);
		if (file.HasMember(nName.c_str()))
		{
			// LOAD NODE
			index++;
		}
		else
		{
			keepLoop = false;
		}

	}
}

void AnimatorPanel::LoadNode(Wiwa::JSONValue value)
{
}

void AnimatorPanel::SaveColor(Wiwa::JSONValue value, ImColor color, std::string name)
{
	
	value.AddMemberObject(name.c_str()).AddMember("r", color.Value.x).AddMember("g", color.Value.y).AddMember("b", color.Value.z).AddMember("a", color.Value.w);

}

void AnimatorPanel::SaveLink(Wiwa::JSONDocument* file, int index)
{
	std::string name = "link" + std::to_string(index);
	file->AddMemberObject(name.c_str()).AddMember("input node index", delegate.mLinks[index].mInputNodeIndex).AddMember( "input slot index", delegate.mLinks[index].mInputSlotIndex)
		.AddMember("output node index", delegate.mLinks[index].mOutputNodeIndex).AddMember("output slot index", delegate.mLinks[index].mOutputSlotIndex);
}
