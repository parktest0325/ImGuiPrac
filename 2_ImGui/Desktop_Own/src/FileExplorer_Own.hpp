#pragma once

#include <cstdint>
#include <string_view>
#include <filesystem>

#include "WindowBase.hpp"

namespace fs = std::filesystem;

class FileExplorer : public WindowBase
{
public:
    FileExplorer() : currentPath(fs::current_path()), selectedPath(fs::path{}) {};
    virtual ~FileExplorer(){};

    virtual void Draw(std::string_view label, bool *open = nullptr) final;

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

void render(FileExplorer &window_obj);
