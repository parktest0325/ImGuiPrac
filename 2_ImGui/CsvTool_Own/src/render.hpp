#pragma once

#include <vector>
#include <cstdint>
#include <string_view>

class WindowClass
{
public:
    static constexpr auto popUpFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    static constexpr auto popUpSize = ImVec2(300.0f, 100.0f);
    static constexpr auto popUpButtonSize = ImVec2(120.0f, 0.0f);
    static constexpr auto popUpPos = ImVec2(1280.0f / 2.0f - popUpSize.x / 2.0f,
                                            720.0f / 2.0f - popUpSize.y / 2.0f);

    static constexpr auto maxNumRows = 30;
    static constexpr auto maxNumCols = 8;

public:
    WindowClass()
        : colSize(0), rowSize(0), data({}), filenameBuffer("test.csv"){};
    void Draw(std::string_view label);

private:
    void DrawSizeButtons();
    void DrawIoButtons();
    void DrawTable();

    void DrawSavePopup();
    void DrawLoadPopup();

    void DrawValuePopup(const int row, const int col);

    void SaveToCsvFile(std::string_view filename);
    void LoadFromCsvFile(std::string_view filename);

    template <typename T>
    void PlotCellValue(std::string_view formatter, const T value);
    void SetPopupLayout();

private:
    std::int32_t colSize;
    std::int32_t rowSize;
    std::vector<std::vector<float>> data;

    char filenameBuffer[256];
};

void render(WindowClass &window_obj);
