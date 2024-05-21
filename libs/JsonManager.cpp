#include <filesystem>
#include "JsonManager.h"

static std::filesystem::file_time_type lastTimeJsonModified = std::filesystem::file_time_type::min();
void JsonManager::CheckIfJsonModified(nlohmann::json& configJson)
{
	if (std::filesystem::last_write_time(JSON_FILE_PATH) != lastTimeJsonModified || configJson.empty())
	{
		InitilizeJsonSettings(configJson);
	}
}

void JsonManager::InitilizeJsonSettings(nlohmann::json& configJson)
{
	if (!std::filesystem::exists(JSON_FILE_PATH))
		throw std::runtime_error("JSON configuration file does not exist.");
	std::ifstream configFile(JSON_FILE_PATH);
	if (!configFile.is_open())
		throw std::runtime_error("Failed to open JSON configuration file.");
	LOG_DEBUG("JSON configuration file opened successfully.");

	try {
		configFile >> configJson;
	}
	catch (const nlohmann::json::parse_error& e) {
		std::string errorMessage = "JSON parsing error: " + std::string(e.what());
		LOG_ERROR(errorMessage);
	}
	lastTimeJsonModified = std::filesystem::last_write_time(JSON_FILE_PATH);

}
