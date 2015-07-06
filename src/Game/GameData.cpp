// Stephan Vedder 2015
#include "GameData.hpp"

const std::string GameData::dataDIR = "data/";
const std::string GameData::artDIR = "art/";
const std::string GameData::videoDIR = dataDIR + "movies/";
const std::string GameData::speechDIR = dataDIR + "audio/speech/";
const std::string GameData::compiledtexDIR = artDIR + "compiledtextures/";

const std::string GameData::videoINI = dataDIR + "ini/video.ini";
const std::string GameData::speechINI = dataDIR + "ini/speech.ini";
const std::string GameData::languageINI = "language.ini";

std::map<std::string, std::shared_ptr<GameData::DialogEvent>> GameData::gd_dialogs;
std::map<std::string, std::shared_ptr<GameData::Video>> GameData::gd_videos;
std::map<std::string, std::shared_ptr<BigStream>> GameData::gd_fonts;
std::shared_ptr<GameData::Language> GameData::gd_language;
std::shared_ptr<BigStream> GameData::gd_defaultFont = nullptr;