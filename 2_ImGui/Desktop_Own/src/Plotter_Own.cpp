#include <iostream>
#include <cmath>
#include <set>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "Plotter_Own.hpp"

void Plotter::Draw(std::string_view label, bool *open)
{
    ImGui::SetNextWindowSize(mainWindowSize);
    ImGui::SetNextWindowPos(mainWindowPos);

    ImGui::Begin(label.data(), open, mainWindowFlags);

    DrawSelection();
    DrawPlot();

    ImGui::End();
}

void Plotter::DrawSelection()
{
    for (const auto func_name : functionNames)
    {
        const auto curr_function = functionNameMapping(func_name);
        auto selected = selectedFunctions.contains(curr_function);
        if (ImGui::Checkbox(func_name.data(), &selected)) {
            if (selected) {
                selectedFunctions.insert(curr_function);
            }
            else {
                selectedFunctions.erase(curr_function);
            }   
        }
    }
}

void Plotter::DrawPlot()
{
    static constexpr auto num_points = 10'000;
    static constexpr auto x_min = -100.0;
    static constexpr auto x_max = 100.0;
    static constexpr auto x_step = (x_max - x_min) / num_points;

    static auto xs = std::array<double, num_points>{};
    static auto ys = std::array<double, num_points>{};

    ImPlot::BeginPlot("###Plot", ImVec2(-1.0F, -1.0F), ImPlotFlags_NoTitle);
    
    if (selectedFunctions.size() == 0 ||
        (selectedFunctions.size() == 1 &&
         *selectedFunctions.begin() == Function::NONE))
    {
        ImPlot::EndPlot();
        return;
    }

    for (const auto& function : selectedFunctions)
    {
        auto x = x_min;
        for (std::size_t i = 0; i < num_points; ++i)
        {
            xs[i] = x;
            ys[i] = evaluateFunction(function, x);
            x += x_step;
        }

        const auto plot_label =
            fmt::format("##function{}", static_cast<int>(function));
        //ImPlot::PlotLine(plot_label.data(), xs.data(), ys.data(), num_points);
        ImPlot::PlotScatter(plot_label.data(), xs.data(), ys.data(), num_points);
    }
    ImPlot::EndPlot();
}

Plotter::Function Plotter::functionNameMapping(
    std::string_view function_name)
{
    if (function_name == std::string_view{"sin(x)"})
        return Plotter::Function::SIN;
    if (function_name == std::string_view{"cos(x)"})
        return Plotter::Function::COS;
    return Plotter::Function::NONE;
}

double Plotter::evaluateFunction(const Function function, const double x)
{
    switch (function)
    {
    case Function::SIN:
        return std::sin(x);

    case Function::COS:
        return std::cos(x);
    case Function::NONE:
    default:
        return 0.0;
    }
}
