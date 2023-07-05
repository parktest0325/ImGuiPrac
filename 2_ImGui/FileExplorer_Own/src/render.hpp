#pragma once

#include <cstdint>
#include <string_view>
#include <filesystem>

namespace fs = std::filesystem;

class WindowClass
{
public:
    WindowClass() : currentPath(fs::current_path()), selectedPath(fs::path{}) {};

    void Draw(std::string_view label);

private:
    void DrawMenu();
    void DrawContent();
    void DrawActions();
    void DrawFilter();

    void openFileWithDefaultEditor();
    void renameFilePopup();
    void deleteFilePopup();
    bool renameFile(const fs::path& oldPath, const fs::path& newPath);
    bool deleteFile(const fs::path& path);

private:
    fs::path currentPath;
    fs::path selectedPath;

    bool renameDialogOpen = false;
    bool deleteDialogOpen = false;
};

void render(WindowClass &window_obj);
