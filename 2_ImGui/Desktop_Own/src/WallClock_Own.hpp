#pragma once

#include <cstdint>
#include <string_view>
#include <numbers>
#include <tuple>

#include "WindowBase.hpp"

class Clock : public WindowBase
{
private:
    static constexpr auto PI = std::numbers::pi_v<float>;
    static constexpr auto circleRadius = 250.0f;
    static constexpr auto offset = PI / 2.0f;
    static constexpr auto innerRadius = 5.0f;

    static constexpr auto hrsClockHandLength = 0.75F;
    static constexpr auto minsClockHandLength = 0.85F;
    static constexpr auto secsClockHandLength = 0.95F;

    static constexpr auto hrsStrokesLength = 0.90f;
    static constexpr auto minsStrokesLength = 0.95f;


public:
    Clock() : secs(0), mins(0), hrs(0), center({}){};
    virtual void Draw(std::string_view label, bool *open = nullptr) final;

    void GetTime();

private:
    void DrawCircle(const float radius);
    void DrawClockHand(const float radius,
                       const float theta,
                       const ImColor color);
    void DrawAllHourStrokes();
    void DrawAllMinuteStrokes();
    void DrawDigitalClock();

    std::tuple<float, float, float> GetTheta();

public:
    std::int32_t secs;
    std::int32_t mins;
    std::int32_t hrs;

private:
    ImVec2 center;
};

