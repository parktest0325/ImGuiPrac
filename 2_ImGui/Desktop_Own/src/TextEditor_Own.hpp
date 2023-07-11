#pragma once

#include <cstring>
#include <string>
#include <cstdint>
#include <string_view>
#include <filesystem>
#include "WindowBase.hpp"

namespace fs = std::filesystem;

class TextEditor : public WindowBase
{
public:
    static constexpr auto bufferSize = std::size_t{1024};
    static constexpr auto popUpFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    static constexpr auto popUpSize = ImVec2(300.0f, 100.0f);
    static constexpr auto popUpButtonSize = ImVec2(120.0f, 0.0f);
    static constexpr auto popUpPos = ImVec2(1280.0f / 2.0f - popUpSize.x / 2.0f,
                                            720.0f / 2.0f - popUpSize.y / 2.0f);

public:
    TextEditor() : currentFilename({})
    {
        std::memset(textBuffer, 0, bufferSize);
    }
    virtual ~TextEditor(){};

    virtual void Draw(std::string_view label, bool *open = nullptr) final;

private:
    void DrawMenu();
    void DrawContent();
    void DrawInfo();

    void DrawSavePopup();
    void DrawLoadPopup();

    void SaveToFile(std::string_view filename);
    void LoadFromFile(std::string_view filename);

    std::string GetFileExtension(std::string_view filename);

private:
    char textBuffer[bufferSize];
    std::string currentFilename;
};

void render(TextEditor &window_obj);
