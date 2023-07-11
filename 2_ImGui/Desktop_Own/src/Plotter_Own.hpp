#pragma once

#include <cstdint>
#include <string_view>
#include <array>
#include <set>

#include "WindowBase.hpp"

class Plotter : public WindowBase
{
public:
    constexpr static auto functionNames = std::array<std::string_view, 3>{
        "unk",
        "sin(x)",
        "cos(x)",
    };
    enum class Function
    {
        NONE,
        SIN,
        COS,
    };


public:
    Plotter() : selectedFunctions({}){};
    virtual ~Plotter(){};
    virtual void Draw(std::string_view label, bool *open = nullptr) final;

private:
    void DrawSelection();
    void DrawPlot();

    Function functionNameMapping(std::string_view function_name);
    double evaluateFunction(const Function function, const double x);

public:
    std::set<Function> selectedFunctions;
};
