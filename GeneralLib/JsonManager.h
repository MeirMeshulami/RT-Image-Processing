#pragma once
#include <fstream>
#include <json.hpp>
#include "LogManager.h"
#define JSON_FILE_PATH "Configurations.json"
class JsonManager
{
private:
	std::ifstream configFile;
	static void InitilizeJsonSettings(nlohmann::json& configJson);
public:
	
	static void CheckIfJsonModified(nlohmann::json& configJson);
};

