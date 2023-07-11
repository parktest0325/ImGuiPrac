#include <iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <string>
#include <string_view>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "TextEditor_Own.hpp"

void TextEditor::Draw(std::string_view label, bool *open)
{
    ImGui::SetNextWindowSize(mainWindowSize);
    ImGui::SetNextWindowPos(mainWindowPos);

    ImGui::Begin(label.data(), open, mainWindowFlags | ImGuiWindowFlags_MenuBar);

    SettingsMenuBar();

    DrawMenu();
    ImGui::Separator();
    DrawContent();
    ImGui::Separator();
    DrawInfo();

    ImGui::End();
}

void TextEditor::DrawMenu()
{
    const auto ctrl_pressed = ImGui::GetIO().KeyCtrl;
    const auto esc_pressed =
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape));
    const auto s_pressed =
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S));
    const auto l_pressed =
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_L));



    if (ImGui::Button("Save") || (ctrl_pressed && s_pressed))
    {
        if (currentFilename.empty())
        {
            ImGui::OpenPopup("Save File");
        }
        else
        {
            SaveToFile(currentFilename);
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Load") || (ctrl_pressed && l_pressed))
    {
        ImGui::OpenPopup("Load File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        std::memset(textBuffer, 0, bufferSize);
    }

    DrawSavePopup();
    DrawLoadPopup();

}

void TextEditor::DrawContent()
{
    static constexpr auto inputTextSize = ImVec2(1200.0f, 625.0f);
    static constexpr auto lineNumberSize = ImVec2(30.0f, inputTextSize.y);
    static constexpr auto inputTextFlags =
        ImGuiInputTextFlags_AllowTabInput |
        ImGuiInputTextFlags_NoHorizontalScroll;

    ImGui::BeginChild("LineNumbers", lineNumberSize);

    const auto line_count = std::count(textBuffer, textBuffer + bufferSize, '\n') + 1;

    for (auto i = 1; i <= line_count; ++i)
        ImGui::Text("%d", i);

    ImGui::EndChild();

    ImGui::SameLine();

    float text_line_height = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 20)); 
    ImGui::InputTextMultiline("###inputField",
                              textBuffer,
                              bufferSize,
                              inputTextSize,
                              inputTextFlags);
    ImGui::PopStyleVar();
}

void TextEditor::DrawInfo()
{
    if (currentFilename.size() == 0)
    {
        ImGui::Text("No File Opened");
        return;
    }
    const auto file_extionsion = GetFileExtension(currentFilename);
    ImGui::Text("Opened file %s | File extionsion %s",
                currentFilename.data(),
                file_extionsion.data());
}

void TextEditor::DrawSavePopup()
{
    static char saveFilenameBuffer[256];
    const auto esc_pressed =
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape));

    ImGui::SetNextWindowSize(popUpSize);
    ImGui::SetNextWindowPos(popUpPos);
    if (ImGui::BeginPopupModal("Save File", nullptr, popUpFlags))
    {
        ImGui::InputText("Filename",
            saveFilenameBuffer,
            sizeof(saveFilenameBuffer));

        if (ImGui::Button("Save", popUpButtonSize))
        {
            SaveToFile(saveFilenameBuffer);
            currentFilename = saveFilenameBuffer;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", popUpButtonSize) || esc_pressed)
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void TextEditor::DrawLoadPopup()
{
    static char loadFilenameBuffer[256];
    const auto esc_pressed =
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape));

    ImGui::SetNextWindowSize(popUpSize);
    ImGui::SetNextWindowPos(popUpPos);
    if (ImGui::BeginPopupModal("Load File", nullptr, popUpFlags))
    {
        ImGui::InputText("Filename",
                         loadFilenameBuffer,
                         sizeof(loadFilenameBuffer));

        if (ImGui::Button("Load", popUpButtonSize))
        {
            LoadFromFile(loadFilenameBuffer);
            currentFilename = loadFilenameBuffer;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", popUpButtonSize) || esc_pressed)
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void TextEditor::SaveToFile(std::string_view filename)
{
    auto out = std::ofstream{filename.data()};
    if (out.is_open())
    {
        out << textBuffer;
        out.close();
    }
}

void TextEditor::LoadFromFile(std::string_view filename)
{
    auto in = std::ifstream{filename.data()};
    if (in.is_open())
    {
        auto buffer = std::stringstream{};
        buffer << in.rdbuf();
        std::memcpy(textBuffer, buffer.str().data(), bufferSize);
        in.close();
    }
}

std::string TextEditor::GetFileExtension(std::string_view filename)
{
    const auto file_path = fs::path{filename};

    return file_path.extension().string();
}

void render(TextEditor &window_obj)
{
    window_obj.Draw("TextEditor_Own");
}
