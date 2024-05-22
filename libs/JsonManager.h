#pragma once
#include "LogManager.h"
#include <fstream>
#include <json.hpp>
#define JSON_FILE_PATH "Configurations.json"

class JsonManager
{
private:

	std::ifstream configFile;

	void InitilizeJsonSettings();
public:
	nlohmann::json configJson;
	void CheckIfJsonModified();
};

