#include "JsonManager.h"

void JsonManager::ReadSettings(nlohmann::json& configs) {
	if (!std::filesystem::exists(JSON_FILE_PATH))
		throw std::runtime_error("JSON configuration file does not exist.");

	std::ifstream configFile(JSON_FILE_PATH);
	if (!configFile.is_open())
		throw std::runtime_error("Failed to open JSON configuration file.");

	LOG_DEBUG("JSON configuration file opened successfully.");

	try {
		configFile >> configs;
	}
	catch (const nlohmann::json::parse_error& e) {
		std::string errorMessage = "JSON parsing error: " + std::string(e.what());
		LOG_ERROR(errorMessage);
		configFile.close();
		throw std::runtime_error(errorMessage);
	}
	configFile.close();

	LOG_DEBUG("JSON configuration file parsed successfully.");
}

void JsonManager::SaveSettings(nlohmann::json& configs) {
	std::ofstream outputFile(JSON_FILE_PATH);
	if (!outputFile.is_open()) {
		std::string errorMsg = "Failed to open JSON file for saving settings.";
		LOG_ERROR(errorMsg);
		throw std::runtime_error(errorMsg);
	}
	try
	{
		outputFile << configs.dump(4);
		outputFile.close();
		LOG_DEBUG("Settings has saved successfully.");
	}
	catch (const std::exception&)
	{
		std::string errorMsg = "Error while saving settings.";
		LOG_ERROR(errorMsg);
		throw std::runtime_error(errorMsg);
	}
}

