#include "GamePanel.h"
#include <Wiwa/core/Application.h>
#include <Wiwa/scene/SceneManager.h>
#include <Wiwa/core/Renderer3D.h>
#include "../../Utils/EditorUtils.h"

#include <Wiwa/render/RenderManager.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/core/Input.h>


GamePanel::GamePanel(EditorLayer* instance)
	: Panel("Game", ICON_FK_GAMEPAD,instance)
{
}

GamePanel::~GamePanel()
{
}

void GamePanel::Draw()
{
    Wiwa::CameraManager& cameraManager = Wiwa::SceneManager::getActiveScene()->GetCameraManager();

    ImGui::Begin(iconName.c_str(), &active);

    ImGui::Checkbox("Show stats", &m_ShowStats);
    ImGui::Separator();


    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

    Wiwa::Size2i resolution = Wiwa::Application::Get().GetTargetResolution();
    float ar = resolution.w / (float)resolution.h;
    Wiwa::Size2f scales = { viewportPanelSize.x / (float)resolution.w, viewportPanelSize.y / (float)resolution.h };

    float scale = scales.x < scales.y ? scales.x : scales.y;
    ImVec2 isize = { resolution.w * scale, resolution.h * scale };

    Wiwa::Input::OverrideMouseInputs(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    if (ImGui::IsWindowHovered()) {
        ImVec2 rectPos = ImGui::GetItemRectMin();

        // Calculate mouse position in viewport (0 to 1)
        ImVec2 mpos = ImGui::GetMousePos();
        ImVec2 cspos = ImGui::GetCursorScreenPos();

        ImVec2 rpos = { mpos.x - rectPos.x, mpos.y - rectPos.y };
        CLAMP(rpos.x, 0.0f, isize.x);
        CLAMP(rpos.y, 0.0f, isize.y);

        Wiwa::Vector2f v2f = { rpos.x / (float)isize.x, rpos.y / (float)isize.y };

        Wiwa::Vector2f rel2f = m_LastPos - v2f;
        rel2f.x /= rel2f.x == 0.0f ? 1.0f : abs(rel2f.x);
        rel2f.y /= rel2f.y == 0.0f ? 1.0f : abs(rel2f.y);

        Wiwa::Input::OverrideMouseInputs(v2f.x * 1920, v2f.y * 1080, m_LastPos.x * 1920, m_LastPos.y * 1080, rel2f.x, rel2f.y);

        m_LastPos = v2f;
    }

    if (cameraManager.getCameraSize() > 0)
    {

        ImTextureID tex = (ImTextureID)(intptr_t)Wiwa::RenderManager::getColorTexture();

        ImVec2 cpos = ImGui::GetCursorPos();
        cpos.x = (viewportPanelSize.x - isize.x) / 2;
        ImGui::SetCursorPos(cpos);
        ImGui::Image(tex, isize, ImVec2(0, 1), ImVec2(1, 0));

        ImVec2 rectPos = ImGui::GetItemRectMin();
        //FPS widget

        if (m_ShowStats)
        {
            ImVec2 rectSize(rectPos.x + 200.0f, rectPos.y + 90.0f);
            ImGui::GetWindowDrawList()->AddRectFilled(
                ImVec2(rectPos.x + 10, rectPos.y),
                rectSize,
                IM_COL32(30, 30, 30, 128)
            );

            ImGui::GetWindowDrawList()->AddRect(
                ImVec2(rectPos.x + 10, rectPos.y),
                rectSize,
                IM_COL32(255, 255, 255, 30)
            );
            float y = cpos.y + 5.0f;
            float x = cpos.x + 15.0f;
            ImGui::SetCursorPos(ImVec2(x, y));
            ImGui::TextColored(ImColor(255, 255, 255, 128), "FPS");
            ImGui::SetCursorPos(ImVec2(x + 60.0f, y));
            ImGui::TextColored(ImColor(255, 255, 255, 128), "%.f FPS", 1000 / Wiwa::Time::GetDeltaTime());

            ImGui::SetCursorPos(ImVec2(x, y + 20.0f));
            ImGui::TextColored(ImColor(255, 255, 255, 128), "Frame time");
            ImGui::SetCursorPos(ImVec2(x + 100.0f, y + 20.0f));
            ImGui::TextColored(ImColor(255, 255, 255, 128), "%.3f ms", Wiwa::Time::GetDeltaTime());

            ImGui::SetCursorPos(ImVec2(x, y + 40.0f));
            ImGui::TextColored(ImColor(255, 255, 255, 128), "Play time");
            ImGui::SetCursorPos(ImVec2(x + 100.0f, y + 40.0f));
            ImGui::TextColored(ImColor(255, 255, 255, 128), "%.3f s", Wiwa::Time::GetTime());

            ImGui::SetCursorPos(ImVec2(x, y + 60.0f));
            ImGui::TextColored(ImColor(255, 255, 255, 128), "Time scale");
            ImGui::SetCursorPos(ImVec2(x + 100.0f, y + 60.0f));
            ImGui::TextColored(ImColor(255, 255, 255, 128), "%.2f", Wiwa::Time::GetTimeScale());
        }
    }
    else
    {
        TextCentered("No cameras to display, please create a camera to render the scene!");
    }
    ImGui::End();
}

void GamePanel::Update()
{
}
