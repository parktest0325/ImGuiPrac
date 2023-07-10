#include <iostream>
#include <vector>
#include <cstdint>
#include <sstream>
#include <string>
#include <fstream>

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

    DrawSizeButtons();
    ImGui::Separator();
    DrawIoButtons();
    ImGui::Separator();
    DrawTable();

    ImGui::End();
}

void WindowClass::DrawSizeButtons()
{
    auto user_control_rows = false;
    auto user_control_cols = false;

    auto slider_value_rows = rowSize;
    auto slider_value_cols = colSize;

    ImGui::Text("Num Rows: ");
    ImGui::SameLine();
    if (ImGui::SliderInt("##numRows", &slider_value_rows, 0, maxNumRows))
    {
        user_control_rows = true;
        rowSize = slider_value_rows;
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Row") && rowSize < maxNumRows)
    {
        ++rowSize;
        user_control_rows = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Drop Row") && rowSize > 0)
    {
        --rowSize;
        user_control_rows = true;
    }

    ImGui::Text("Num Cols: ");
    ImGui::SameLine();
    if (ImGui::SliderInt("##numCols", &slider_value_cols, 0, maxNumCols))
    {
        user_control_cols = true;
        colSize = slider_value_cols;
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Col") && colSize < maxNumCols)
    {
        ++colSize;
        user_control_cols = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Drop Col") && colSize > 0)
    {
        --colSize;
        user_control_cols = true;
    }

    if (user_control_rows)
    {
        data.resize(rowSize, std::vector<float>(colSize, 0.0f));
    }
    else if (user_control_cols)
    {
        for (auto row = 0; row < rowSize; ++row)
        {
            data[row].resize(colSize, 0.0f);
        }
    }
 }

void WindowClass::DrawIoButtons()
{
    if (ImGui::Button("Save"))
        ImGui::OpenPopup("Save File");
    ImGui::SameLine();
    if (ImGui::Button("Load"))
        ImGui::OpenPopup("Load File");
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
    {
        data.clear();       
        rowSize = 0;
        colSize = 0;
    }
    DrawSavePopup();
    DrawLoadPopup();
}

void WindowClass::DrawTable()
{
    constexpr static auto table_flags =
        ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuter;

    static auto row_clicked = 0;
    static auto col_clicked = 0;

    if (colSize == 0)
        return;

    ImGui::BeginTable("Table", colSize, table_flags);
    for (std::int32_t col = 0; col < colSize; ++col)
    {
        const auto col_name = fmt::format("{}", 'A' + col);
        ImGui::TableSetupColumn(col_name.data(),
                                ImGuiTableColumnFlags_WidthFixed,
                                1280.0f / static_cast<float>(colSize));
    }

    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
    for (std::int32_t col = 0; col < colSize; ++col)
        PlotCellValue("%c", 'A' + col);

    for (std::int32_t row = 0; row < rowSize; ++row)
    {
        for (std::int32_t col = 0; col < colSize; ++col)
        {
            PlotCellValue("%f", data[row][col]);
            if (ImGui::IsItemClicked())
            {
                ImGui::OpenPopup("Change Value");
                row_clicked = row;
                col_clicked = col;
            }
            else if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Cell: (%d, %d)", row, col);
            }
        }
        ImGui::TableNextRow();
    }
    DrawValuePopup(row_clicked, col_clicked);
    ImGui::EndTable();
}

template <typename T>
void WindowClass::PlotCellValue(std::string_view formatter, const T value)
{
    ImGui::TableNextColumn();
    ImGui::Text(formatter.data(), value);
}


void WindowClass::DrawSavePopup()
{
    SetPopupLayout();
    if (ImGui::BeginPopupModal("Save File", nullptr, popUpFlags))
    {
        ImGui::InputText("Filename", filenameBuffer, sizeof(filenameBuffer));

        if (ImGui::Button("Save", popUpButtonSize))
        {
            SaveToCsvFile(filenameBuffer);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", popUpButtonSize))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void WindowClass::DrawLoadPopup()
{
    SetPopupLayout();
    if (ImGui::BeginPopupModal("Load File", nullptr, popUpFlags))
    {
        ImGui::InputText("Filename", filenameBuffer, sizeof(filenameBuffer));

        if (ImGui::Button("Load", popUpButtonSize))
        {
            LoadFromCsvFile(filenameBuffer);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", popUpButtonSize))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void WindowClass::DrawValuePopup(const int row, const int col)
{
    static char buffer[64] = { 0, };
    const auto esc_pressed =
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape));

    SetPopupLayout();
    if (ImGui::BeginPopupModal("Change Value", nullptr, popUpFlags))
    {
        const auto label = fmt::format("##{}_{}", row, col);
        ImGui::InputText(label.data(), buffer, sizeof(buffer));

        if (ImGui::Button("Save", popUpButtonSize))
        {
            data[row][col] = std::stof(buffer);
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

void WindowClass::SaveToCsvFile(std::string_view filename)
{
    auto out = std::ofstream{filename.data()};
    if (!out.is_open())
        return;

    for (std::int32_t row = 0; row < rowSize; ++row)
    {
        for (std::int32_t col = 0; col < colSize; ++col)
        {
            out << data[row][col] << ",";
        }
        out << "\n";
    }


    out.close();
}

void WindowClass::LoadFromCsvFile(std::string_view filename)
{
    auto in = std::ifstream{filename.data()};
    if (!in.is_open())
        return;

    auto line = std::string{};
    auto num_rows = 0U;

    while (std::getline(in, line))
    {
        auto ss = std::istringstream(line);
        auto row = std::vector<float>{};
        auto value = std::string{};

        while (std::getline(ss, value, ','))
            row.push_back(std::stof(value));
        data.push_back(row);
        ++num_rows;
    }
    in.close();

    rowSize = num_rows;
    if (rowSize > 0U)
        colSize = static_cast<std::int32_t>(data[0].size());
    else
        colSize = 0;
}

void WindowClass::SetPopupLayout()
{
    ImGui::SetNextWindowSize(popUpSize);
    ImGui::SetNextWindowPos(popUpPos);
}

void render(WindowClass &window_obj)
{
    window_obj.Draw("Label");
}
