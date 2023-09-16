#pragma once

#include "Panel.h"

#include <Wiwa/events/ApplicationEvent.h>
#include <GraphEditor.h>
#include <array>

#include <Wiwa/utilities/json/JSONDocument.h>

struct GraphEditorDelegate : public GraphEditor::Delegate
{

    

    bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override
    {
        return true;
    }

    void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override
    {
        mNodes[nodeIndex].mSelected = selected;
    }

    void MoveSelectedNodes(const ImVec2 delta) override
    {
        for (auto& node : mNodes)
        {
            if (!node.mSelected)
            {
                continue;
            }
            node.mRect.Translate(delta);
        }
    }

    virtual void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override
    {
    }

    void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override
    {
        mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
    }

    void DelLink(GraphEditor::LinkIndex linkIndex) override
    {
        mLinks.erase(mLinks.begin() + linkIndex);
    }

    void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override
    {
        drawList->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
        drawList->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), "Draw");
    }

    const size_t GetNodeCount() override
    {
        return mNodes.size();
    }

    const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override
    {
        const auto& myNode = mNodes[index];
        return GraphEditor::Node
        {
            myNode.name,
            myNode.templateIndex,
            myNode.mRect,
            myNode.mSelected
            
        };
    }

    const size_t GetTemplateCount() override
    {
        return sizeof(mTemplates) / sizeof(GraphEditor::Template);
    }

    const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override
    {
        return  mTemplates.at(index); //mTemplates[index];
    }
    
    void ChangeInputCount(int index, int num) override 
    {
        mTemplates[index].mInputCount += num;
    }

    void ChangeOutputCount(int index, int num) override
    {
        mTemplates[index].mOutputCount += num;
    }

    std::vector<GraphEditor::Template> mTemplates;
    
    void AddTemplate() {
        GraphEditor::Template tem;
        tem.mHeaderColor = IM_COL32(160, 160, 180, 255);
        tem.mBackgroundColor = IM_COL32(100, 100, 140, 255);
        tem.mBackgroundColorOver = IM_COL32(110, 110, 150, 255);
        tem.mInputCount = 2;
        tem.mInputNames = nullptr;
        tem.mInputColors = nullptr;
        tem.mOutputCount = 2;
        tem.mOutputNames = nullptr;
        tem.mOutputColors = nullptr;
        mTemplates.push_back(tem);
    };

    //static inline GraphEditor::Template mTemplates[] = {
    //   {
    //        IM_COL32(160, 160, 180, 255),
    //        IM_COL32(100, 100, 140, 255),
    //        IM_COL32(110, 110, 150, 255),
    //        test,
    //        nullptr,
    //        nullptr,
    //        2,
    //        nullptr,
    //        nullptr
    //    },
    //
    //    {
    //        // IM_COL32(180, 160, 160, 255),
    //        // IM_COL32(140, 100, 100, 255),
    //        // IM_COL32(150, 110, 110, 255),
    //        // 3,
    //        // nullptr,
    //        // Array{ IM_COL32(200,100,100,255), IM_COL32(100,200,100,255), IM_COL32(100,100,200,255) },
    //        // 1,
    //        // Array{"MyOutput0"},
    //        // Array{ IM_COL32(200,200,200,255)}
    //     }
    //};

    
    const size_t GetLinkCount() override
    {
        return mLinks.size();
    }

    const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override
    {
        return mLinks[index];
    }



    struct Node
    {
        const char* name;
        GraphEditor::TemplateIndex templateIndex;
        ImRect mRect;
        bool mSelected;
    };

    std::vector<Node> mNodes = {
        {
            "Test01",
            0,
            {0, 0, 100, 75},
            false
        },
        {
            "Test02",
            0,
            {0, 0, 100, 75},
            false
        },
    };

    std::vector<GraphEditor::Link> mLinks = { };
};

class AnimatorPanel : public Panel
{
public:
	AnimatorPanel(EditorLayer* instance);
	virtual ~AnimatorPanel();

	void Draw() override;

	
	void OnEvent(Wiwa::Event&) override;
	bool OnEntityChangeEvent(EntityChangeEvent& e);
	bool OnSceneChangeEvent(Wiwa::SceneChangeEvent& e);

    void SaveOnFile(const char* modelName);
    void SaveNode(Wiwa::JSONDocument *file, int index);
    void SaveLink(Wiwa::JSONDocument *file, int index);
    void SaveTemplate(Wiwa::JSONDocument* file, int index);
    void SaveColor(Wiwa::JSONValue value, ImColor color, std::string name);

    void LoadOnFile(std::string name);
    void LoadNode(Wiwa::JSONValue value);
private:


    GraphEditor::Options options;
    GraphEditorDelegate delegate;
    GraphEditor::ViewState viewState;

	int m_CurrentID = 0;
	bool s_EntitySet = false;

};


