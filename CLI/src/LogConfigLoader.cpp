#include "../include/LogConfigLoader.hpp"

LogConfigLoader::LogConfigLoader(PATH filepath): filepath(filepath){}
int LogConfigLoader::lCounter = 0;
int LogConfigLoader::lid = 0;
void LogConfigLoader::load(){
    FileAccess fileaccess;
    fileaccess.set_filename(filepath);
    InitLogVar ilv = fileaccess.load_launchCounter();
    lCounter = ilv.launchCounter;
    lid = ilv.user_id;
}
int LogConfigLoader::getlCounter(){
    return lCounter;
}
int LogConfigLoader::getlId(){
    return lid;
}