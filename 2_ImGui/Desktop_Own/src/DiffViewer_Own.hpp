#pragma once

#include <cstdint>
#include <string_view>
#include <vector>
#include <string>

#include "WindowBase.hpp"

class DiffViewer : public WindowBase
{
public:
    using FileContent = std::vector<std::string>;

public:
    DiffViewer() : filePath1("text1.txt"), filePath2("text2.txt"),
        fileContent1({}), fileContent2({}),
          diffResult1({}), diffResult2({})
    {}
    virtual void Draw(std::string_view label, bool *open = nullptr) final;

private:
    void DrawSelection();
    void DrawDiffView();
    void DrawStats();

    FileContent LoadFileContent(std::string_view file_path);
    void SaveFileContent(std::string_view file_path, FileContent fileContent);
    void CreateDiff();

private:
    std::string filePath1;
    std::string filePath2;

    FileContent fileContent1;
    FileContent fileContent2;

    FileContent diffResult1;
    FileContent diffResult2;
};

void render(DiffViewer &window_obj);
