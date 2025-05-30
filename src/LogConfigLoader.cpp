#include "../include/LogConfigLoader.hpp"

LogConfigLoader::LogConfigLoader(PATH filepath): filepath(filepath){}
int LogConfigLoader::lCounter = 0;
void LogConfigLoader::load(){
    FileAccess fileaccess;
    fileaccess.set_filename(filepath);
    lCounter = fileaccess.load_launchCounter();
}
int LogConfigLoader::getlCounter(){
    return lCounter;
}