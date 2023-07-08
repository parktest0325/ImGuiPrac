#pragma once

#include <cstdint>
#include <string_view>
#include <imgui.h>
#include <vector>

class WindowClass
{
public:
    using PointData = std::tuple<ImVec2, ImColor, float>;

    static constexpr auto popUpFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    static constexpr auto popUpSize = ImVec2(300.0f, 100.0f);
    static constexpr auto popUpButtonSize = ImVec2(120.0f, 0.0f);
    static constexpr auto popUpPos = ImVec2(1280.0f / 2.0f - popUpSize.x / 2.0f,
                                            720.0f / 2.0f - popUpSize.y / 2.0f);

public:
    WindowClass()
        : points({}), canvasPos({}), currentDrawColor(ImColor(255, 255, 255)),
          pointDrawSize(2.0F), filenameBuffer("test.bin"){};

    void Draw(std::string_view label);

private:
    void DrawMenu();
    void DrawCanvas();
    void DrawColorButtons();
    void DrawSizeSettings();

    void SaveToImageFile(std::string_view filename);
    void LoadFromImageFile(std::string_view filename);
    void ClearCanvas();

    void DrawSavePopup();
    void DrawLoadPopup();

private:
    std::uint32_t numRows = 800;
    std::uint32_t numCols = 600;
    std::uint32_t numChannels = 3;
    ImVec2 canvasSize = ImVec2(static_cast<float>(numRows), static_cast<float>(numCols));

    std::vector<PointData> points;
    ImVec2 canvasPos;
    ImColor currentDrawColor;
    float pointDrawSize;

    char filenameBuffer[256];
};

void render(WindowClass &window_obj);
