#include <iostream>
#include <cstdint>
#include <string_view>
#include <array>
#include <map>
#include <vector>
#include <chrono>
#include <compare>
#include <algorithm>
#include <fstream>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "Calendar_Own.hpp"

void Calendar::Draw(std::string_view label, bool *open)
{
    ImGui::SetNextWindowSize(mainWindowSize);
    ImGui::SetNextWindowPos(mainWindowPos);

    ImGui::Begin(label.data(),
                 open,
                 mainWindowFlags);

    DrawDateCombo();
    ImGui::Separator();
    DrawCalendar();
    ImGui::Separator();
    DrawMeetingList();

    ImGui::End();

    if (addMeetingWindowOpen)
        DrawAddMeetingWindow();

}

void Calendar::DrawDateCombo()
{
    ImGui::Text("Select a date:");
    ImGui::PushItemWidth(50);
    if (ImGui::BeginCombo("##day", std::to_string(selectedDay).data()))
    {
        for (std::int32_t day_idx = 1; day_idx <= 31; ++day_idx)
        {
            const auto curr_date =
                std::chrono::year_month_day(std::chrono::year(selectedYear),
                                            std::chrono::month(selectedMonth),
                                            std::chrono::day(day_idx));
            if (!curr_date.ok())
                break;
            if (ImGui::Selectable(std::to_string(day_idx).data(),
                day_idx == selectedDay))
            {
                selectedDay = day_idx;
                selectedDate = curr_date;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(100);

    if (ImGui::BeginCombo("##month", monthNames[selectedMonth - 1].data()))
    {
        
        for (std::int32_t month_idx = 1; month_idx <= 12; ++month_idx)
        {
            const auto curr_date =
                std::chrono::year_month_day(std::chrono::year(selectedYear),
                                            std::chrono::month(month_idx),
                                            std::chrono::day(selectedDay));
            if (!curr_date.ok())
                continue;
            if (ImGui::Selectable(monthNames[month_idx - 1].data(),
                month_idx == selectedMonth))
            {
                selectedMonth = month_idx;
                selectedDate = curr_date;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60);

    if (ImGui::BeginCombo("##year", std::to_string(selectedYear).data()))
    {
        for (std::int32_t year_idx = minYear; year_idx <= maxYear; ++year_idx)
        {
            const auto curr_date =
                std::chrono::year_month_day(std::chrono::year(year_idx),
                                            std::chrono::month(selectedMonth),
                                            std::chrono::day(selectedDay));
            if (!curr_date.ok())
                continue;
            if (ImGui::Selectable(std::to_string(year_idx).data(),
                                  year_idx == selectedDay))
            {
                selectedYear = year_idx;
                selectedDate = curr_date;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    if (ImGui::Button("Add Meeting"))
        addMeetingWindowOpen = true;

}

void Calendar::DrawCalendar()
{
    const auto child_size = ImVec2(ImGui::GetContentRegionAvail().x, 400.0f);
    ImGui::BeginChild("###calendar", child_size, false);
    const auto original_font_size = ImGui::GetFontSize();
    ImGui::SetWindowFontScale(calendarFontSize);

    const auto curr_time =
        std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    const auto todays_date = std::chrono::year_month_day(curr_time);

    const auto y = selectedYear;
    for (std::int32_t m = 1; m <= 12; ++m)
    {
        ImGui::Text("%s", fmt::format("{:.3}", monthNames[m - 1]).data());
        ImGui::SameLine();

        for (std::int32_t d = 1; d <= 31; ++d)
        {
            const auto iteration_date =
                std::chrono::year_month_day(std::chrono::year(y),
                                            std::chrono::month(m),
                                            std::chrono::day(d));
            if (!iteration_date.ok())
                break;

            ImGui::SameLine();
            if (todays_date == iteration_date)
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "%d", d);
            else if (selectedDate == iteration_date)
                ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%d", d);
            else if (meetings.contains(iteration_date))
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "%d", d);
            else
                ImGui::Text("%d", d);

            if (ImGui::IsItemClicked())
            {
                selectedDate = iteration_date;
                UpdateSelectedDateVariables();
            }
        }
    }
    ImGui::SetWindowFontScale(original_font_size);
    ImGui::EndChild();
}

void Calendar::DrawAddMeetingWindow()
{
    static char meeting_name_buffer[128] = "...";

    
    ImGui::SetNextWindowSize(popUpSize);
    ImGui::SetNextWindowPos(popUpPos);

    ImGui::Begin("###addMeeting", &addMeetingWindowOpen, popUpFlags);

    ImGui::Text("Add meeting to %d.%s.%d",
                selectedDay,
                monthNames[selectedMonth - 1].data(),
                selectedYear);
    ImGui::InputText("Meeting Name",
                     meeting_name_buffer,
                     sizeof(meeting_name_buffer));
    if (ImGui::Button("Save"))
    {
        meetings[selectedDate].push_back(Meeting{meeting_name_buffer});
        std::memset(meeting_name_buffer, 0, sizeof(meeting_name_buffer));
        addMeetingWindowOpen = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
        addMeetingWindowOpen = false;
    }

    ImGui::End();
}

void Calendar::DrawMeetingList()
{
    if (meetings.empty())
    {
        ImGui::Text("No meetings at all.");
        return;
    }

    ImGui::Text("Meetings on %d.%s.%d: ",
                selectedDay,
                monthNames[selectedMonth - 1].data(),
                selectedYear);

    if (!meetings.contains(selectedDate) || meetings[selectedDate].empty())
    {
        ImGui::Text("No meetings for this day.");
        return;
    }

    for (const auto& meeting : meetings[selectedDate])
    {
        ImGui::BulletText("%s", meeting.name.data());
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            std::erase(meetings[selectedDate], meeting);
            return;
        }
    }
}

void Calendar::LoadMeetingsFromFile(std::string_view filename)
{
    auto in = std::ifstream(filename.data());

    if (!in.is_open())
        return;

    auto num_meetings = std::size_t{0};
    in.read(reinterpret_cast<char *>(&num_meetings), sizeof(num_meetings));

    for (std::size_t i = 0; i < num_meetings; ++i)
    {
        auto date = std::chrono::year_month_day{};
        in.read(reinterpret_cast<char *>(&date), sizeof(date));

        auto num_meetings_on_that_date = std::size_t{0};
        in.read(reinterpret_cast<char *>(&num_meetings_on_that_date),
                sizeof(num_meetings_on_that_date));

        for (std::size_t j = 0; j < num_meetings_on_that_date; ++j)
        {
            auto meeting = Meeting::Deserialize(in);
            meetings[date].push_back(meeting);
        }
    }

    in.close();


}

void Calendar::SaveMeetingsToFile(std::string_view filename)
{
    auto out = std::ofstream(filename.data(), std::ios::binary);

    if (!out.is_open())
        return;

    const auto meetings_count = meetings.size();
    out.write(reinterpret_cast<const char *>(&meetings_count),
              sizeof(meetings_count));
    for (const auto& [date, meetings_vec] : meetings)
    {
        out.write(reinterpret_cast<const char*>(&date), sizeof(date));

        const auto meetings_count_on_that_date = meetings_vec.size();
        out.write(reinterpret_cast<const char *>(&meetings_count_on_that_date),
                  sizeof(meetings_count_on_that_date));

        for (const auto& meeting : meetings_vec)
            meeting.Serialize(out);
    }
    out.close();
}

void Calendar::UpdateSelectedDateVariables()
{
    selectedDay = static_cast<int>(selectedDate.day().operator unsigned int());
    selectedMonth = static_cast<int>(selectedDate.month().operator unsigned int());
    selectedYear = static_cast<int>(selectedDate.year());
}

void render(Calendar &window_obj)
{
    window_obj.Draw("Calendar_Own");
}

void Calendar::Meeting::Serialize(std::ofstream &out) const
{
    const auto name_length = name.size();
    out.write(reinterpret_cast<const char *>(&name_length),
              sizeof(name_length));
    out.write(name.data(), static_cast<std::streamsize>(name.size()));
}

Calendar::Meeting Calendar::Meeting::Deserialize(std::ifstream &in)
{
    auto meeting = Meeting{};
    auto name_length = std::size_t{0};
    in.read(reinterpret_cast<char *>(&name_length), sizeof(name_length));

    meeting.name.resize(name_length);
    in.read(meeting.name.data(), static_cast<std::streamsize>(name_length));

    return meeting;
}
