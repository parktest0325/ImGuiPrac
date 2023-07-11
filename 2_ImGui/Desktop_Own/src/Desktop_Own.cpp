#include <iostream>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "Desktop_Own.hpp"

void Desktop::Draw(std::string_view label, bool *open)
{
    constexpr static auto window_flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs;
    constexpr static auto window_size = ImVec2(1280.0F, 720.0F);
    constexpr static auto window_pos = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);

    ImGui::Begin(label.data(), open, window_flags);

    DrawDesktop();
    DrawTaskbar();

    ImGui::End();
}

void Desktop::DrawDesktop()
{
    for (auto &icon : icons)
        icon.Draw();
}

void Desktop::DrawTaskbar()
{
    constexpr static auto taskbar_flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoTitleBar;
    constexpr static auto taskbar_size = ImVec2(1280.0F, 40.0F);
    constexpr static auto taskbar_pos = ImVec2(0.0F, 720.0F - taskbar_size.y);

    static auto open_taskbar = false;

    ImGui::SetNextWindowSize(taskbar_size);
    ImGui::SetNextWindowPos(taskbar_pos);

    ImGui::Begin("Taskbar", nullptr, taskbar_flags);

    if (ImGui::Button("All Icons", ImVec2(100.0F, 30.0F)))
    {
        ImGui::OpenPopup("My Programs");
        open_taskbar = true;
    }

    if (open_taskbar)
        ShowIconList();

    ImGui::SameLine();

    static auto theme_open = false;
    if (ImGui::Button("Theme", ImVec2(100.0F, 30.0F)) || theme_open)
    {
        theme_open = true;
        DrawColorsSettings(&theme_open);
    }

    ImGui::SameLine();

    ImGui::SetCursorPosX(mainWindowSize.x - 100.0f);
    static auto clock_open = false;
    clock.GetTime();
    const auto time = fmt::format("{}:{}", clock.hrs, clock.mins);

    if (ImGui::Button(time.data(), ImVec2(100.0f, 30.0f)) || clock_open)
    {
        clock.Draw("clockWindow");
        clock_open = true;
    }

    if (clock_open && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        clock_open = false;

    ImGui::End();
}

void Desktop::ShowIconList(bool *open)
{
    const auto selectable_height = ImGui::GetTextLineHeightWithSpacing();
    const auto popup_height = selectable_height * numIcons + 40.0f;

    ImGui::SetNextWindowSize(ImVec2(100.0f, popup_height), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0.0f, 680.0f - popup_height),
                            ImGuiCond_Always);

    if (ImGui::BeginPopupModal("My Programs", open, ImGuiWindowFlags_NoResize))

    {
        for (auto &icon : icons)
        {
            if (ImGui::Selectable(icon.label.data()))
            {
                icon.popupOpen = true;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
}


void Desktop::Icon::Draw()
{
    constexpr static auto flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowPos(position, ImGuiCond_FirstUseEver);
    ImGui::Begin(fmt::format("###{}", label).data(), nullptr, flags);
    if (ImGui::Button(label.data(), ImVec2(100.0F, 50.0F)) || popupOpen)
    {
        popupOpen = true;
        base->Draw(label, &popupOpen);
    }
    ImGui::End();
}

void render(Desktop &window_obj)
{
    window_obj.Draw("Desktop_Own");
}
