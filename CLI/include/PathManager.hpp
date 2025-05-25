#pragma once
#include <filesystem>
#include <string>
#include <stdexcept>
#include "Types.hpp"

class PathManager {
public:
    PathManager(const std::filesystem::path& root_dir);
    const char* appdata = std::getenv("APPDATA");  // AppData\Roaming
    const PATH& getHotkeyFile() const;
    const PATH& getVkFile() const;
    const PATH& getInitlogFile() const;
    const PATH& getDbFile() const;
    const PATH& getErrorFile() const;

    void ensureDirectoriesExist() const;

private:
    PATH root;
    PATH data_dir;
    PATH hotkeyfile;
    PATH vkfile;
    PATH initlogfile;
    PATH dbfile;
    PATH errorfile;
};
