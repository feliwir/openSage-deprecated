#include "GameData.hpp"

std::map<std::string, std::shared_ptr<GameData::DialogEvent>> GameData::gd_dialogs;
std::map<std::string, std::shared_ptr<GameData::Video>> GameData::gd_videos;