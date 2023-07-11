#include <iostream>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "Desktop_Own.hpp"

void Desktop::Draw(std::string_view label)
{
    constexpr static auto window_flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs;
    constexpr static auto window_size = ImVec2(1280.0F, 720.0F);
    constexpr static auto window_pos = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);

    ImGui::Begin(label.data(), nullptr, window_flags);

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

    if (ImGui::Button("All Icons"))
    {
        ImGui::OpenPopup("My Programs");
        open_taskbar = true;
    }

    if (open_taskbar)
        ShowIconList();

    ImGui::SameLine();

    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x);
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
    constexpr static auto icon_window_flags = ImGuiWindowFlags_NoCollapse |
                                          ImGuiWindowFlags_NoScrollbar;
    constexpr static auto button_size = ImVec2(100.0f, 50.0f);

    const auto label_icon_window = fmt::format("IconWindow##{}", label);
    const auto label_icon_popup = fmt::format("IconPopup##{}", label);

    ImGui::SetNextWindowSize(position, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(
        ImVec2(button_size.x + 35.0f, button_size.y + 35.0f),
        ImGuiCond_FirstUseEver);

    ImGui::Begin(label_icon_window.data(), nullptr, icon_window_flags);
    if (ImGui::Button(label.data(), button_size))
    {
        ++clickCount;
    }
    if (clickCount >= 1 || popupOpen)
    {
        ImGui::OpenPopup(label_icon_popup.data());
        clickCount = 0;
        popupOpen = true;
    }
    if (ImGui::BeginPopupModal(label_icon_popup.data(), &popupOpen))
    {
        ImGui::Text("Hi");
        if (ImGui::Button("Close"))
        {
            popupOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void render(Desktop &window_obj)
{
    window_obj.Draw("Desktop_Own");
}
