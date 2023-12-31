#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <implot.h>


#include "DiffViewer_Own.hpp"

void DiffViewer::Draw(std::string_view label, bool *open)
{
    ImGui::SetNextWindowSize(mainWindowSize);
    ImGui::SetNextWindowPos(mainWindowPos);

    ImGui::Begin(label.data(), open, mainWindowFlags);

    DrawSelection();
    DrawDiffView();
    DrawStats();

    ImGui::End();
}

void DiffViewer::DrawSelection()
{
    ImGui::InputText("Left", &filePath1);
    ImGui::SameLine();
    if (ImGui::Button("Save###Left"))
        SaveFileContent(filePath1, fileContent1);
    ImGui::InputText("Right", &filePath2);
    ImGui::SameLine();
    if (ImGui::Button("Save###Right"))
        SaveFileContent(filePath2, fileContent2);

    if (ImGui::Button("Compare"))
    {
        fileContent1 = LoadFileContent(filePath1);
        fileContent2 = LoadFileContent(filePath2);

        CreateDiff();
    }
}

void DiffViewer::DrawDiffView()
{
    constexpr static auto swap_width = 40.0f;
    const auto parent_size = ImVec2(ImGui::GetContentRegionAvail().x, 500.0f);
    const auto child_size = ImVec2(parent_size.x / 2.0f - swap_width, parent_size.y);

    const auto swap_size = ImVec2(swap_width, child_size.y);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::BeginChild("Parent", parent_size, true);

    if (ImGui::BeginChild("Diff1", child_size, false))
    {
        for (std::size_t i = 0; i < fileContent1.size(); ++i)
        {
            if (!diffResult1[i].empty())
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                                   "%s",
                                   fileContent1[i].data());
            else
                ImGui::Text("%s", fileContent1[i].data());
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    const auto line_height = ImGui::GetTextLineHeightWithSpacing();
    const auto button_size = ImVec2(15.0f, line_height);

    if (ImGui::BeginChild("Swap", swap_size, true))
    {
        for (std::size_t i = 0; i < diffResult1.size(); ++i)
        {
            const auto left_label = fmt::format("<##{}", i);
            const auto right_label = fmt::format(">##{}", i);

            if (!diffResult1[i].empty() || !diffResult2[i].empty())
            {
                if (ImGui::Button(left_label.data(), button_size))
                {
                    if (fileContent1.size() > i && fileContent2.size() > i)
                        fileContent1[i] = fileContent2[i];
                    else if (fileContent2.size() > i)
                        fileContent1.push_back(fileContent2[i]);
                    CreateDiff();
                }

                ImGui::SameLine();

                if (ImGui::Button(right_label.data(), button_size))
                {
                    if (fileContent2.size() > i && fileContent1.size() > i)
                        fileContent2[i] = fileContent1[i];
                    else if (fileContent1.size() > i)
                        fileContent2.push_back(fileContent1[i]);
                    CreateDiff();
                }
            }
            else
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + line_height);
            }
        }       
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if (ImGui::BeginChild("Diff2"))
    {
        for (std::size_t i = 0; i < fileContent2.size(); ++i)
        {
            if (!diffResult2[i].empty())
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                                   "%s",
                                   fileContent2[i].data());
            else
                ImGui::Text("%s", fileContent2[i].data());
        }
    }
    ImGui::EndChild();

    ImGui::EndChild();

    ImGui::PopStyleVar();
}

void DiffViewer::DrawStats()
{
    auto diff_lines_count = std::size_t{0};
    for (const auto& line : diffResult1)
    {
        if (!line.empty())
            ++diff_lines_count;
    }

    //ImGui::SetCursorPosY(ImGui::GetCursorPosY());
    //ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 20.0f);
    ImGui::Text("Diff lines count: %u", diff_lines_count);
}

DiffViewer::FileContent DiffViewer::LoadFileContent(std::string_view file_path)
{
    auto content = FileContent();

    auto in = std::ifstream(file_path.data());
    if (in.is_open())
    {
        auto line = std::string{};
        while (std::getline(in, line))
            content.push_back(line);
    }
    in.close();
    return content;
}

void DiffViewer::SaveFileContent(std::string_view file_path,
                                  FileContent fileContent)
{
    auto out = std::ofstream(file_path.data());
    if (out.is_open())
    {
        for (const auto& s : fileContent)
            out << s << '\n';
    }
    out.close();
}

void DiffViewer::CreateDiff()
{
    diffResult1.clear();
    diffResult2.clear();

    const auto max_num_lines =
        std::max(fileContent1.size(), fileContent2.size());

    for (std::size_t i = 0; i < max_num_lines; ++i)
    {
        const auto line1 = i < fileContent1.size() ? fileContent1[i] : "EMPTY";
        const auto line2 = i < fileContent2.size() ? fileContent2[i] : "EMPTY";

        if (line1 != line2)
        {
            diffResult1.push_back(line1);
            diffResult2.push_back(line2);
        }
        else
        {
            diffResult1.push_back("");
            diffResult2.push_back("");
        }
    }
}

void render(DiffViewer &window_obj)
{
    window_obj.Draw("DiffViewer_Own");
}
