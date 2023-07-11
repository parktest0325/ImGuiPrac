#pragma once

#include <fmt/format.h>
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>

#include "Calendar_Own.hpp"
#include "CsvTool_Own.hpp"
#include "DiffViewer_Own.hpp"
#include "FileExplorer_Own.hpp"
#include "OtherTopics.hpp"
#include "Paint_Own.hpp"
#include "Plotter_Own.hpp"
#include "TextEditor_Own.hpp"
#include "WallClock_Own.hpp"
#include "WindowBase.hpp"


class Desktop : public WindowBase
{
public:
    constexpr static auto numIcons = std::uint32_t{10};

    struct Icon
    {
        Icon(std::string_view label_, WindowBase *base_ = nullptr)
            : label(label_), position(ImVec2{}), popupOpen(false),
              clickCount(0), base(base_){};

        void Draw();

        std::string label;
        ImVec2 position;
        bool popupOpen;
        std::uint32_t clickCount;
        WindowBase *base;
    };

public:
    Desktop()
        : icons({}), plotter(), calendar(), diff_viewer(), file_explorer(),
          paint(), text_editor(), csv_editor(), clock()
    {
        icons.reserve(7);

        icons.push_back(Icon{"Plotter", &plotter});
        icons.push_back(Icon{"Calendar", &calendar});
        icons.push_back(Icon{"DiffViewer", &diff_viewer});
        icons.push_back(Icon{"FileExplorer", &file_explorer});
        icons.push_back(Icon{"Paint", &paint});
        icons.push_back(Icon{"TextEditor", &text_editor});
        icons.push_back(Icon{"CsvTool", &csv_editor});
        icons.push_back(Icon{"OtherTopics", &other_topics});

        LoadTheme();
    };
    virtual void Draw(std::string_view label, bool *open = nullptr) final;

private:
    void DrawDesktop();
    void DrawTaskbar();

    void ShowIconList(bool *open = nullptr);

private:
    std::vector<Icon> icons;

    Plotter plotter;
    Calendar calendar;
    DiffViewer diff_viewer;
    FileExplorer file_explorer;
    Paint paint;
    TextEditor text_editor;
    CsvTool csv_editor;
    OtherTopics other_topics;

    Clock clock;
};

void render(Desktop &window_obj);
