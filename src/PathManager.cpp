#include "PathManager.hpp"


PathManager::PathManager(const std::filesystem::path& root_dir)
    : root(root_dir),
        data_dir(root / "data"),
        hotkeyfile(data_dir / "hotkeys.txt"),
        vkfile(data_dir / "vkmap_grouped_numeric.json") {
            const char* appdata_env = std::getenv("APPDATA");  // AppData\Roaming
            if (!appdata_env) {
                throw std::runtime_error("APPDATA environment variable not found\r\n");
            }
            PATH APPDATA = PATH(appdata_env);
            initlogfile = APPDATA / "MyApp" / "log"  / "initlog.txt";
            dbfile = APPDATA / "MyApp" / "log"  / "keylog.db";
            errorfile = APPDATA / "MyApp" / "log"  / "errorlog.txt";
        }

const PATH& PathManager::getHotkeyFile() const {
    return hotkeyfile;
}

const PATH& PathManager::getVkFile() const {
    return vkfile;
}

const PATH& PathManager::getInitlogFile() const {
    return initlogfile;
}

const PATH& PathManager::getDbFile() const {
    return dbfile;
}

const PATH& PathManager::getErrorFile() const {
    return errorfile;
}

void PathManager::ensureDirectoriesExist() const {
    std::filesystem::create_directories(data_dir);
}