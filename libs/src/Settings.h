#pragma once
#include "LogManager.h"
#include <fstream>
#include <json.hpp>

#define JSON_FILE_PATH "Configurations.json"

class Settings
{
public:
	static void ReadSettings(nlohmann::json& configs);
	static void SaveSettings(nlohmann::json& configs);
};

