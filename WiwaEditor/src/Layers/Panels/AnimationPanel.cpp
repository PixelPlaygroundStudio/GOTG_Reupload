#include "AnimationPanel.h"

#include "imgui.h"

#include <Wiwa/core/Application.h>
#include <Wiwa/scene/SceneManager.h>

#include <ImSequencer.h>

AnimationPanel::AnimationPanel(EditorLayer* instance)
	: Panel("Animation Editor", ICON_FK_SKATE, instance)
{
}

AnimationPanel::~AnimationPanel()
{
}

void AnimationPanel::Draw()
{
	Wiwa::SysInfo& info = Wiwa::Application::Get().GetSystemInfo();

	

	Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();

	ImGui::Begin(iconName.c_str(), &active);
	

	static float w = 400.0f;
	static float h = 300.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginChild("select", ImVec2(w, h), true);
	if (s_EntitySet && (m_CurrentID >= 0))
	{
		const char* entName = em.GetEntityName(m_CurrentID);
		std::string edit = entName;
		//std::vector<EntityId>* entities = em.GetParentEntitiesAlive();
		//EntityId entID = em.GetParentEntitiesAlive()->at(m_CurrentID);
		ImGui::Text("Entity:%s", (char*)edit.c_str());
		
		
		Wiwa::Mesh* meshId = em.GetComponent<Wiwa::Mesh>(m_CurrentID);
		if (meshId != nullptr)
		{
			Wiwa::Model* model = Wiwa::Resources::GetResourceById<Wiwa::Model>(meshId->meshId);
			model = model->getModelAt(meshId->modelIndex);
			if (ImGui::CollapsingHeader("Animations "))
			{
				//model->GetParent()->GetAnimations().size();
				ImGui::Dummy(ImVec2(20.0f, 0.0f));
				ImGui::SameLine();
				ImGui::BeginChild("Names");
				
				//for (int i = 0; i< model->GetParent()->GetAnimations().size(); i++)
				//{
				//	if (model->GetParent()->GetAnimations()[i] != nullptr)
				//	{
				//		if (ImGui::CollapsingHeader(model->GetParent()->GetAnimations()[i]->name.c_str()))
				//		{
				//			
				//			DisplayBones(model, meshId, i);
				//		}

				//		
				//	}
				//	
				//}
				ImGui::EndChild();
				
				
			}
		}	
	}
	else
	{
		ImGui::Text("Entity:Select an entity!");
		if (ImGui::CollapsingHeader("Animations "))
		{
			ImGui::Text("No animations (no entity selected)");
			
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();
	
	CreateSequencer();

	ImGui::PopStyleVar();
	ImGui::End();
}

void AnimationPanel::OnEvent(Wiwa::Event& e)
{
	Wiwa::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<EntityChangeEvent>({ &AnimationPanel::OnEntityChange, this});
	dispatcher.Dispatch<Wiwa::SceneChangeEvent>({ &AnimationPanel::OnSceneChange, this });
}
bool AnimationPanel::OnEntityChange(EntityChangeEvent& e)
{
	m_CurrentID = e.GetResourceId();
	s_EntitySet = true;


	return false;
}
bool AnimationPanel::OnSceneChange(Wiwa::SceneChangeEvent& e)
{
	m_CurrentID = -1;
	s_EntitySet = false;



	return false;
}

void AnimationPanel::DisplayBones(Wiwa::Model* model, Wiwa::Mesh* meshId, int animationNum)
{
	
	
	//ImGui::Button(model->GetParent()->GetAnimations()[animationNum]->name.c_str());
	//model->GetBoneInfo().size();
	ImGui::Text("test");
	
}

void AnimationPanel::CreateSequencer()
{
	ImGui::BeginChild("Editor", ImVec2(2000, 700), true);

	MySequence mySequence;
	mySequence.mFrameMin = -100;
	mySequence.mFrameMax = 1000;

	mySequence.myItems.push_back(MySequence::MySequenceItem{ 0, 10, 30, true });
	//mySequence.myItems.push_back(MySequence::MySequenceItem{ 1, 20, 30, true });
	//mySequence.myItems.push_back(MySequence::MySequenceItem{ 3, 12, 60, false });
	//mySequence.myItems.push_back(MySequence::MySequenceItem{ 2, 61, 90, false });
	//mySequence.myItems.push_back(MySequence::MySequenceItem{ 4, 90, 99, false });


	static int selectedEntry = 0;
	static int firstFrame = 0;
	static bool expanded = true;
	static int currentFrame = 100;

	ImGui::PushItemWidth(130);
	ImGui::InputInt("Frame Min", &mySequence.mFrameMin);
	ImGui::SameLine();
	ImGui::InputInt("Frame ", &currentFrame);
	ImGui::SameLine();
	ImGui::InputInt("Frame Max", &mySequence.mFrameMax);
	ImGui::PopItemWidth();
	Sequencer(&mySequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME);
	// add a UI to edit that particular item
	if (selectedEntry != -1)
	{
		const MySequence::MySequenceItem& item = mySequence.myItems[selectedEntry];
		ImGui::Text("I am a %s, please edit me", SequencerItemTypeNames[item.mType]);
		// switch (type) ....
	}


	ImGui::EndChild();
}






void MySequence::CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect)
{
	static const char* labels[] = { "Translation", "Rotation" , "Scale" };

	rampEdit.mMax = ImVec2(float(mFrameMax), 1.f);
	rampEdit.mMin = ImVec2(float(mFrameMin), 0.f);
	draw_list->PushClipRect(legendClippingRect.Min, legendClippingRect.Max, true);
	for (int i = 0; i < 3; i++)
	{
		ImVec2 pta(legendRect.Min.x + 30, legendRect.Min.y + i * 14.f);
		ImVec2 ptb(legendRect.Max.x, legendRect.Min.y + (i + 1) * 14.f);
		draw_list->AddText(pta, rampEdit.mbVisible[i] ? 0xFFFFFFFF : 0x80FFFFFF, labels[i]);
		if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
			rampEdit.mbVisible[i] = !rampEdit.mbVisible[i];
	}
	draw_list->PopClipRect();

	ImGui::SetCursorScreenPos(rc.Min);
	ImCurveEdit::Edit(rampEdit, ImVec2(rc.Max.x - rc.Min.x, rc.Max.y - rc.Min.y), 137 + index, &clippingRect);
}

void MySequence::CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
{
	rampEdit.mMax = ImVec2(float(mFrameMax), 1.f);
	rampEdit.mMin = ImVec2(float(mFrameMin), 0.f);
	draw_list->PushClipRect(clippingRect.Min, clippingRect.Max, true);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < rampEdit.mPointCount[i]; j++)
		{
			float p = rampEdit.mPts[i][j].x;
			if (p < myItems[index].mFrameStart || p > myItems[index].mFrameEnd)
				continue;
			float r = (p - mFrameMin) / float(mFrameMax - mFrameMin);
			float x =  ImLerp(rc.Min.x, rc.Max.x, r);
			draw_list->AddLine(ImVec2(x, rc.Min.y + 6), ImVec2(x, rc.Max.y - 4), 0xAA000000, 4.f);
		}
	}
	draw_list->PopClipRect();
}


