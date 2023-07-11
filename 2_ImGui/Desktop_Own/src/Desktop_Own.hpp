#pragma once

#include "WallClock_Own.hpp"
#include <fmt/format.h>
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>

class Desktop
{
public:
    constexpr static auto numIcons = std::uint32_t{10};

    struct Icon
    {
        Icon(std::string_view label_)
            : label(label_), position(ImVec2{}), popupOpen(false),
              clickCount(0){};

        void Draw();

        std::string label;
        ImVec2 position;
        bool popupOpen;
        std::uint32_t clickCount;
    };

public:
    Desktop() : icons({}), clock({})
    {
        icons.reserve(numIcons);
        for (std::uint32_t i = 0; i < numIcons; i++)
            icons.push_back(Icon{fmt::format("Icon{}", i)});
    }
    void Draw(std::string_view label);

private:
    void DrawDesktop();
    void DrawTaskbar();

    void ShowIconList(bool *open = nullptr);

private:
    std::vector<Icon> icons;
    Clock clock;
};

void render(Desktop &window_obj);
