#include <iostream>
#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "render.hpp"

void WindowClass::Draw(std::string_view label)
{
    constexpr static auto window_flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    constexpr static auto window_size = ImVec2(1280.0F, 720.0F);
    constexpr static auto window_pos = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);

    ImGui::Begin(label.data(), nullptr, window_flags);

    // 맨 위 메뉴 모음줄    
    DrawMenu();
    ImGui::Separator();
    // 디렉토리, 모든 파일 출력   
    DrawContent();

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 100.0F);

    ImGui::Separator();
    // 파일이나 디렉토리의 이름을 변경하거나 삭제하는 부분   
    DrawActions();
    ImGui::Separator();
    // 현재 폴더의 파일 확장자 필터링   
    DrawFilter();

    ImGui::End();
}

void WindowClass::DrawMenu()
{
    if (ImGui::Button("Go Up"))
    {
        if (currentPath.has_parent_path())
        {
            currentPath = currentPath.parent_path();
        }
    }

    ImGui::SameLine();
    ImGui::Text("Currenty directory: %s", currentPath.string().c_str());
}

void WindowClass::DrawContent()
{
    for (const auto& entry : fs::directory_iterator(currentPath))
    {
        const auto is_selected = entry.path() == selectedPath;
        const auto is_directory = entry.is_directory(); 
        const auto is_file = entry.is_regular_file();
        auto entry_name = entry.path().filename().string();

        if (is_directory)
            entry_name = "[D] " + entry_name;
        else if (is_file)
            entry_name = "[F] " + entry_name;

        if (ImGui::Selectable(entry_name.c_str(), is_selected))
        {
            if (is_directory)
                currentPath /= entry.path().filename();
            selectedPath = entry.path();
        }
    }
}

void WindowClass::DrawActions()
{
    if (fs::is_directory(selectedPath))
        ImGui::Text("Selected dir: %s", selectedPath.string().c_str());
    else if (fs::is_regular_file(selectedPath))
        ImGui::Text("Selected file: %s", selectedPath.string().c_str());
    else
    {
        ImGui::Text("Nothing selected!");
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha,
                            ImGui::GetStyle().Alpha * 0.0f);
        ImGui::Button("Non-clickable button");
        ImGui::PopStyleVar();
        return;
    }

    if (fs::is_regular_file(selectedPath) && ImGui::Button("Open"))
        openFileWithDefaultEditor();

    ImGui::SameLine();

    if (ImGui::Button("Rename"))
    {
        renameDialogOpen = true;
        ImGui::OpenPopup("Rename File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete"))
    {
        deleteDialogOpen = true;
        ImGui::OpenPopup("Delete File");
    }

    renameFilePopup();
    deleteFilePopup();
}

void WindowClass::openFileWithDefaultEditor()
{
#ifdef _WIN32
    const auto command = "start \"\" \"" + selectedPath.string() + "\"";
#elif __APPLE__
    const auto command = "open \"" + selectedPath.string() + "\"";
#elif __linux__
    const auto command = "xdg-open \"" + selectedPath.string() + "\"";
#endif
    std::system(command.c_str());
}

void WindowClass::deleteFilePopup()
{
    if (ImGui::BeginPopupModal("Delete File"))
    {
        ImGui::Text("Are you sure you want to delete %s?",
            selectedPath.string().c_str());

        if (ImGui::Button("Yes"))
        {
            if (deleteFile(selectedPath))
                selectedPath.clear();
            deleteDialogOpen = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("No"))
        {
            deleteDialogOpen = false;
            ImGui::CloseCurrentPopup();
        }


        ImGui::EndPopup();
    }
}

void WindowClass::renameFilePopup()
{
    if (ImGui::BeginPopupModal("Rename File", &renameDialogOpen))
    {
        static char buffer_name[512] = {0, };

        ImGui::Text("New name: ");
        ImGui::InputText("###newName", buffer_name, sizeof(buffer_name));

        if (ImGui::Button("Rename"))
        {
            auto new_path = selectedPath.parent_path() / buffer_name;
            if (renameFile(selectedPath, new_path))
            {
                selectedPath = new_path;
                renameDialogOpen = false;
                std::memset(buffer_name, 0, sizeof(buffer_name));
               // ImGui::CloseCurrentPopup();

            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            renameDialogOpen = false;
            std::memset(buffer_name, 0, sizeof(buffer_name));
        }


        ImGui::EndPopup();
    }
}

bool WindowClass::deleteFile(const fs::path& path)
{
    try
    {
        fs::remove(path);
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }

}
bool WindowClass::renameFile(const fs::path& old_path, const fs::path& new_path)
{
    try
    {
        fs::rename(old_path, new_path);
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

void WindowClass::DrawFilter()
{
    static char extension_filter[16] = { 0, };
    ImGui::Text("Filter by extension");
    ImGui::SameLine();
    ImGui::InputText("###inFilter", extension_filter, sizeof(extension_filter));

    if (std::strlen(extension_filter) == 0)
        return;

    auto filtered_file_count = 0;
    for (const auto& entry : fs::directory_iterator(currentPath))
    {
        if (!fs::is_regular_file(entry))
            continue;

        if (entry.path().extension().string() == extension_filter)
            ++filtered_file_count;
    }

    ImGui::Text("filtered file count: %d", filtered_file_count);
}

void render(WindowClass &window_obj)
{
    window_obj.Draw("Label");
}
