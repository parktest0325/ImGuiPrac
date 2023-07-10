#pragma once

#include <cstdint>
#include <string_view>
#include <array>
#include <map>
#include <vector>
#include <chrono>
#include <compare>

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

    static constexpr auto monthNames =
        std::array<std::string_view, 12U>{"January",
                                          "February",
                                          "March",
                                          "April",
                                          "May",
                                          "June",
                                          "July",
                                          "August",
                                          "September",
                                          "October",
                                          "November",
                                          "December"};
    static constexpr auto minYear = 2000U;
    static constexpr auto maxYear = 2038U;
    struct Meeting
    {
        std::string name;

        void Serialize(std::ofstream& out) const;
        static Meeting Deserialize(std::ifstream& in);

        constexpr bool operator==(const Meeting& rhs) const
        { return name == rhs.name; }
    };

public:
    WindowClass() : meetings({})
    {
    }
    void Draw(std::string_view label);

public:
    void LoadMeetingsFromFile(std::string_view filename);
    void SaveMeetingsToFile(std::string_view filename);

private:
    void DrawDateCombo();
    void DrawCalendar();
    void DrawAddMeetingWindow();
    void DrawMeetingList();

    void UpdateSelectedDateVariables();

private:
    int selectedDay = 1;
    int selectedMonth = 1;
    int selectedYear = 2023;

    std::chrono::year_month_day selectedDate;

    bool addMeetingWindowOpen = false;

    float calendarFontSize = 2.0F;

    std::map<std::chrono::year_month_day, std::vector<Meeting>> meetings;
};

void render(WindowClass &window_obj);
